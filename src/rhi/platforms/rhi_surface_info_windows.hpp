#pragma once

#if defined(WIN32) || defined(_WIN32)

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

namespace avio::infos {
  struct RhiSurfaceInfo {
    HWND hwnd;
  };
}

#endif