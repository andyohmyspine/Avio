#pragma once

#include "core.hpp"

namespace avio {
#ifdef WIN32
#define AVIO_SURFACE_WIN32
extern "C" typedef struct HWND__* HWND;
using NativeWindow = HWND;
#endif
}  // namespace avio

namespace avio {
struct RhiSurface {
  NativeWindow window;
};

RhiSurface* create_surface(NativeWindow window);

}  // namespace avio