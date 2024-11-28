#pragma once

#include "core.hpp"
#include "rhi_types.hpp"
#include "d3d12_common.hpp"

namespace avio::dx12 {
  struct D3D12Image {
    RhiImage base;

    ID3D12Resource* image;
    DXGI_FORMAT native_format;
  };
}