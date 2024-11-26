#pragma once

#include "core.hpp"
#include "rhi_render_surface.hpp"
#include "rhi_swapchain.hpp"

namespace avio {

  inline constexpr uint32_t RHI_DEFAULT_SWAPCHAIN_IMAGE_COUNT = 3;

  enum class RenderAPI {
#ifdef AVIO_D3D12_AVAILABLE
    d3d12,
#endif

#ifdef AVIO_VULKAN_AVAILABLE
    vulkan,
#endif
  };

  /**
 * Rhi configuration struct.
 */
  namespace infos {
    struct RHIInfo {
      RenderAPI render_api{};
    };
  }  // namespace infos
  /**
 * Rhi object. That should be initialized.
 */
  struct RHI {
    bool (*init_impl)(RHI* rhi, const infos::RHIInfo& info);
    void (*shutdown_impl)(RHI* rhi);
  };

  bool init_rhi(RHI** out_rhi, const infos::RHIInfo& info);
  void shutdown_rhi(RHI* rhi);

  using PFN_get_rhi = RHI* (*)();
  inline PFN_get_rhi get_rhi;

  template <typename T>
  extern T* get_rhi_as() {
    return (T*)get_rhi();
  }

  template <typename T>
  inline T* cast_rhi(RHI* rhi) {
    return (T*)rhi;
  }

}  // namespace avio