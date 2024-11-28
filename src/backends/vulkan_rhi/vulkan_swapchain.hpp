#pragma once

#include "rhi_swapchain.hpp"
#include "vulkan_common.hpp"
#include "vulkan_image.hpp"

#include <array>
#include <variant>

namespace avio::vulkan {

  struct VulkanSwapchain {
    RhiSwapchain base;

    vk::SwapchainKHR swapchain;
    vk::SurfaceFormatKHR surface_format;
    vk::PresentModeKHR present_mode;
    vk::Extent2D extent;
    uint8_t current_image_index{};
    uint8_t image_count{};
    PerImageArray<vk::Semaphore> image_ready_semaphores;
    PerImageArray<VulkanImage> images;
    PerImageArray<VulkanImageView> image_views;

    bool has_more_than_default_images : 1 = false;
  };

  RhiSwapchain* vulkan_create_swapchain(RHI* rhi, const infos::RhiSwapchainInfo& info);
  void vulkan_destroy_swapchain(RHI* rhi, RhiSwapchain* swapchain);
  void vulkan_present_swapchain(RHI* rhi, RhiSwapchain* swapchain);
  void swapchain_acquire_next_image(RhiVulkan* rhi, VulkanSwapchain* swapchain);
}  // namespace avio::vulkan