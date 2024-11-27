#pragma once

#include "core.hpp"

namespace avio {

  struct RHI;
  struct RhiSwapchain;

  using PFN_rhi_cmd_begin_draw_to_swapchain = void(*)(RHI* rhi, RhiSwapchain* swapchain);
  inline PFN_rhi_cmd_begin_draw_to_swapchain rhi_cmd_begin_draw_to_swapchain;

  using PFN_rhi_cmd_end_draw_to_swapchain = void(*)(RHI* rhi, RhiSwapchain* swapchain);
  inline PFN_rhi_cmd_end_draw_to_swapchain rhi_cmd_end_draw_to_swapchain;
}