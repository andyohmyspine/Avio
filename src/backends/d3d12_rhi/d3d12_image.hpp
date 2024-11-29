#pragma once

#include "core.hpp"
#include "rhi_types.hpp"
#include "d3d12_common.hpp"
#include "d3d12_descriptors.hpp"

namespace avio::dx12 {
  struct D3D12Image {
    RhiImage base;

    ID3D12Resource* image;
    DXGI_FORMAT native_format;
  };

  struct D3D12ImageView {
    RhiImageView base;

    D3D12_DESCRIPTOR_HEAP_TYPE heap_type;
    D3D12DescriptorHandle descriptor_handle;
  };
}