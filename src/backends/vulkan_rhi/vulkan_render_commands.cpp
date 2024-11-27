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
  void vulkan_cmd_begin_draw_to_swapchain(RHI* rhi, RhiSwapchain* swapchain) {
    auto [vulkan, cmd] = get_cmd(rhi);
    auto vk_sc = cast_rhi<VulkanSwapchain>(swapchain);

    // Transition swapchain image
    vk::Image image = vk_sc->images[vk_sc->current_image_index];
    vk::ImageMemoryBarrier transition{};
    transition.setSrcAccessMask(vk::AccessFlagBits::eColorAttachmentRead)
        .setDstAccessMask(vk::AccessFlagBits::eColorAttachmentWrite)
        .setOldLayout(vk::ImageLayout::eUndefined)
        .setNewLayout(vk::ImageLayout::eColorAttachmentOptimal)
        .setSrcQueueFamilyIndex(vulkan->physical_device.queue_indices.graphics)
        .setDstQueueFamilyIndex(vulkan->physical_device.queue_indices.graphics)
        .setImage(image)
        .setSubresourceRange(vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1));
    cmd.pipelineBarrier(vk::PipelineStageFlagBits::eColorAttachmentOutput,
                        vk::PipelineStageFlagBits::eColorAttachmentOutput, vk::DependencyFlagBits::eByRegion, {}, {},
                        transition);
  }

  // -------------------------------------------------------------------------------------------
  void vulkan_cmd_end_draw_to_swapchain(RHI* rhi, RhiSwapchain* swapchain) {
    auto [vulkan, cmd] = get_cmd(rhi);
    auto vk_sc = cast_rhi<VulkanSwapchain>(swapchain);

    vk::Image image = vk_sc->images[vk_sc->current_image_index];
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
    rhi_cmd_begin_draw_to_swapchain = vulkan_cmd_begin_draw_to_swapchain;
    rhi_cmd_end_draw_to_swapchain = vulkan_cmd_end_draw_to_swapchain;
  }
}  // namespace avio::vulkan
