#pragma once

#include "core.hpp"

// ----------------------------------------------------
#if defined(WIN32)
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

namespace avio::infos {
  namespace detail {

    struct RhiSurfaceInfoWin32 {
      HWND hwnd;
    };

  }  // namespace detail
  using RhiSurfaceInfo = detail::RhiSurfaceInfoWin32;
}  // namespace avio::infos

// ----------------------------------------------------
#elif defined(__linux__)
#error "Unsupported platform"
namespace avio::infos {

  namespace detail {
    struct RhiSurfaceInfoLinux {}
  }  // namespace detail

  using RhiSurfaceInfo = detail::RhiSurfaceInfoLinux;
}  // namespace avio::infos

#elif defined(__APPLE__)
#error "Unsupported platform"
namespace avio::infos {

  namespace detail {
    struct RhiSurfaceInfoApple {}
  }  // namespace detail

  using RhiSurfaceInfo = detail::RhiSurfaceInfoApple;
}  // namespace avio::infos
#endif

namespace avio {
  struct RHI;
  struct RhiSurface {
    infos::RhiSurfaceInfo info;
  };
  using PFN_rhi_create_surface =
      RhiSurface* (*)(RHI* rhi, const infos::RhiSurfaceInfo& info);

  /**
 * @brief Create RHI surface.
 * @param rhi RHI to pass in
 * @param window Native window handle.
 */
  inline PFN_rhi_create_surface rhi_create_surface;

  using PFN_rhi_destroy_surface = void (*)(RHI* rhi, RhiSurface* surface);
  inline PFN_rhi_destroy_surface rhi_destroy_surface;

}  // namespace avio