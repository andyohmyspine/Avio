#pragma once

#include "d3d12_common.hpp"
#include "rhi.hpp"

namespace avio {

struct RhiD3D12 {
  RHI base;

  IDXGIFactory6* dxgi_factory;
  IDXGIAdapter4* adapter;
  ID3D12Device5* device;

  ID3D12CommandQueue* graphics_queue;
};

extern RhiD3D12 g_rhi_d3d12;

inline auto get_d3d12_device() { return get_d3d12_rhi()->device; }
inline auto get_dxgi_factory() { return get_d3d12_rhi()->dxgi_factory; }
inline auto get_dxgi_adapter() { return get_d3d12_rhi()->adapter; }

}  // namespace avio