#include "rhi.hpp"

#ifdef AVIO_D3D12_AVAILABLE
#include "d3d12_rhi.hpp"
#endif

#ifdef AVIO_VULKAN_AVAILABLE
#include "vulkan_rhi.hpp"
#endif

#include "rhi_shader_compiler.hpp"

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

    (*out_rhi)->shader_compiler = detail::rhi_get_shader_compiler();

    detail::infos::RhiShaderCompilerInfo compiler_info {
      .render_api = info.render_api,
      .shader_search_paths = info.shader_search_paths,
    };

    detail::rhi_init_shader_compiler((*out_rhi)->shader_compiler, compiler_info);
    return true;
  }

  void shutdown_rhi(RHI* rhi) {
    detail::rhi_shutdown_shader_compiler(rhi->shader_compiler);
    rhi->shutdown_impl(rhi);
  }

  RhiShaderModule* rhi_compile_shader_module(RHI* rhi, const char* module_name) {
    return rhi_compiler_compile_shader_module(rhi->shader_compiler, module_name);
  }

}  // namespace avio
