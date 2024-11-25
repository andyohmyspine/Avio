#include "rhi.hpp"

namespace avio {

bool init_rhi(RHI** out_rhi, const infos::RHIInfo& info) {
  *out_rhi = get_rhi();
  return true;
}

void shutdown_rhi(RHI* rhi) {}

}  // namespace avio
