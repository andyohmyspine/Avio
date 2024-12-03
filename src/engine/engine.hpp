#pragma once

#include "core.hpp"
#include "rhi.hpp"

#include <span>
#include <optional>

namespace avio {

  namespace infos {
    struct EngineInfo {
      std::span<const char* const> args;
      std::span<const char* const> shader_search_paths;
      std::optional<RenderAPI> override_render_api;
    };
  }  // namespace infos

  struct Engine {
    RHI* rhi;
    std::span<const char* const> args;
  };

  inline std::span<const char* const> make_launch_args(int argc, const char* const* const argv) {
    return std::span<const char* const>(argv, argc);
  }

  inline bool has_compile_arg(std::span<const char* const> args, const char* requested_arg) {
    for(const char* const arg : args) {
      if(strcmp(arg, requested_arg) == 0) 
        return true;
    }

    return false;
  }

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