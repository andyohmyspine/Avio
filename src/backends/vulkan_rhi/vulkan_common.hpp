#pragma once

#include "core.hpp"
#include "rhi.hpp"

#include <vulkan/vulkan.hpp>

#define VK_ASSERT(result) \
  AV_ASSERT(((vk::Result)result) == vk::Result::eSuccess)

namespace avio::vulkan {
  struct RhiVulkan;
}