#pragma once

#if defined(__linux__)

#ifdef AVIO_X11_AVAILABLE
  #include <X11/Xlib.h>
#endif

namespace avio::infos {
  enum class LinuxSurfaceType {
#ifdef AVIO_X11_AVAILABLE
    x11,
#endif

#ifdef AVIO_WAYLAND_AVAILABLE
    wayland,
#endif
    none,
  };

  struct RhiSurfaceInfo {
    LinuxSurfaceType surface_type = LinuxSurfaceType::none;

    union 
    {
      struct {
        Display* x11_display;
        Window x11_window;
      } x11;
    };
  };
}  // namespace avio::infos

#endif