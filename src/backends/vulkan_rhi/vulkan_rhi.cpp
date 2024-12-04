#include "vulkan_rhi.hpp"
#include "vulkan_render_commands.hpp"
#include "vulkan_surface.hpp"
#include "vulkan_swapchain.hpp"
#include "rhi_interface.hpp"

#include <array>
#include <ranges>
#include <span>

namespace avio::vulkan {

  static bool vulkan_rhi_init(RHI* rhi, const infos::RHIInfo& info);
  static void vulkan_rhi_shutdown(RHI* rhi);

  // ---------------------------------------------------------------------------------------------
  RhiVulkan g_rhi_vulkan{
      .base =
          {
              .init_impl = vulkan_rhi_init,
              .shutdown_impl = vulkan_rhi_shutdown,
          },
  };

  // ---------------------------------------------------------------------------------------------
  static consteval auto get_required_instance_extensions() noexcept {
    return std::array{
        "VK_KHR_surface",

#if defined(WIN32)
        "VK_KHR_win32_surface",
#endif

#if AV_VK_USE_DYNAMIC_RENDERING
        VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME,
#endif

#if defined(AVIO_ENABLE_GPU_VALIDATION)
        "VK_EXT_debug_utils",
#endif
    };
  }

    // ---------------------------------------------------------------------------------------------
  static consteval auto get_required_device_extensions() noexcept {
    return std::array{
        VK_KHR_SWAPCHAIN_EXTENSION_NAME,
        VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME,

#if AV_VK_USE_DYNAMIC_RENDERING
        VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME,
#endif

#if AV_REQUIRE_DESCRIPTOR_BUFFERS
        VK_EXT_DESCRIPTOR_BUFFER_EXTENSION_NAME,
#endif
    };
  }

  // ---------------------------------------------------------------------------------------------
  static consteval auto get_required_layer_names() noexcept {
#ifdef AVIO_ENABLE_GPU_VALIDATION
    return std::array{"VK_LAYER_KHRONOS_validation"};
#else
    return std::array<const char*, 0>{};
#endif
  }

  // ---------------------------------------------------------------------------------------------
  static VkBool32 vulkan_callback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                  VkDebugUtilsMessageTypeFlagsEXT messageTypes,
                                  const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {
    switch (messageSeverity) {
      case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
        AV_LOG(error, "Vulkan validation: {}", pCallbackData->pMessage);
        break;
      case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
        AV_LOG(info, "Vulkan validation: {}", pCallbackData->pMessage);
        break;
      case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
        AV_LOG(warn, "Vulkan validation: {}", pCallbackData->pMessage);
        break;
      default:
        break;
    }

    return VK_FALSE;
  }

  // ---------------------------------------------------------------------------------------------
  static void assert_instance_extensions_supported(std::span<const char* const> extensions) {
    auto supported_extensions = vk::enumerateInstanceExtensionProperties();
    for (const auto& required : extensions) {
      bool is_supported = false;
      for (const auto& supported : supported_extensions) {
        if (strcmp(required, supported.extensionName.data()) == 0) {
          is_supported = true;
          AV_LOG(trace, "Vulkan instance extension '{}' is supported.", required);
          break;
        }
      }

      if (!is_supported) {
        throw Error("Vulkan instance extension '{}' is unsupported", required);
      }
    }
  }

  // ---------------------------------------------------------------------------------------------
  static void assert_valication_layers_supported(std::span<const char* const> layers) {
    if (layers.empty()) {
      return;
    }

    auto supported_layers = vk::enumerateInstanceLayerProperties();
    for (const auto& required : layers) {
      bool is_supported = false;
      for (const auto& supported : supported_layers) {
        if (strcmp(required, supported.layerName.data()) == 0) {
          is_supported = true;
          AV_LOG(trace, "Vulkan instance layer '{}' is supported.", required);
          break;
        }
      }

      if (!is_supported) {
        throw Error("Vulkan instance layer '{}' is unsupported", required);
      }
    }
  }

  // ---------------------------------------------------------------------------------------------
  static void create_rhi_sync(RhiVulkan* vulkan, const infos::RHIInfo& info) {
    for (uint8_t index = 0; index < RHI_NUM_FRAMES_IN_FLIGHT; ++index) {
      vulkan->render_finished_semaphores[index] = vulkan->device.createSemaphore({});
      vk::FenceCreateInfo fence_create_info{};
      fence_create_info.setFlags(vk::FenceCreateFlagBits::eSignaled);
      vulkan->in_flight_fences[index] = vulkan->device.createFence(fence_create_info);
    }
  }

