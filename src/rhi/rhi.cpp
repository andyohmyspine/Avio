#include "rhi.hpp"

#ifdef AVIO_D3D12_AVAILABLE
#include "d3d12_rhi.hpp"
#endif

#ifdef AVIO_VULKAN_AVAILABLE
#include "vulkan_rhi.hpp"
#endif

namespace avio {

bool init_rhi(RHI** out_rhi, const infos::RHIInfo& info) {
  // First we need to initialize proper function pointers
  switch (info.render_api) {
#ifdef AVIO_D3D12_AVAILABLE
    case RenderAPI::d3d12: {
      dx12::init_global_rhi_pointers();
    } break;
#endif

#ifdef AVIO_VULKAN_AVAILABLE
    case RenderAPI::vulkan: {
      vulkan::init_global_rhi_pointers();
    } break;
#endif
  }

  *out_rhi = get_rhi();

  RHI* rhi = *out_rhi;
  AV_ASSERT(rhi->init_impl && rhi->shutdown_impl);

  AV_ASSERT_MSG(rhi->init_impl(rhi, info),
                "Failed to initialize RHI native component.");

  return true;
}

void shutdown_rhi(RHI* rhi) {
  rhi->shutdown_impl(rhi);
}

}  // namespace avio
