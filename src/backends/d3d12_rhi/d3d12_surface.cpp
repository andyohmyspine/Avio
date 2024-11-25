#include "d3d12_surface.hpp"

namespace avio {

RhiSurface* d3d12_create_surface(RHI* rhi, const infos::RhiSurfaceInfo& info) {
  RhiSurface* surface = new RhiSurface {};
  surface->info = info;
  return surface;
}

void d3d12_destroy_surface(RHI* rhi, RhiSurface* surface) {
  delete surface;
}
}  // namespace avio