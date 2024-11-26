#include "vulkan_rhi.hpp"
#include "vulkan_surface.hpp"

#include <array>
#include <ranges>
#include <span>

#define AV_REQUIRE_DESCRIPTOR_BUFFERS 0

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
#if defined(WIN32)
      "VK_KHR_surface",
      "VK_KHR_win32_surface",
#endif

#if defined(AVIO_ENABLE_GPU_VALIDATION)
      "VK_EXT_debug_utils",
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
static VkBool32 vulkan_callback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageTypes,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData) {
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
static void assert_instance_extensions_supported(
    std::span<const char* const> extensions) {
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
static void assert_valication_layers_supported(
    std::span<const char* const> layers) {
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
static void create_vulkan_instance(RhiVulkan* vulkan,
                                   const infos::RHIInfo& info);
static void pick_suitable_physical_device(RhiVulkan* vulkan,
                                          const infos::RHIInfo& info);

static void create_vulkan_device(RhiVulkan* vulkan, const infos::RHIInfo& info);

bool vulkan_rhi_init(RHI* rhi, const infos::RHIInfo& info) {
  auto vulkan = cast_rhi<RhiVulkan>(rhi);
  create_vulkan_instance(vulkan, info);
  pick_suitable_physical_device(vulkan, info);

  if (vulkan->physical_device.device) {
    auto device_props = vulkan->physical_device.device.getProperties();
    AV_LOG(info, "Selected vulkan adapter: {}", device_props.deviceName.data());
  }

  create_vulkan_device(vulkan, info);

  return true;
}

// ---------------------------------------------------------------------------------------------
void vulkan_rhi_shutdown(RHI* rhi) {
  auto vulkan = cast_rhi<RhiVulkan>(rhi);

  // Destroy the device
  vulkan->graphics_queue.waitIdle();
  vulkan->device.waitIdle();
  vulkan->device.destroy();

  // Destroy the debug messenger
#ifdef AVIO_ENABLE_GPU_VALIDATION
  PFN_vkDestroyDebugUtilsMessengerEXT destroy_messenger_func =
      (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
          vulkan->instance, "vkDestroyDebugUtilsMessengerEXT");
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
                      .setApiVersion(VK_API_VERSION_1_1);

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
      (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
          vulkan->instance, "vkCreateDebugUtilsMessengerEXT");

  VkDebugUtilsMessengerEXT messenger;
  VkDebugUtilsMessengerCreateInfoEXT messenger_info_c = msg_info;
  VK_ASSERT(create_messenger_func(vulkan->instance, &messenger_info_c, nullptr,
                                  &messenger));
  vulkan->debug_messenger = messenger;
#endif
}

// ---------------------------------------------------------------------------------------------
static consteval auto get_required_device_extensions() noexcept {
  return std::array{
      VK_KHR_SWAPCHAIN_EXTENSION_NAME,

#if AV_REQUIRE_DESCRIPTOR_BUFFERS
      VK_EXT_DESCRIPTOR_BUFFER_EXTENSION_NAME,
#endif
  };
}

// ---------------------------------------------------------------------------------------------
static bool device_supports_extensions(
    vk::PhysicalDevice device, std::span<const char* const> extensions) {
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
static VulkanQueueFamilyIndices get_queue_family_indices(
    vk::PhysicalDevice device) {
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
void pick_suitable_physical_device(RhiVulkan* vulkan,
                                   const infos::RHIInfo& info) {
  auto physical_devices = vulkan->instance.enumeratePhysicalDevices();
  auto required_device_extensions = get_required_device_extensions();
  for (const vk::PhysicalDevice& device : physical_devices) {
    const bool supports_extensions =
        device_supports_extensions(device, required_device_extensions);

    const VulkanQueueFamilyIndices queue_indices =
        get_queue_family_indices(device);
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
void create_vulkan_device(RhiVulkan* vulkan, const infos::RHIInfo& info) {
  std::vector<vk::DeviceQueueCreateInfo> queue_infos{};
  float queue_priority = 1.0f;
  for (const uint32_t queue_index :
       vulkan->physical_device.queue_indices.as_unique_set()) {
    queue_infos.push_back(vk::DeviceQueueCreateInfo()
                              .setPQueuePriorities(&queue_priority)
                              .setQueueCount(1)
                              .setQueueFamilyIndex(queue_index));
  }

  auto required_device_extensions = get_required_device_extensions();
  auto create_info = vk::DeviceCreateInfo()
                         .setPEnabledExtensionNames(required_device_extensions)
                         .setQueueCreateInfos(queue_infos);

  vulkan->device = vulkan->physical_device.device.createDevice(create_info);
  AV_LOG(info, "Vulkan logical device created.");

  // Pick queues
  vulkan->graphics_queue = vulkan->device.getQueue(
      vulkan->physical_device.queue_indices.graphics, 0);
  AV_LOG(info, "Vulkan graphics queue initialized.");
}

// ---------------------------------------------------------------------------------------------
RHI* get_rhi_vulkan() {
  return &g_rhi_vulkan.base;
}

void init_global_rhi_pointers() {
  // Default functions
  get_rhi = get_rhi_vulkan;

  // Surface functions
  rhi_create_surface = vulkan_create_surface;
  rhi_destroy_surface = vulkan_destroy_surface;
}

}  // namespace avio::vulkan
