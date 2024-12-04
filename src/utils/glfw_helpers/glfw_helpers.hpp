#pragma once

#include "rhi_render_surface.hpp"
#include <GLFW/glfw3.h>

namespace avio::glfw {
  infos::RhiSurfaceInfo create_surface_info(GLFWwindow* window);
  RhiSurface* create_window_surface(RHI* rhi, GLFWwindow* window);
}