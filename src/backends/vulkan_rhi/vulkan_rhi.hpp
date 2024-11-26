#pragma once

#include "vulkan_common.hpp"
#include "vulkan_surface.hpp"
#include "vulkan_swapchain.hpp"

#include "rhi.hpp"

#include <set>

namespace avio::vulkan {

  struct VulkanQueueFamilyIndices {
    uint32_t graphics = UINT32_MAX;

    inline bool is_valid() const { return graphics != UINT32_MAX; }

    inline std::set<uint32_t> as_unique_set() const noexcept {
      return {graphics};
    }
  };

  struct VulkanPhysicalDevice {
    vk::PhysicalDevice device;
    VulkanQueueFamilyIndices queue_indices;
  };

  struct RhiVulkan {
    RHI base;

    vk::Instance instance;

#ifdef AVIO_ENABLE_GPU_VALIDATION
    vk::DebugUtilsMessengerEXT debug_messenger;
#endif

    VulkanPhysicalDevice physical_device;
    vk::Device device;
    vk::Queue graphics_queue;

    ArrayPool<VulkanSurface, 16> surfaces;
    ArrayPool<VulkanSwapchain, 16> swapchains;

    // For now we will use the single submit per frame approach, though this isn't the best idea
    // in a long run.
    // So this solution is ok for now.
    std::array<vk::Semaphore, RHI_NUM_FRAMES_IN_FLIGHT> render_finished_semaphores;
  };

  std::span<vk::Semaphore> vulkan_get_present_wait_semaphores(RhiVulkan* vulkan);

  extern RhiVulkan g_rhi_vulkan;

  void init_global_rhi_pointers();

}  // namespace avio::vulkan