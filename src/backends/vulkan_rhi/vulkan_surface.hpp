#pragma once

#include "rhi_render_surface.hpp"
#include "vulkan_common.hpp"

namespace avio::vulkan {
  struct VulkanSurface {
    RhiSurface base;

    vk::SurfaceKHR vulkan_surface;
  };

  RhiSurface* vulkan_create_surface(RHI* rhi,
                                    const infos::RhiSurfaceInfo& info);

#ifdef AVIO_ENABLE_GLFW
  RhiSurface* vulkan_create_surface_glfw(RHI* rhi, GLFWwindow* window);
#endif

  void vulkan_destroy_surface(RHI* rhi, RhiSurface* surface);
}  // namespace avio::vulkan