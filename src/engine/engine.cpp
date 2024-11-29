#include "engine.hpp"

namespace avio {
  static RenderAPI determine_render_api(const infos::EngineInfo& info) {
    if(info.override_render_api.has_value()) {
      return info.override_render_api.value_or(RenderAPI());
    }

    if (!info.args.empty()) {
#ifdef AVIO_D3D12_AVAILABLE
      if (has_compile_arg(info.args, "d3d12")) {
        return RenderAPI::d3d12;
      }
#endif

#ifdef AVIO_VULKAN_AVAILABLE
      if (has_compile_arg(info.args, "vulkan")) {
        return RenderAPI::vulkan;
      }
#endif
    }

    return RenderAPI();
  }

  bool init_engine(Engine& out_engine, const infos::EngineInfo& in_engine_info) {
    memset(&out_engine, 0, sizeof(out_engine));

    // Initialize RHI
    AV_ASSERT_MSG(init_rhi(&out_engine.rhi,
                           {
                               .render_api = determine_render_api(in_engine_info),
                           }),
                  "Failed to initialize RHI.");

    return true;
  }

  void shutdown_engine(Engine& engine) {
    shutdown_rhi(engine.rhi);
  }

}  // namespace avio