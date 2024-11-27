#include "d3d12_render_commands.hpp"

namespace avio::dx12 {
  void d3d12_cmd_begin_draw_to_swapchain(RHI* rhi, RhiSwapchain* swapchain) {}
  void d3d12_cmd_end_draw_to_swapchain(RHI* rhi, RhiSwapchain* swapchain) {}

  void detail::init_cmd_pointers() {
    rhi_cmd_begin_draw_to_swapchain = d3d12_cmd_begin_draw_to_swapchain;
    rhi_cmd_end_draw_to_swapchain = d3d12_cmd_end_draw_to_swapchain;
  }
}  // namespace avio::dx12