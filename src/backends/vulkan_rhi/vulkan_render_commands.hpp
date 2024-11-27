#pragma once

#include "rhi_render_commands.hpp"

namespace avio::vulkan {
  void vulkan_cmd_begin_draw_to_swapchain(RHI* rhi, RhiSwapchain* swapchain);
  void vulkan_cmd_end_draw_to_swapchain(RHI* rhi, RhiSwapchain* swapchain);

  namespace detail {
    void init_cmd_pointers();
  }
}