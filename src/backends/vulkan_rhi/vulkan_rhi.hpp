#pragma once

#include "vulkan_common.hpp"

#include "rhi.hpp"

#include <set>

namespace avio {

struct VulkanQueueFamilyIndices {
  uint32_t graphics = UINT32_MAX;

  inline bool is_valid() const { return graphics != UINT32_MAX; }

  inline std::set<uint32_t> as_unique_set() const noexcept { return {graphics}; }
};

struct VulkanPhysicalDevice {
  vk::PhysicalDevice device;
  VulkanQueueFamilyIndices queue_indices;
};

struct RhiVulkan {
  RHI base;

  vk::Instance instance;

#ifdef AVIO_ENABLE_GPU_VALIDATION
  vk::DebugUtilsMessengerEXT debug_messenger;
#endif

  VulkanPhysicalDevice physical_device;
  vk::Device device;
  vk::Queue graphics_queue;
};

extern RhiVulkan g_rhi_vulkan;

}  // namespace avio