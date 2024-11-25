#pragma once

#include "core.hpp"

namespace avio {

struct EngineInfo {};

struct Engine {};

bool init_engine(Engine& out_engine, const EngineInfo& in_engine_info);
void shutdown_engine(Engine& engine);

}  // namespace avio