#include "rhi.hpp"

namespace avio {

bool init_rhi(RHI** out_rhi, const infos::RHIInfo& info) {
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
