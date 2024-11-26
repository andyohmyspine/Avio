#pragma once

#include "rhi_swapchain.hpp"
#include "vulkan_common.hpp"

#include <variant>
#include <array>

namespace avio::vulkan {
  template<class T>
  using PerImageArray = std::variant<
    std::monostate,
    std::array<T, RHI_DEFAULT_SWAPCHAIN_IMAGE_COUNT>,
    std::vector<T>>; 

  struct VulkanSwapchain {
    RhiSwapchain base;

    vk::SwapchainKHR swapchain;
    vk::SurfaceFormatKHR surface_format;
    vk::PresentModeKHR present_mode;
    uint8_t current_image_index{};
    uint8_t image_count {};
    PerImageArray<vk::Semaphore> image_ready_semaphores;

    bool has_more_than_default_images : 1 = false;
  };

  RhiSwapchain* vulkan_create_swapchain(RHI* rhi, const infos::RhiSwapchainInfo& info);
  void vulkan_destroy_swapchain(RHI* rhi, RhiSwapchain* swapchain);
  void vulkan_present_swapchain(RHI* rhi, RhiSwapchain* swapchain);
}