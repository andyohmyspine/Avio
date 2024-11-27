#pragma once

#include "core.hpp"
#include "rhi_render_surface.hpp"

#include <variant>

namespace avio {
  struct RHI;

  inline constexpr uint32_t RHI_DEFAULT_SWAPCHAIN_IMAGE_COUNT = 3;

  template <class T>
  struct PerImageArray {
    using VariantType = std::variant<std::monostate, std::array<T, RHI_DEFAULT_SWAPCHAIN_IMAGE_COUNT>, std::vector<T>>;
    VariantType data;

    bool uses_vector = false;

    void assign(VariantType&& variant) {
      data = std::move(variant);
      uses_vector = std::holds_alternative<std::vector<T>>(data);
    }

    inline auto& get_array() { return std::get<std::array<T, RHI_DEFAULT_SWAPCHAIN_IMAGE_COUNT>>(data); }
    inline void set_is_array() { data = std::array<T, RHI_DEFAULT_SWAPCHAIN_IMAGE_COUNT>(); }

    inline auto& get_vector() { return std::get<std::vector<T>>(data); }
    inline void set_is_vector() { data = std::vector<T>(); }

    inline T& operator[](size_t index) {
      if (uses_vector) {
        return std::get<std::vector<T>>(data)[index];
      }

      return std::get<std::array<T, RHI_DEFAULT_SWAPCHAIN_IMAGE_COUNT>>(data)[index];
    }

    inline const T& operator[](size_t index) const {
      if (uses_vector) {
        return std::get<std::vector<T>>(data)[index];
      }

      return std::get<std::array<T, RHI_DEFAULT_SWAPCHAIN_IMAGE_COUNT>>(data)[index];
    }
  };

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

  using PFN_rhi_create_swapchain = RhiSwapchain* (*)(RHI* rhi, const infos::RhiSwapchainInfo& info);
  inline PFN_rhi_create_swapchain rhi_create_swapchain;

  using PFN_rhi_destroy_swapchain = void (*)(RHI* rhi, RhiSwapchain* swapchain);
  inline PFN_rhi_destroy_swapchain rhi_destroy_swapchain;

  using PFN_rhi_present_swapchain = void (*)(RHI* rhi, RhiSwapchain* swapchain);
  inline PFN_rhi_present_swapchain rhi_present_swapchain;

  using PFN_rhi_submit_frame = void (*)(RHI* rhi);
  inline PFN_rhi_submit_frame rhi_submit_frame;
}  // namespace avio