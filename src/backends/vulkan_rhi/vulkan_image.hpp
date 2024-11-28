#pragma once

#include "core.hpp"
#include "rhi_types.hpp"

#include "vulkan_common.hpp"

namespace avio::vulkan {
  struct VulkanImage {
    RhiImage base;

    vk::Image image;
    vk::Format native_format;
  };

  struct VulkanImageView {
    RhiImageView base;

    vk::ImageView view;
  };
}