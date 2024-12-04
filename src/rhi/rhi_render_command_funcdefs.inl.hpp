#pragma once

namespace avio::rhi::cmd {
  inline void begin_draw_to_swapchain(RHI* rhi, RhiSwapchain* swapchain, bool clear = true,
                                      Color clear_color = avio::colors::black) {
    avio::funcs::rhi_cmd_begin_draw_to_swapchain_(rhi, swapchain, clear, clear_color);
  }

  inline void end_draw_to_swapchain(RHI* rhi, RhiSwapchain* swapchain) {
    avio::funcs::rhi_cmd_end_draw_to_swapchain_(rhi, swapchain);
  }
}  // namespace avio::rhi::cmd