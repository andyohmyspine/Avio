#pragma once

#include "rhi_swapchain.hpp"
#include "vulkan_common.hpp"

namespace avio::vulkan {
  struct VulkanSwapchain {
    RhiSwapchain base;

    vk::SwapchainKHR swapchain;
    vk::SurfaceFormatKHR surface_format;
    vk::PresentModeKHR present_mode;
    uint32_t current_image_index;
  };

  RhiSwapchain* vulkan_create_swapchain(RHI* rhi, const infos::RhiSwapchainInfo& info);
  void vulkan_destroy_swapchain(RHI* rhi, RhiSwapchain* swapchain);
  void vulkan_present_swapchain(RHI* rhi, RhiSwapchain* swapchain);
}