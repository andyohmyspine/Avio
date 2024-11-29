#pragma once

#include "rhi_render_commands.hpp"

namespace avio::vulkan {
  void vulkan_cmd_begin_draw_to_swapchain(RHI* rhi, RhiSwapchain* swapchain, bool clear = true, Color clear_color = Color(0.2f, 0.2f, 0.2f, 1.0f));
  void vulkan_cmd_end_draw_to_swapchain(RHI* rhi, RhiSwapchain* swapchain);

  namespace detail {
    void init_cmd_pointers();
  }
}