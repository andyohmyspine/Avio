
#ifdef WIN32
#define GLFW_EXPOSE_NATIVE_WIN32
#endif

#ifdef AVIO_X11_AVAILABLE
#define GLFW_EXPOSE_NATIVE_X11
#endif

#include "glfw_helpers.hpp"
#include "GLFW/glfw3native.h"


namespace avio::glfw {
  infos::RhiSurfaceInfo create_surface_info(GLFWwindow* window) {
    infos::RhiSurfaceInfo surface_info{
#ifdef WIN32
        .hwnd = glfwGetWin32Window(window),
#endif

#ifdef __linux__
        .surface_type = infos::LinuxSurfaceType::x11,
#ifdef AVIO_X11_AVAILABLE
        .x11 = {.x11_display = glfwGetX11Display(), .x11_window = glfwGetX11Window(window)},
#endif
#endif
    };

    return surface_info;
  }
}  // namespace avio::glfw