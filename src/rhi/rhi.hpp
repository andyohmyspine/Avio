#pragma once

#include "core.hpp"
#include "rhi_render_commands.hpp"
#include "rhi_render_surface.hpp"
#include "rhi_swapchain.hpp"
#include "rhi_types.hpp"

#include <array>

namespace avio {

  inline constexpr uint32_t RHI_NUM_FRAMES_IN_FLIGHT = 2;

  template <typename T>
  using InFlightArray = std::array<T, RHI_NUM_FRAMES_IN_FLIGHT>;

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
    uint32_t current_frame_in_flight = 0;
    bool (*init_impl)(RHI* rhi, const infos::RHIInfo& info);
    void (*shutdown_impl)(RHI* rhi);

    bool has_began_frame : 1 = false;
  };

  bool init_rhi(RHI** out_rhi, const infos::RHIInfo& info);
  void shutdown_rhi(RHI* rhi);

  RHI_FUNC_PTR(get_rhi, RHI* (*)());
  RHI_FUNC_PTR(rhi_begin_frame, void (*)(RHI* rhi));
  RHI_FUNC_PTR(rhi_end_frame, void (*)(RHI* rhi));
  RHI_FUNC_PTR(rhi_submit_frame, void (*)(RHI* rhi));

  template <typename T>
  extern T* get_rhi_as() {
    return (T*)get_rhi();
  }

  template <typename T, typename U>
  inline T* cast_rhi(U* rhi) {
    return (T*)rhi;
  }

}  // namespace avio