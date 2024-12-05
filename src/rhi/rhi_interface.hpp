#pragma once

#include "rhi.hpp"
#include "rhi_render_surface.hpp"
#include "rhi_shader_compiler.hpp"
#include "rhi_swapchain.hpp"
#include "rhi_render_commands.hpp"
#include "rhi_types.hpp"

namespace avio::funcs {
  // RHI FUNCTION DECLARATIONS

  RHI_FUNC_PTR(get_rhi, RHI* (*)());
  RHI_FUNC_PTR(rhi_begin_frame, void (*)(RHI* rhi));
  RHI_FUNC_PTR(rhi_end_frame, void (*)(RHI* rhi));
  RHI_FUNC_PTR(rhi_submit_frame, void (*)(RHI* rhi));

  RHI_FUNC_PTR(rhi_create_surface, RhiSurface* (*)(RHI* rhi, const infos::RhiSurfaceInfo& info));
  RHI_FUNC_PTR(rhi_destroy_surface, void (*)(RHI* rhi, RhiSurface* surface));

  RHI_FUNC_PTR(rhi_create_swapchain, RhiSwapchain* (*)(RHI* rhi, const infos::RhiSwapchainInfo& info));
  RHI_FUNC_PTR(rhi_destroy_swapchain, void (*)(RHI* rhi, RhiSwapchain* swapchain));
  RHI_FUNC_PTR(rhi_present_swapchain, void (*)(RHI* rhi, RhiSwapchain* swapchain));

  #ifdef AVIO_ENABLE_GLFW
  RHI_FUNC_PTR(rhi_create_surface_glfw, RhiSurface*(*)(RHI* rhi, GLFWwindow* window));
  #endif
}  // namespace avio::funcs

#include "rhi_interface_funcdefs.inl.hpp"