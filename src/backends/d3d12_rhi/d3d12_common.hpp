#pragma once

#include <directx/d3d12.h>
#include <directx/d3dx12.h>
#include <dxgi1_6.h>

#include "rhi.hpp"

namespace avio {
  struct RhiD3D12;
}

#define HR_ASSERT(hr) AV_ASSERT(SUCCEEDED(hr))