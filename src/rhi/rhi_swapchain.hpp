#pragma once

#include "core.hpp"
#include "rhi_render_surface.hpp"

namespace avio {
  struct RHI;

  namespace infos {

    struct RhiSwapchainInfo {
      RhiSurface* surface;

      /**
       * @brief Set this to a value you need. If set to 0 will use default (RHI_DEFAULT_SWAPCHAIN_IMAGE_COUNT)
       */
      uint16_t override_image_count = 0;
      bool allow_vsync : 1 = false;
    };

  }  // namespace infos

  struct RhiSwapchain {
    infos::RhiSwapchainInfo info;
  };

  using PFN_rhi_create_swapchain = RhiSwapchain*(*)(RHI* rhi, const infos::RhiSwapchainInfo& info);
  inline PFN_rhi_create_swapchain rhi_create_swapchain;

  using PFN_rhi_destroy_swapchain = void(*)(RHI* rhi, RhiSwapchain* swapchain);
  inline PFN_rhi_destroy_swapchain rhi_destroy_swapchain;

  using PFN_rhi_present_swapchain = void(*)(RHI* rhi, RhiSwapchain* swapchain);
  inline PFN_rhi_present_swapchain rhi_present_swapchain;

  using PFN_rhi_submit_frame = void(*)(RHI* rhi);
  inline PFN_rhi_submit_frame rhi_submit_frame;
}  // namespace avio