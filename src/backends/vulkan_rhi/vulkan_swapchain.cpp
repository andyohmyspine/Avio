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

  // ---------------------------------------------------------------------------------------------
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

  // ---------------------------------------------------------------------------------------------
  static void create_swapchain_semaphores(RhiVulkan* vulkan, VulkanSwapchain* swapchain) {
    uint32_t swapchain_image_count = 0;
    VK_ASSERT(vkGetSwapchainImagesKHR(vulkan->device, swapchain->swapchain, &swapchain_image_count, nullptr));
    swapchain->image_count = static_cast<uint8_t>(swapchain_image_count);

    if (swapchain->image_count > RHI_DEFAULT_SWAPCHAIN_IMAGE_COUNT) {
      swapchain->has_more_than_default_images = true;
      std::vector<vk::Semaphore> semaphores;
      semaphores.reserve(swapchain->image_count);

      for (uint8_t index = 0; index < swapchain->image_count; ++index) {
        vk::Semaphore semaphore = vulkan->device.createSemaphore({});
        semaphores.push_back(semaphore);
      }

      swapchain->image_ready_semaphores.assign(std::move(semaphores));
    } else {
      swapchain->has_more_than_default_images = false;
      std::array<vk::Semaphore, RHI_DEFAULT_SWAPCHAIN_IMAGE_COUNT> out_pool;
      for (uint8_t index = 0; index < swapchain->image_count; ++index) {
        out_pool[index] = vulkan->device.createSemaphore({});
      }
      swapchain->image_ready_semaphores.assign(std::move(out_pool));
    }
  }

  // ---------------------------------------------------------------------------------------------
  static vk::Semaphore get_image_ready_semaphore(VulkanSwapchain* swapchain, uint8_t index) {
    return swapchain->image_ready_semaphores[index];
  }

  // ---------------------------------------------------------------------------------------------
  void swapchain_acquire_next_image(RhiVulkan* rhi, VulkanSwapchain* swapchain) {
    if (swapchain->requires_resize) {
      swapchain->requires_resize = false;

      // Recreate the swapchain
      rhi->device.waitIdle();

      // First destroy the swapchain
      infos::RhiSwapchainInfo sc_info = swapchain->base.info;
      vulkan_destroy_swapchain(&rhi->base, &swapchain->base);

      // Then recreate the swapchain
      RhiSwapchain* new_sc = vulkan_create_swapchain(&rhi->base, sc_info);

      // Just copy a new swapchain to the old place. 
      *swapchain = std::move(*(VulkanSwapchain*)new_sc);
    }

    vk::Semaphore sem_to_signal = get_image_ready_semaphore(swapchain, swapchain->current_image_index);
    uint32_t new_image_index =
        rhi->device.acquireNextImageKHR(swapchain->swapchain, UINT64_MAX, sem_to_signal, VK_NULL_HANDLE).value;
    swapchain->current_image_index = (uint8_t)new_image_index;
    vulkan_add_submit_wait_semaphore(
        rhi,
        WaitSemaphore{.semaphore = sem_to_signal, .wait_dst_stage = vk::PipelineStageFlagBits::eColorAttachmentOutput});
  }

  // ---------------------------------------------------------------------------------------------
  static void create_swapchain_image_views(RhiVulkan* vulkan, VulkanSwapchain* swapchain) {
    auto images = vulkan->device.getSwapchainImagesKHR(swapchain->swapchain);

    if (swapchain->has_more_than_default_images) {
      swapchain->images.set_is_vector(images.size());
      swapchain->image_views.set_is_vector(images.size());
    } else {
      swapchain->images.set_is_array();
      swapchain->image_views.set_is_array();
    }

    for (uint32_t index = 0; index < swapchain->image_count; ++index) {
      VulkanImage out_image = {
          .base =
              {
                  .type = ImageType::image_2d,
                  .width = swapchain->extent.width,
                  .height = swapchain->extent.height,
                  .format = PixelFormat::window_output,
              },
          .image = images[index],
          .native_format = swapchain->surface_format.format,
      };
      swapchain->images[index] = out_image;

      // Create image view
      vk::ImageViewCreateInfo view_info{};
      view_info.setFlags({})
          .setImage(out_image.image)
          .setViewType(vk::ImageViewType::e2D)
          .setFormat(out_image.native_format)
          .setComponents(vk::ComponentMapping())
          .setSubresourceRange(vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1));

      VulkanImageView out_view{
          .base =
              {
                  .image = &swapchain->images[index].base,
                  .type = ImageViewType::color,
              },
          .view = vulkan->device.createImageView(view_info),
      };

      swapchain->image_views[index] = out_view;
    }
  }

  // ---------------------------------------------------------------------------------------------
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
    set_pixel_format_mapping(PixelFormat::window_output, surface_format.format);

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

    out_swapchain->extent = create_info.imageExtent;
    out_swapchain->swapchain = vulkan->device.createSwapchainKHR(create_info);
    create_swapchain_semaphores(vulkan, out_swapchain);

    create_swapchain_image_views(vulkan, out_swapchain);
    return &out_swapchain->base;
  }

  // ---------------------------------------------------------------------------------------------
  void vulkan_destroy_swapchain(RHI* rhi, RhiSwapchain* swapchain) {
    RhiVulkan* vulkan = cast_rhi<RhiVulkan>(rhi);
    VulkanSwapchain* vk_sc = cast_rhi<VulkanSwapchain>(swapchain);

    vulkan->device.waitIdle();

    for(auto index = 0; index < vk_sc->image_count; ++index) {
      vulkan->device.destroy(vk_sc->image_views[index].view);
    }

    // Clear semaphores
    if (vk_sc->has_more_than_default_images) {
      auto& semaphores = vk_sc->image_ready_semaphores.get_vector();
      for (vk::Semaphore sem : semaphores) {
        vulkan->device.destroy(sem);
      }
    } else {
      auto& semaphores = vk_sc->image_ready_semaphores.get_array();
      for (vk::Semaphore sem : semaphores) {
        vulkan->device.destroy(sem);
      }
    }

    if (vk_sc->swapchain) {
      // TODO: there will be a lot more code.
      vulkan->device.destroy(vk_sc->swapchain);
    }

    vulkan->swapchains.deallocate(vk_sc);
  }

  // ---------------------------------------------------------------------------------------------
  void vulkan_present_swapchain(RHI* rhi, RhiSwapchain* swapchain) {
    RhiVulkan* vulkan = cast_rhi<RhiVulkan>(rhi);
    VulkanSwapchain* vk_sc = cast_rhi<VulkanSwapchain>(swapchain);

    std::span<vk::Semaphore> present_wait_semaphores = vulkan_get_present_wait_semaphores(vulkan);
    vk::PresentInfoKHR present_info{};
    uint32_t image_index = vk_sc->current_image_index;
    present_info.setWaitSemaphores(present_wait_semaphores)
        .setSwapchains(vk_sc->swapchain)
        .setImageIndices(image_index);

    vk::Result present_result{};
    try {
      present_result = vulkan->graphics_queue.presentKHR(present_info);
      switch (present_result) {
        case vk::Result::eSuccess: {
          vk_sc->current_image_index = (vk_sc->current_image_index + 1) % vk_sc->image_count;
        } break;
        default:
          throw Error("Pressent failed: {}", vk::to_string(present_result));
      }
    } catch (const vk::OutOfDateKHRError& error) {
      // throw Error("Resizing swapchain is currently unsupported in vulkan. Pressent failed: {}",
      //            vk::to_string(present_result));

      vk_sc->requires_resize = true;
      log::info("Vulkan swapchain resize requested.");
    } catch (...) {
      throw;
    }
  }
}  // namespace avio::vulkan