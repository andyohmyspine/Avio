#include "vulkan_render_commands.hpp"
#include "vulkan_rhi.hpp"

namespace avio::vulkan {
  struct RhiCmd {
    RhiVulkan* vulkan;
    vk::CommandBuffer cmd;
  };

  inline static RhiCmd get_cmd(RHI* rhi) {
    RhiVulkan* vulkan = cast_rhi<RhiVulkan>(rhi);
    return {vulkan, get_current_command_buffer(vulkan)};
  }

  // -------------------------------------------------------------------------------------------
  void vulkan_cmd_begin_draw_to_swapchain(RHI* rhi, RhiSwapchain* swapchain, bool clear, Color clear_color) {
    auto [vulkan, cmd] = get_cmd(rhi);
    auto vk_sc = cast_rhi<VulkanSwapchain>(swapchain);

    // Transition swapchain image
    VulkanImage& image = vk_sc->images[vk_sc->current_image_index];
    vk::ImageMemoryBarrier transition{};
    transition.setSrcAccessMask(vk::AccessFlagBits::eColorAttachmentRead)
        .setDstAccessMask(vk::AccessFlagBits::eColorAttachmentWrite)
        .setOldLayout(vk::ImageLayout::eUndefined)
        .setNewLayout(vk::ImageLayout::eColorAttachmentOptimal)
        .setSrcQueueFamilyIndex(vulkan->physical_device.queue_indices.graphics)
        .setDstQueueFamilyIndex(vulkan->physical_device.queue_indices.graphics)
        .setImage(image.image)
        .setSubresourceRange(vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1));
    cmd.pipelineBarrier(vk::PipelineStageFlagBits::eColorAttachmentOutput,
                        vk::PipelineStageFlagBits::eColorAttachmentOutput, vk::DependencyFlagBits::eByRegion, {}, {},
                        transition);

#if AV_VK_USE_DYNAMIC_RENDERING
    vk::RenderingAttachmentInfo image_attachment {};
    image_attachment.setImageView(vk_sc->image_views[vk_sc->current_image_index].view)
      .setImageLayout(vk::ImageLayout::eColorAttachmentOptimal)
      .setLoadOp(clear ? vk::AttachmentLoadOp::eClear : vk::AttachmentLoadOp::eLoad)
      .setStoreOp(vk::AttachmentStoreOp::eStore)
      .setClearValue(vk::ClearValue(vk::ClearColorValue(clear_color.r, clear_color.g, clear_color.b, clear_color.a)));

    vk::RenderingInfoKHR rendering_info{};
    rendering_info.setFlags({})
      .setRenderArea(vk::Rect2D({0, 0}, {image.base.width, image.base.height}))
      .setLayerCount(1)
      .setViewMask(0)
      .setColorAttachmentCount(1)
      .setColorAttachments(image_attachment);

    cmd.beginRendering(rendering_info); // Uncomment when implemented
#endif
  }

  // -------------------------------------------------------------------------------------------
  void vulkan_cmd_end_draw_to_swapchain(RHI* rhi, RhiSwapchain* swapchain) {
    auto [vulkan, cmd] = get_cmd(rhi);
    auto vk_sc = cast_rhi<VulkanSwapchain>(swapchain);

    cmd.endRendering();

    vk::Image image = vk_sc->images[vk_sc->current_image_index].image;
    vk::ImageMemoryBarrier transition{};
    transition.setSrcAccessMask(vk::AccessFlagBits::eColorAttachmentWrite)
        .setDstAccessMask(vk::AccessFlagBits::eColorAttachmentRead)
        .setOldLayout(vk::ImageLayout::eColorAttachmentOptimal)
        .setNewLayout(vk::ImageLayout::ePresentSrcKHR)
        .setSrcQueueFamilyIndex(vulkan->physical_device.queue_indices.graphics)
        .setDstQueueFamilyIndex(vulkan->physical_device.queue_indices.graphics)
        .setImage(image)
        .setSubresourceRange(vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1));
    cmd.pipelineBarrier(vk::PipelineStageFlagBits::eColorAttachmentOutput,
                        vk::PipelineStageFlagBits::eColorAttachmentOutput, vk::DependencyFlagBits::eByRegion, {}, {},
                        transition);
  }

  // -------------------------------------------------------------------------------------------
  // -------------------------------------------------------------------------------------------
  // -------------------------------------------------------------------------------------------
  void detail::init_cmd_pointers() {
    funcs::rhi_cmd_begin_draw_to_swapchain_ = vulkan_cmd_begin_draw_to_swapchain;
    funcs::rhi_cmd_end_draw_to_swapchain_ = vulkan_cmd_end_draw_to_swapchain;
  }
}  // namespace avio::vulkan
