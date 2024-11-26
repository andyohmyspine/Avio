#pragma once

#include "core.hpp"
#include "rhi.hpp"

namespace avio {

  namespace infos {
    struct EngineInfo {
      RenderAPI render_api{};
    };
  }  // namespace infos

  struct Engine {
    RHI* rhi;
  };

  /**
 * @brief Initialize the engine. This is the main function that should be called.
 * @param out_engine reference to engine object.
 * @param in_engine_info engine configuration structure instance.
 */
  bool init_engine(Engine& out_engine, const infos::EngineInfo& in_engine_info);

  /**
 * @brief Shutdown the engine. This will terminate everything.
 * @param engine The engine to shutdown.
 */
  void shutdown_engine(Engine& engine);

}  // namespace avio