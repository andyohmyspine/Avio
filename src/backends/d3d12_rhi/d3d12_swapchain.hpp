#pragma once

#include "rhi_swapchain.hpp"
#include "d3d12_common.hpp"
#include "d3d12_image.hpp"

namespace avio::dx12 {
  struct D3D12Swapchain {
    RhiSwapchain base;

    IDXGISwapChain4* swapchain;
    uint32_t current_back_buffer_index{};
    uint32_t image_count{};

    PerImageArray<D3D12Image> swapchain_images{};
    PerImageArray<D3D12ImageView> swapchain_image_views{};

    uint32_t width = {};
    uint32_t height = {};
  };

  RhiSwapchain* d3d12_create_swapchain(RHI* rhi, const infos::RhiSwapchainInfo& info);
  void d3d12_destroy_swapchain(RHI* rhi, RhiSwapchain* swapchain);
  void d3d12_present_swapchain(RHI* rhi, RhiSwapchain* swapchain);

  inline D3D12Image* d3d12_current_swapchain_image(D3D12Swapchain* swapchain) {
    return &swapchain->swapchain_images[swapchain->current_back_buffer_index];
  }

  inline D3D12ImageView* d3d12_current_swapchain_rtv(D3D12Swapchain* swapchain) {
    return &swapchain->swapchain_image_views[swapchain->current_back_buffer_index];
  }
}