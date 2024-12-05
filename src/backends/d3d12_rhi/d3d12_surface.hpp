#pragma once

#include "rhi_render_surface.hpp"

/**
 * The surface is not required for d3d12 but still keeping it here for compatibility with the api
 */

namespace avio::dx12 {

  RhiSurface* d3d12_create_surface(RHI* rhi, const infos::RhiSurfaceInfo& info);
  void d3d12_destroy_surface(RHI* rhi, RhiSurface* surface);

#ifdef AVIO_ENABLE_GLFW
  RhiSurface* d3d12_create_surface_glfw(RHI* rhi, GLFWwindow* window);
#endif

}  // namespace avio::dx12
