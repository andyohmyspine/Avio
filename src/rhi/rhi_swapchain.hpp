#pragma once

#include "core.hpp"
#include "rhi_render_surface.hpp"
#include "rhi_types.hpp"

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
    inline void set_is_array() {
      data = std::array<T, RHI_DEFAULT_SWAPCHAIN_IMAGE_COUNT>();
      uses_vector = false;
    }

    inline auto& get_vector() { return std::get<std::vector<T>>(data); }
    inline void set_is_vector(uint32_t immediate_resize = 0) {
      data = std::vector<T>();
      if(immediate_resize > 0) {
        get_vector().resize(immediate_resize);
      }

      uses_vector = true;
    }

    inline T& operator[](size_t index) {
      if (uses_vector) {
        return get_vector()[index];
      }

      return get_array()[index];
    }

    inline const T& operator[](size_t index) const {
      if (uses_vector) {
        return std::get<std::vector<T>>(data)[index];
      }

      return std::get<std::array<T, RHI_DEFAULT_SWAPCHAIN_IMAGE_COUNT>>(data)[index];
    }

    inline auto begin() { return uses_vector ? get_vector().data : get_array().data; }
    inline auto end() {
      return uses_vector ? get_vector().data = get_vector().size() : get_array().data + get_array().size;
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

  RHI_FUNC_PTR(rhi_create_swapchain, RhiSwapchain* (*)(RHI* rhi, const infos::RhiSwapchainInfo& info));
  RHI_FUNC_PTR(rhi_destroy_swapchain, void (*)(RHI* rhi, RhiSwapchain* swapchain));
  RHI_FUNC_PTR(rhi_present_swapchain, void (*)(RHI* rhi, RhiSwapchain* swapchain));
  RHI_FUNC_PTR(rhi_submit_frame, void (*)(RHI* rhi));
}  // namespace avio