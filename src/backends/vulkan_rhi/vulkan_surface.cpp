#include "vulkan_surface.hpp"
#include "vulkan_rhi.hpp"

#ifdef WIN32
#include <vulkan/vulkan_win32.h>
#endif

#ifdef __linux__
#include <vulkan/vulkan_xlib.h>
#include "X11/Xlib.h"
#endif

#ifdef __APPLE__
#include <vulkan/vulkan_macos.h>
#endif

#ifdef AVIO_ENABLE_GLFW
#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"
#endif

namespace avio::vulkan {

  RhiSurface* vulkan_create_surface(RHI* rhi, const infos::RhiSurfaceInfo& info) {
    RhiVulkan* vulkan = cast_rhi<RhiVulkan>(rhi);
    VulkanSurface* surface = vulkan->surfaces.allocate();

    surface->base.info = info;
    VkSurfaceKHR vk_surface{};

// ------ WINDOWS -----------
#if defined(WIN32)
    VkWin32SurfaceCreateInfoKHR create_info{
        .sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,
        .pNext = nullptr,
        .flags = 0,
        .hinstance = ::GetModuleHandleA(nullptr),
        .hwnd = info.hwnd,
    };

    VK_ASSERT(vkCreateWin32SurfaceKHR(vulkan->instance, &create_info, nullptr, &vk_surface));
// ------ LINUX -----------
#elif defined(__linux__)
#ifdef AVIO_X11_AVAILABLE
    if (info.surface_type == infos::LinuxSurfaceType::x11) {
      VkXlibSurfaceCreateInfoKHR create_info{
          .sType = VK_STRUCTURE_TYPE_XLIB_SURFACE_CREATE_INFO_KHR,
          .pNext = nullptr,
          .flags = 0,
          .dpy = info.x11.x11_display,
          .window = info.x11.x11_window,
      };
      VK_ASSERT(vkCreateXlibSurfaceKHR(vulkan->instance, &create_info, nullptr, &vk_surface));
    }
#endif
// ------ MAC -----------
#elif defined(__APPLE__)
#error "Unsupported if not using GLFW"
#endif
    // ifndef AVIO_USES_GLFW
    if (vk_surface) {
      surface->vulkan_surface = vk_surface;
    }

    return &surface->base;
  }

#ifdef AVIO_ENABLE_GLFW
  RhiSurface* vulkan_create_surface_glfw(RHI* rhi, GLFWwindow* window) {
    RhiVulkan* vulkan = cast_rhi<RhiVulkan>(rhi);
    VulkanSurface* surface = vulkan->surfaces.allocate();

    // surface->base.info = info;
    VkSurfaceKHR vk_surface{};
    VkResult result = glfwCreateWindowSurface(vulkan->instance, window, nullptr, &vk_surface);
    if(result != VK_SUCCESS) {
      const char* error_message;
      glfwGetError(&error_message);
      throw Error("Failed to create vulkan window surface: {}", error_message);
    }

    surface->vulkan_surface = vk_surface;
    return &surface->base;
  }
#endif

  void vulkan_destroy_surface(RHI* rhi, RhiSurface* surface) {
    RhiVulkan* vulkan = cast_rhi<RhiVulkan>(rhi);
    VulkanSurface* vk_surf = (VulkanSurface*)surface;
    if (vulkan && vk_surf) {
      vkDestroySurfaceKHR(vulkan->instance, vk_surf->vulkan_surface, nullptr);
    }

    vulkan->surfaces.deallocate(vk_surf);
  }
}  // namespace avio::vulkan