  // ---------------------------------------------------------------------------------------------
  static void create_vulkan_instance(RhiVulkan* vulkan, const infos::RHIInfo& info);
  static void pick_suitable_physical_device(RhiVulkan* vulkan, const infos::RHIInfo& info);
  static void create_vulkan_device(RhiVulkan* vulkan, const infos::RHIInfo& info);
  static void create_vulkan_command_block(RhiVulkan* vulkan, const infos::RHIInfo& info);

  bool vulkan_rhi_init(RHI* rhi, const infos::RHIInfo& info) {
    auto vulkan = cast_rhi<RhiVulkan>(rhi);
    create_vulkan_instance(vulkan, info);
    pick_suitable_physical_device(vulkan, info);

    if (vulkan->physical_device.device) {
      auto device_props = vulkan->physical_device.device.getProperties();
      AV_LOG(info, "Selected vulkan adapter: {}", device_props.deviceName.data());
    }

    create_vulkan_device(vulkan, info);
    create_rhi_sync(vulkan, info);

    // Create command block
    create_vulkan_command_block(vulkan, info);

    return true;
  }

  // ---------------------------------------------------------------------------------------------
  void vulkan_rhi_shutdown(RHI* rhi) {
    auto vulkan = cast_rhi<RhiVulkan>(rhi);

    // Destroy the device
    vulkan->graphics_queue.waitIdle();
    vulkan->device.waitIdle();

    for (auto& fence : vulkan->in_flight_fences) {
      vulkan->device.destroy(fence);
    }

    // Destroy render finished semaphores
    for (auto& semaphore : vulkan->render_finished_semaphores) {
      vulkan->device.destroy(semaphore);
    }

    for (uint8_t index = 0; index < RHI_NUM_FRAMES_IN_FLIGHT; ++index) {
      vulkan->device.destroy(vulkan->command_pools[index]);
    }

    vulkan->device.destroy();

    // Destroy the debug messenger
#ifdef AVIO_ENABLE_GPU_VALIDATION
    PFN_vkDestroyDebugUtilsMessengerEXT destroy_messenger_func =
        (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(vulkan->instance, "vkDestroyDebugUtilsMessengerEXT");
    destroy_messenger_func(vulkan->instance, vulkan->debug_messenger, nullptr);
#endif

    // This should go last
    vulkan->instance.destroy();

    AV_LOG(info, "Vulkan RHI terminated.");
  }

  // ---------------------------------------------------------------------------------------------
  void create_vulkan_instance(RhiVulkan* vulkan, const infos::RHIInfo& info) {
    auto app_info = vk::ApplicationInfo()
                        .setPApplicationName("Avio")
                        .setApplicationVersion(VK_MAKE_API_VERSION(0, 1, 0, 0))
                        .setPEngineName("AvioEngine")
                        .setEngineVersion(VK_MAKE_API_VERSION(0, 1, 0, 0))
                        .setApiVersion(VK_API_VERSION_1_3);

    auto required_extensions = get_required_instance_extensions();

    // Check if all extensions are supported.
    assert_instance_extensions_supported(required_extensions);

    auto required_layers = get_required_layer_names();
    auto inst_info = vk::InstanceCreateInfo()
                         .setPApplicationInfo(&app_info)
                         .setPEnabledExtensionNames(required_extensions)
                         .setPEnabledLayerNames(required_layers);

#ifdef AVIO_ENABLE_GPU_VALIDATION
    vk::DebugUtilsMessengerCreateInfoEXT msg_info{};
    using enum vk::DebugUtilsMessageSeverityFlagBitsEXT;
    using enum vk::DebugUtilsMessageTypeFlagBitsEXT;
    msg_info.setMessageSeverity(eError | eInfo | eWarning)
        .setMessageType(eGeneral | ePerformance | eValidation)
        .setPfnUserCallback(&vulkan_callback);
    inst_info.setPNext(&msg_info);
#endif

    vulkan->instance = vk::createInstance(inst_info);
    AV_LOG(info, "Vulkan instance created.");

#ifdef AVIO_ENABLE_GPU_VALIDATION
    PFN_vkCreateDebugUtilsMessengerEXT create_messenger_func =
        (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(vulkan->instance, "vkCreateDebugUtilsMessengerEXT");

    VkDebugUtilsMessengerEXT messenger;
    VkDebugUtilsMessengerCreateInfoEXT messenger_info_c = msg_info;
    VK_ASSERT(create_messenger_func(vulkan->instance, &messenger_info_c, nullptr, &messenger));
    vulkan->debug_messenger = messenger;
#endif
  }

  // ---------------------------------------------------------------------------------------------
  static bool device_supports_extensions(vk::PhysicalDevice device, std::span<const char* const> extensions) {
    const auto supported_extensions = device.enumerateDeviceExtensionProperties();
    for (const auto& required : extensions) {
      bool is_supported = false;
      for (const auto& supported : supported_extensions) {
        if (strcmp(required, supported.extensionName.data()) == 0) {
          is_supported = true;
          break;
        }
      }

      if (!is_supported) {
        return false;
      }
    }
    return true;
  }

  // ---------------------------------------------------------------------------------------------
  static VulkanQueueFamilyIndices get_queue_family_indices(vk::PhysicalDevice device) {
    auto queue_props = device.getQueueFamilyProperties();
    VulkanQueueFamilyIndices indices = {};

    for (const auto& [idx, queue] : queue_props | std::views::enumerate) {
      if ((queue.queueFlags & vk::QueueFlagBits::eGraphics))
        indices.graphics = idx;

      if (indices.is_valid())
        return indices;
    }

    return {};
  }

  // ---------------------------------------------------------------------------------------------
  void pick_suitable_physical_device(RhiVulkan* vulkan, const infos::RHIInfo& info) {
    auto physical_devices = vulkan->instance.enumeratePhysicalDevices();
    auto required_device_extensions = get_required_device_extensions();
    for (const vk::PhysicalDevice& device : physical_devices) {
      const bool supports_extensions = device_supports_extensions(device, required_device_extensions);

      const VulkanQueueFamilyIndices queue_indices = get_queue_family_indices(device);
      const bool suitable = supports_extensions && queue_indices.is_valid();

      if (suitable) {
        vulkan->physical_device.device = device;
        vulkan->physical_device.queue_indices = queue_indices;
        break;
      }
    }

    if (!vulkan->physical_device.device) {
      throw Error("Failed to pick valid vulkan physical device.");
    }
  }

  // ---------------------------------------------------------------------------------------------
  static void create_vulkan_command_block(RhiVulkan* vulkan, const infos::RHIInfo& info) {
    // Create command pools
    vk::CommandPoolCreateInfo pool_info{};
    pool_info.setQueueFamilyIndex(vulkan->physical_device.queue_indices.graphics);

    for (uint8_t index = 0; index < RHI_NUM_FRAMES_IN_FLIGHT; ++index) {
      vulkan->command_pools[index] = vulkan->device.createCommandPool(pool_info);

      vk::CommandBufferAllocateInfo alloc_info{};
      alloc_info.setCommandPool(vulkan->command_pools[index])
          .setLevel(vk::CommandBufferLevel::ePrimary)
          .setCommandBufferCount(1);

      VK_ASSERT(vulkan->device.allocateCommandBuffers(&alloc_info, &vulkan->command_buffers[index]));
    }

    // Begin first command buffer
    vulkan->device.resetCommandPool(vulkan->command_pools[0]);

    vk::CommandBufferBeginInfo begin_info{};
    vulkan->command_buffers[0].begin(begin_info);
  }

  // ---------------------------------------------------------------------------------------------
  void create_vulkan_device(RhiVulkan* vulkan, const infos::RHIInfo& info) {
    std::vector<vk::DeviceQueueCreateInfo> queue_infos{};
    float queue_priority = 1.0f;
    for (const uint32_t queue_index : vulkan->physical_device.queue_indices.as_unique_set()) {
      queue_infos.push_back(vk::DeviceQueueCreateInfo()
                                .setPQueuePriorities(&queue_priority)
                                .setQueueCount(1)
                                .setQueueFamilyIndex(queue_index));
    }

    auto required_device_extensions = get_required_device_extensions();

    auto create_info =
        vk::DeviceCreateInfo().setPEnabledExtensionNames(required_device_extensions).setQueueCreateInfos(queue_infos);

#if AV_VK_USE_DYNAMIC_RENDERING
    VkPhysicalDeviceDynamicRenderingFeatures dynamic_rendering_features{};
    dynamic_rendering_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES;
    dynamic_rendering_features.dynamicRendering = VK_TRUE;
    create_info.setPNext(&dynamic_rendering_features);
#endif

    vulkan->device = vulkan->physical_device.device.createDevice(create_info);
    AV_LOG(info, "Vulkan logical device created.");

    // Pick queues
    vulkan->graphics_queue = vulkan->device.getQueue(vulkan->physical_device.queue_indices.graphics, 0);
    AV_LOG(info, "Vulkan graphics queue initialized.");
  }

  // ---------------------------------------------------------------------------------------------
  RHI* get_rhi_vulkan() {
    return &g_rhi_vulkan.base;
  }

  // ---------------------------------------------------------------------------------------------
  std::span<vk::Semaphore> vulkan_get_present_wait_semaphores(RhiVulkan* vulkan) {
    return std::span<vk::Semaphore>(&vulkan->render_finished_semaphores[vulkan->base.current_frame_in_flight],
                                    1 /* Select only single semaphore for now. */);
  }

  // ---------------------------------------------------------------------------------------------
  vk::CommandBuffer get_current_command_buffer(const RhiVulkan* rhi) {
    return rhi->command_buffers.at(rhi->base.current_frame_in_flight);
  }

  // ---------------------------------------------------------------------------------------------
  void vulkan_add_submit_wait_semaphore(RhiVulkan* vulkan, const WaitSemaphore& wait_semaphore) {
    std::lock_guard lck{vulkan->access_mutex};
    vulkan->submit_wait_semaphores[vulkan->base.current_frame_in_flight].push_back(wait_semaphore.semaphore);
    vulkan->submit_wait_stage_masks[vulkan->base.current_frame_in_flight].push_back(wait_semaphore.wait_dst_stage);
  }

  // ---------------------------------------------------------------------------------------------
  static void vulkan_rhi_submit_frame(RHI* rhi) {
    RhiVulkan* vulkan = cast_rhi<RhiVulkan>(rhi);

    // Close the current command buffer
    auto current_frame_in_flight = rhi->current_frame_in_flight;
    vulkan->command_buffers[rhi->current_frame_in_flight].end();

    vk::SubmitInfo submit_info{};

    submit_info.setPWaitSemaphores(vulkan->submit_wait_semaphores[current_frame_in_flight].data())
        .setPWaitDstStageMask(vulkan->submit_wait_stage_masks[current_frame_in_flight].data())
        .setWaitSemaphoreCount((uint32_t)vulkan->submit_wait_semaphores[current_frame_in_flight].size())
        .setSignalSemaphores(vulkan->render_finished_semaphores.at(current_frame_in_flight))
        .setCommandBuffers(vulkan->command_buffers[current_frame_in_flight]);

    vulkan->graphics_queue.submit(submit_info, vulkan->in_flight_fences.at(rhi->current_frame_in_flight));
    vulkan->submit_wait_semaphores[current_frame_in_flight].clear();
    vulkan->submit_wait_stage_masks[current_frame_in_flight].clear();
  }

  static void vulkan_begin_frame(RHI* rhi) {
    AV_ASSERT_MSG(!rhi->has_began_frame, "Failed to begin the frame. Did you forget to end the frame?");
    rhi->has_began_frame = true;
    auto vulkan = cast_rhi<RhiVulkan>(rhi);
    auto current_frame_in_flight = rhi->current_frame_in_flight;

    // Wait for the next frame to be done
    VK_ASSERT(vulkan->device.waitForFences(vulkan->in_flight_fences[current_frame_in_flight], VK_TRUE, UINT64_MAX));
    vulkan->device.resetFences(vulkan->in_flight_fences[current_frame_in_flight]);

    // Reset the command pool
    vulkan->device.resetCommandPool(vulkan->command_pools[current_frame_in_flight]);

    // open next command list
    vk::CommandBufferBeginInfo begin_info{};
    vulkan->command_buffers[rhi->current_frame_in_flight].begin(begin_info);

    // Acquire all swapchain images
    for (VulkanSwapchain& swapchain : vulkan->swapchains.objects) {
      if (swapchain.swapchain) {
        swapchain_acquire_next_image(vulkan, &swapchain);
      }
    }
  }

  static void vulkan_end_frame(RHI* rhi) {
    AV_ASSERT_MSG(rhi->has_began_frame, "Failed to end the frame. Did you forget to begin the frame?");
    rhi->has_began_frame = false;

    auto vulkan = cast_rhi<RhiVulkan>(rhi);

    // This happens at the end of the frame
    rhi->current_frame_in_flight = (rhi->current_frame_in_flight + 1) % RHI_NUM_FRAMES_IN_FLIGHT;
  }

  void init_global_rhi_pointers() {
    // Default functions
    funcs::get_rhi_ = get_rhi_vulkan;
    funcs::rhi_submit_frame_ = vulkan_rhi_submit_frame;
    funcs::rhi_begin_frame_ = vulkan_begin_frame;
    funcs::rhi_end_frame_ = vulkan_end_frame;

    // Surface functions
    funcs::rhi_create_surface_ = vulkan_create_surface;
    funcs::rhi_destroy_surface_ = vulkan_destroy_surface;

    // Swapchain functions
    funcs::rhi_create_swapchain_ = vulkan_create_swapchain;
    funcs::rhi_destroy_swapchain_ = vulkan_destroy_swapchain;
    funcs::rhi_present_swapchain_ = vulkan_present_swapchain;

    detail::init_cmd_pointers();
  }

}  // namespace avio::vulkan
