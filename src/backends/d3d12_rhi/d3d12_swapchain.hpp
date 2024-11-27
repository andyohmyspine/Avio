#pragma once

#include "rhi_swapchain.hpp"
#include "d3d12_common.hpp"

namespace avio::dx12 {
  struct D3D12Swapchain {
    RhiSwapchain base;

    IDXGISwapChain4* swapchain;
    uint32_t current_back_buffer_index{};
  };

  RhiSwapchain* d3d12_create_swapchain(RHI* rhi, const infos::RhiSwapchainInfo& info);
  void d3d12_destroy_swapchain(RHI* rhi, RhiSwapchain* swapchain);
  void d3d12_present_swapchain(RHI* rhi, RhiSwapchain* swapchain);
}