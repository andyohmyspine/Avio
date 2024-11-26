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

  ArrayPool<RhiSurface, 16> surfaces;
};

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

namespace d3d12_rhi {
  void init_global_rhi_pointers();
}

}  // namespace avio