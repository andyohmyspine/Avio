#pragma once

#include "core.hpp"
#include "rhi_types.hpp"

namespace avio {

  struct RHI;
  struct RhiSwapchain;

  RHI_FUNC_PTR(rhi_cmd_begin_draw_to_swapchain, void(*)(RHI* rhi, RhiSwapchain* swapchain, bool clear, Color clear_color));
  RHI_FUNC_PTR(rhi_cmd_end_draw_to_swapchain, void(*)(RHI* rhi, RhiSwapchain* swapchain));
}