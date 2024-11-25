#pragma once

#include "core.hpp"

namespace avio {

struct RHIInfo {};

bool init_rhi(const RHIInfo& info);
void shutdown_rhi();

}  // namespace avio