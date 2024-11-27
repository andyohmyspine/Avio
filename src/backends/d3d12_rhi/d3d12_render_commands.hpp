#pragma once

#include "rhi_render_commands.hpp"

namespace avio::dx12 {
  void d3d12_cmd_begin_draw_to_swapchain(RHI* rhi, RhiSwapchain* swapchain);
  void d3d12_cmd_end_draw_to_swapchain(RHI* rhi, RhiSwapchain* swapchain);

  namespace detail {
    void init_cmd_pointers();
  }
}