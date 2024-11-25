#pragma once

#include <directx/d3d12.h>
#include <directx/d3dx12.h>
#include <dxgi1_6.h>

#include "rhi.hpp"

namespace avio {
  struct RhiD3D12;

  inline RhiD3D12* get_d3d12_rhi() { return get_rhi_as<RhiD3D12>(); }
  inline RhiD3D12* cast_rhi(RHI* rhi) { return (RhiD3D12*)rhi; } 
}

#define HR_ASSERT(hr) AV_ASSERT(SUCCEEDED(hr))