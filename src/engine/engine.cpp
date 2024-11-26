#include "engine.hpp"

namespace avio {

  bool init_engine(Engine& out_engine,
                   const infos::EngineInfo& in_engine_info) {
    memset(&out_engine, 0, sizeof(out_engine));

    // Initialize RHI
    AV_ASSERT_MSG(init_rhi(&out_engine.rhi,
                           {
                               .render_api = in_engine_info.render_api,
                           }),
                  "Failed to initialize RHI.");

    return true;
  }

  void shutdown_engine(Engine& engine) {
    shutdown_rhi(engine.rhi);
  }

}  // namespace avio