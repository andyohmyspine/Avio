#pragma once

#include "core.hpp"

#ifndef RHI_FUNC_PTR
#define RHI_FUNC_PTR(name, signature)      \
  using AV_PASTE2(PFN_, name) = signature; \
  inline AV_PASTE2(PFN_, name) name##_;

#endif

namespace avio {

  enum class RenderAPI {
#ifdef AVIO_D3D12_AVAILABLE
    d3d12,
#endif

#ifdef AVIO_VULKAN_AVAILABLE
    vulkan,
#endif
  };

  enum class PixelFormat {
    none,
    window_output,
    max_formats,
  };

  enum class ImageType {
    image_2d,
    image_3d,
    max_image_type,
  };

  struct RhiImage {
    ImageType type;
    uint32_t width;
    uint32_t height;
    PixelFormat format;
  };

  enum class ImageViewType {
    color,
    depth_stencil,
    max_view_type,
  };

  struct RhiImageView {
    RhiImage* image;
    ImageViewType type;
  };
}  // namespace avio