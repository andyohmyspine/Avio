#pragma once

#include "core.hpp"
#include "rhi_render_commands.hpp"
#include "rhi_render_surface.hpp"
#include "rhi_swapchain.hpp"
#include "rhi_types.hpp"

#include <array>
#include <span>

namespace avio {

  inline constexpr uint32_t RHI_NUM_FRAMES_IN_FLIGHT = 2;

  template <typename T>
  using InFlightArray = std::array<T, RHI_NUM_FRAMES_IN_FLIGHT>;

  /**
 * Rhi configuration struct.
 */
  namespace infos {
    struct RHIInfo {
      std::span<const char* const> shader_search_paths;
      RenderAPI render_api{};
    };
  }  // namespace infos
  /**
 * Rhi object. That should be initialized.
 */

  struct RhiShaderCompiler;

  struct RHI {
    RhiShaderCompiler* shader_compiler;

    uint32_t current_frame_in_flight = 0;
    bool (*init_impl)(RHI* rhi, const infos::RHIInfo& info);
    void (*shutdown_impl)(RHI* rhi);

    bool has_began_frame : 1 = false;
  };

  bool init_rhi(RHI** out_rhi, const infos::RHIInfo& info);
  void shutdown_rhi(RHI* rhi);

  // Shader API
  struct RhiShaderModule;

  namespace rhi {
    RhiShaderModule* compile_shader_module(RHI* rhi, const char* module_name);
  }

  namespace rhi {
    RHI* get();
  }

  template <typename T>
  extern T* get_rhi_as() {
    return (T*)rhi::get();
  }

  template <typename T, typename U>
  inline T* cast_rhi(U* rhi) {
    return (T*)rhi;
  }

  // Function pointers

}  // namespace avio