#pragma once

#include "core.hpp"

#ifndef RHI_FUNC_PTR
#define RHI_FUNC_PTR(name, signature)      \
  using AV_PASTE2(PFN_, name) = signature; \
  inline AV_PASTE2(PFN_, name) name;
#endif

namespace avio {
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
}