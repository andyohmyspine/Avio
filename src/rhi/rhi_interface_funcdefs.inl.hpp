#pragma once

namespace avio::rhi {
  inline RHI* get() {
    return avio::funcs::get_rhi_();
  }

  inline void begin_frame(RHI* rhi) {
    avio::funcs::rhi_begin_frame_(rhi);
  }

  inline void end_frame(RHI* rhi) {
    avio::funcs::rhi_end_frame_(rhi);
  }

  inline void submit_frame(RHI* rhi) {
    avio::funcs::rhi_submit_frame_(rhi);
  }

  inline RhiSurface* create_surface(RHI* rhi, const avio::infos::RhiSurfaceInfo& info) {
    return avio::funcs::rhi_create_surface_(rhi, info);
  }

#ifdef AVIO_ENABLE_GLFW
  inline RhiSurface* create_surface_glfw(RHI* rhi, GLFWwindow* window) {
    return avio::funcs::rhi_create_surface_glfw_(rhi, window);
  }
#endif

  inline void destroy_surface(RHI* rhi, RhiSurface* surface) {
    avio::funcs::rhi_destroy_surface_(rhi, surface);
  }

  inline RhiSwapchain* create_swapchain(RHI* rhi, const avio::infos::RhiSwapchainInfo& info) {
    return avio::funcs::rhi_create_swapchain_(rhi, info);
  }

  inline void destroy_swapchain(RHI* rhi, RhiSwapchain* swapchain) {
    avio::funcs::rhi_destroy_swapchain_(rhi, swapchain);
  }

  inline void present_swapchain(RHI* rhi, RhiSwapchain* swapchain) {
    avio::funcs::rhi_present_swapchain_(rhi, swapchain);
  }
}  // namespace avio::rhi