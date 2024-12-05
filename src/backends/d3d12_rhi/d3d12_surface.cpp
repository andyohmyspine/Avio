#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include "d3d12_surface.hpp"
#include "d3d12_rhi.hpp"

namespace avio::dx12 {

  RhiSurface* d3d12_create_surface(RHI* rhi, const infos::RhiSurfaceInfo& info) {
    RhiSurface* surface = cast_rhi<RhiD3D12>(rhi)->surfaces.allocate();
    surface->info = info;
    return surface;
  }

  void d3d12_destroy_surface(RHI* rhi, RhiSurface* surface) {
    cast_rhi<RhiD3D12>(rhi)->surfaces.deallocate(surface);
  }

#ifdef AVIO_ENABLE_GLFW
  RhiSurface* d3d12_create_surface_glfw(RHI* rhi, GLFWwindow* window) {
    RhiSurface* surface = cast_rhi<RhiD3D12>(rhi)->surfaces.allocate();
    surface->info.hwnd = glfwGetWin32Window(window);
    return surface;
  }
#endif
}  // namespace avio::dx12