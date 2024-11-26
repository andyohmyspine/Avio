#include "vulkan_swapchain.hpp"
#include "vulkan_rhi.hpp"

#include <ranges>

namespace avio::vulkan {
  inline constexpr vk::ColorSpaceKHR AVIO_COLOR_SPACE = vk::ColorSpaceKHR::eSrgbNonlinear;
  inline constexpr vk::SurfaceFormatKHR AVIO_PREFERRED_SWAPCHAIN_FORMAT = {vk::Format::eR8G8B8A8Unorm,
                                                                           AVIO_COLOR_SPACE};
  inline constexpr vk::SurfaceFormatKHR AVIO_FALLBACK_SWAPCHAIN_FORMAT = {vk::Format::eB8G8R8A8Unorm, AVIO_COLOR_SPACE};

  inline constexpr vk::PresentModeKHR AVIO_PREFERRED_PRESENT_MODE = vk::PresentModeKHR::eMailbox;
  inline constexpr vk::PresentModeKHR AVIO_FALLBACK_PRESENT_MODE = vk::PresentModeKHR::eFifo;

  static vk::SurfaceFormatKHR pick_surface_format(RhiVulkan* vulkan, const VulkanSurface* surface) {
    const auto supported_formats = vulkan->physical_device.device.getSurfaceFormatsKHR(surface->vulkan_surface);
    const bool supports_preferred = std::ranges::contains(supported_formats, AVIO_PREFERRED_SWAPCHAIN_FORMAT);
    const bool supports_fallback = std::ranges::contains(supported_formats, AVIO_FALLBACK_SWAPCHAIN_FORMAT);

    if (supports_preferred) {
      return AVIO_PREFERRED_SWAPCHAIN_FORMAT;
    } else if (supports_fallback) {
      return AVIO_FALLBACK_SWAPCHAIN_FORMAT;
    } else {
      throw Error(
          "None of the required swapchain formats found in surface "
          "capabilities.");
    }

    return {};
  }

  static vk::PresentModeKHR pick_present_mode(RhiVulkan* vulkan, const VulkanSurface* surface, bool vsync) {
    const auto present_modes = vulkan->physical_device.device.getSurfacePresentModesKHR(surface->vulkan_surface);
    if (vsync) {
      return vk::PresentModeKHR::eFifo;
    }

    const bool contains_preferred = std::ranges::contains(present_modes, AVIO_PREFERRED_PRESENT_MODE);
    const bool contains_fallback = std::ranges::contains(present_modes, AVIO_FALLBACK_PRESENT_MODE);

    if (contains_preferred)
      return AVIO_PREFERRED_PRESENT_MODE;
    else if (contains_fallback)
      return AVIO_FALLBACK_PRESENT_MODE;

    return vk::PresentModeKHR::eFifo;
  }

  static void swapchain_acquire_next_image(RhiVulkan* rhi, VulkanSwapchain* swapchain) {

  }

  RhiSwapchain* vulkan_create_swapchain(RHI* rhi, const infos::RhiSwapchainInfo& info) {
    RhiVulkan* vulkan = cast_rhi<RhiVulkan>(rhi);
    VulkanSwapchain* out_swapchain = vulkan->swapchains.allocate();
    out_swapchain->base.info = info;

    // Get the surface and capabilities
    const VulkanSurface* vulkan_surface = cast_rhi<VulkanSurface>(info.surface);
    const vk::SurfaceCapabilitiesKHR surface_caps =
        vulkan->physical_device.device.getSurfaceCapabilitiesKHR(vulkan_surface->vulkan_surface);

    // Get surface format
    const vk::SurfaceFormatKHR surface_format = pick_surface_format(vulkan, vulkan_surface);
    out_swapchain->surface_format = surface_format;

    // Get present mode
    const vk::PresentModeKHR present_mode = pick_present_mode(vulkan, vulkan_surface, info.allow_vsync);
    out_swapchain->present_mode = present_mode;

    // Create info for the swapchain
    vk::SwapchainCreateInfoKHR create_info{};
    create_info.setImageExtent(surface_caps.currentExtent)
        .setSurface(cast_rhi<VulkanSurface>(out_swapchain->base.info.surface)->vulkan_surface)
        .setImageFormat(surface_format.format)
        .setImageColorSpace(surface_format.colorSpace)
        .setPresentMode(present_mode)
        .setMinImageCount(info.override_image_count > 0 ? info.override_image_count : RHI_DEFAULT_SWAPCHAIN_IMAGE_COUNT)
        .setImageArrayLayers(1)
        .setImageUsage(vk::ImageUsageFlagBits::eColorAttachment)
        .setImageSharingMode(vk::SharingMode::eExclusive)
        .setPreTransform(surface_caps.currentTransform)
        .setCompositeAlpha(vk::CompositeAlphaFlagBitsKHR::eOpaque)
        .setClipped(false)
        .setOldSwapchain(VK_NULL_HANDLE);

    out_swapchain->swapchain = vulkan->device.createSwapchainKHR(create_info);

    // Acquire the first image of the swapchain.
    swapchain_acquire_next_image(vulkan, out_swapchain);

    return &out_swapchain->base;
  }
  void vulkan_destroy_swapchain(RHI* rhi, RhiSwapchain* swapchain) {
    RhiVulkan* vulkan = cast_rhi<RhiVulkan>(rhi);
    VulkanSwapchain* vk_sc = cast_rhi<VulkanSwapchain>(swapchain);

    vulkan->device.waitIdle();

    if (vk_sc->swapchain) {
      // TODO: there will be a lot more code.
      vulkan->device.destroy(vk_sc->swapchain);
    }

    vulkan->swapchains.deallocate(vk_sc);
  }

  void vulkan_present_swapchain(RHI* rhi, RhiSwapchain* swapchain) {
    RhiVulkan* vulkan = cast_rhi<RhiVulkan>(rhi);
    VulkanSwapchain* vk_sc = cast_rhi<VulkanSwapchain>(swapchain);
  }
}  // namespace avio::vulkan