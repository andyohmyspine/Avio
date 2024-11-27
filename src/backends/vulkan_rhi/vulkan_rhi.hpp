#pragma once

#include "vulkan_common.hpp"
#include "vulkan_surface.hpp"
#include "vulkan_swapchain.hpp"

#include "rhi.hpp"

#include <set>
#include <vector>
#include <mutex>

namespace avio::vulkan {

  struct VulkanQueueFamilyIndices {
    uint32_t graphics = UINT32_MAX;

    inline bool is_valid() const { return graphics != UINT32_MAX; }

    inline std::set<uint32_t> as_unique_set() const noexcept {
      return {graphics};
    }
  };

  struct WaitSemaphore {
    vk::Semaphore semaphore;
    vk::PipelineStageFlags wait_dst_stage;
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
    std::mutex access_mutex;

    ArrayPool<VulkanSurface, 16> surfaces;
    ArrayPool<VulkanSwapchain, 16> swapchains;

    // For now we will use the single submit per frame approach, though this isn't the best idea
    // in a long run.
    // So this solution is ok for now.
    std::array<vk::Semaphore, RHI_NUM_FRAMES_IN_FLIGHT> render_finished_semaphores;
    std::array<vk::Fence, RHI_NUM_FRAMES_IN_FLIGHT> in_flight_fences;

    std::array<vk::CommandPool, RHI_NUM_FRAMES_IN_FLIGHT> command_pools;
    std::array<vk::CommandBuffer, RHI_NUM_FRAMES_IN_FLIGHT> command_buffers;

    std::array<std::vector<vk::Semaphore>, RHI_NUM_FRAMES_IN_FLIGHT> submit_wait_semaphores;
    std::array<std::vector<vk::PipelineStageFlags>, RHI_NUM_FRAMES_IN_FLIGHT> submit_wait_stage_masks;
  };

  std::span<vk::Semaphore> vulkan_get_present_wait_semaphores(RhiVulkan* vulkan);
  vk::CommandBuffer get_current_command_buffer(const RhiVulkan* rhi);
  void vulkan_add_submit_wait_semaphore(RhiVulkan* vulkan, const WaitSemaphore& wait_semaphore);

  extern RhiVulkan g_rhi_vulkan;

  void init_global_rhi_pointers();

}  // namespace avio::vulkan