#pragma once

#include "core.hpp"
#include "rhi_render_surface.hpp"

namespace avio {
  namespace infos {

    struct RhiSwapchainInfo {};

  }  // namespace infos

  struct RhiSwapchain {
    RhiSurface* surface;
  };
}  // namespace avio