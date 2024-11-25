#pragma once

#include "rhi_render_surface.hpp"
#include "vulkan_common.hpp"

namespace avio {
  struct VulkanSurface {
    RhiSurface base;

    vk::SurfaceKHR vulkan_surface;
  };

  RhiSurface* vulkan_create_surface(RHI* rhi, const infos::RhiSurfaceInfo& info);
  void vulkan_destroy_surface(RHI* rhi, RhiSurface* surface);
}