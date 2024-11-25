#pragma once

#include "core.hpp"

namespace avio {

/**
 * Rhi configuration struct.
 */
namespace infos {
struct RHIInfo {};
}
/**
 * Rhi object. That should be initialized.
 */
struct RHI {};

bool init_rhi(RHI** out_rhi, const infos::RHIInfo& info);
void shutdown_rhi(RHI* rhi);

extern RHI* get_rhi();

template<typename T>
extern T* get_rhi_as() {
  return (T*)get_rhi();
}

}  // namespace avio