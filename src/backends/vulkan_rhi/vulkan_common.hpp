#pragma once

#include "core.hpp"
#include "rhi.hpp"
#include <vulkan/vulkan.hpp>

#define VK_ASSERT(result) AV_ASSERT(((vk::Result)result) == vk::Result::eSuccess)

namespace avio {
  struct RhiVulkan;

  inline RhiVulkan* get_vulkan_rhi() { return get_rhi_as<RhiVulkan>(); }
  inline RhiVulkan* cast_rhi(RHI* rhi) { return (RhiVulkan*)rhi; }
}