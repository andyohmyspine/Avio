#include "d3d12_rhi.hpp"

namespace avio {

RhiD3D12 g_rhi_d3d12;
RHI* get_rhi() {
  return &g_rhi_d3d12.base;
}

}