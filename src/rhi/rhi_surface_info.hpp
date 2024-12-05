#pragma once

// ----------------------------------------------------
#if defined(WIN32)
#include "platforms/rhi_surface_info_windows.hpp"
#elif defined(__linux__)
#include "platforms/rhi_surface_info_linux.hpp"
#elif defined(__APPLE__)
#include "platforms/rhi_surface_info_apple.hpp"
#endif

#ifdef AVIO_ENABLE_GLFW
#include "platforms/rhi_surface_info_glfw.hpp"
#endif