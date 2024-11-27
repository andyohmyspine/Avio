#pragma once

#include "d3d12_common.hpp"
#include "d3d12_swapchain.hpp"
#include "rhi.hpp"

namespace avio::dx12 {

  struct RhiD3D12 {
    RHI base;

    IDXGIFactory6* dxgi_factory;
    IDXGIAdapter4* adapter;
    ID3D12Device5* device;

#ifdef AVIO_ENABLE_GPU_VALIDATION
    ID3D12InfoQueue1* info_queue;
#endif

    ID3D12CommandQueue* graphics_queue;

    ArrayPool<RhiSurface, 16> surfaces{};
    ArrayPool<D3D12Swapchain, 16> swapchains{};

    // For now we will use the single submit per frame approach, though this isn't the best idea in a long run
    // So this solution is ok for now
    ID3D12Fence* fence;
    InFlightArray<uint64_t> fence_frame_values {};
    uint64_t fence_value{};

    InFlightArray<ID3D12CommandAllocator*> command_allocators;
    InFlightArray<ID3D12GraphicsCommandList4*> command_lists;
  };

  void d3d12_flush_command_queue(RhiD3D12* rhi);

  extern RhiD3D12 g_rhi_d3d12;

  inline auto get_d3d12_device() {
    return g_rhi_d3d12.device;
  }
  inline auto get_dxgi_factory() {
    return g_rhi_d3d12.dxgi_factory;
  }
  inline auto get_dxgi_adapter() {
    return g_rhi_d3d12.adapter;
  }
  void init_global_rhi_pointers();

}  // namespace avio::dx12