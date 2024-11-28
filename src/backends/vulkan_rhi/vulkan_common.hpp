#pragma once

#include "core.hpp"
#include "rhi.hpp"

#include <vulkan/vulkan.hpp>

#include <array>

#include "rhi_types.hpp"

#define VK_ASSERT(result) \
  AV_ASSERT(((vk::Result)result) == vk::Result::eSuccess)

namespace avio::vulkan {
  struct RhiVulkan;

  inline vk::Format vulkan_format_mapping[(size_t)PixelFormat::max_formats] {};
  inline void set_pixel_format_mapping(PixelFormat format, vk::Format mapping) {
    vulkan_format_mapping[(size_t)format] = mapping;
  }

  inline void init_vulkan_format_mappings() {
    set_pixel_format_mapping(PixelFormat::none, vk::Format::eUndefined);

    // To be defined when swapchain gets created.
    set_pixel_format_mapping(PixelFormat::window_output, vk::Format::eUndefined);
  }
}