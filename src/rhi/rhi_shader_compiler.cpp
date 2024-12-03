#include "rhi_shader_compiler.hpp"

#include <slang-com-ptr.h>
#include <slang-gfx.h>
#include <slang/slang.h>

#include <unordered_map>

namespace avio {
  struct RhiShaderModule {
    RhiShaderCompiler* parent;
    slang::IModule* shader_module;
  };

  struct RhiShaderCompiler {
    std::vector<const char*> shader_search_paths;

    slang::IGlobalSession* global_session;
    slang::ISession* session;

    std::unordered_map<string, RhiShaderModule> shader_modules;
  };

  static RhiShaderCompiler g_shader_compiler;

  namespace detail {
    RhiShaderCompiler* rhi_get_shader_compiler() {
      return &g_shader_compiler;
    }

    void rhi_init_shader_compiler(RhiShaderCompiler* compiler, const infos::RhiShaderCompilerInfo& info) {
      compiler->shader_search_paths = {info.shader_search_paths.begin(), info.shader_search_paths.end()};
      compiler->shader_modules = {};

      SlangResult result = slang::createGlobalSession(&compiler->global_session);
      if (SLANG_FAILED(result)) {
        throw Error("Failed to create slang global session.");
      }

      slang::SessionDesc session_desc{};
      slang::TargetDesc target_desc{};

#ifdef AVIO_VULKAN_AVAILABLE
      if (info.render_api == RenderAPI::vulkan) {
        target_desc.format = SLANG_SPIRV;
        target_desc.profile = compiler->global_session->findProfile("glsl_450");
        AV_LOG(info, "Initializing Slang Shader Compiler to work with SPIR-V");
      }
#endif

#ifdef AVIO_D3D12_AVAILABLE
      if (info.render_api == RenderAPI::d3d12) {
        target_desc.format = SLANG_DXIL;
        target_desc.profile = compiler->global_session->findProfile("sm_6_2");
        AV_LOG(info, "Initializing Slang Shader Compiler to work with DXIL");
      }
#endif

      AV_ASSERT(target_desc.format != SLANG_TARGET_UNKNOWN);
      session_desc.searchPaths = info.shader_search_paths.data();
      session_desc.searchPathCount = (SlangInt)info.shader_search_paths.size();

      session_desc.targetCount = 1;
      session_desc.targets = &target_desc;

      compiler->global_session->createSession(session_desc, &compiler->session);
    }

    void rhi_shutdown_shader_compiler(RhiShaderCompiler* compiler) {
      for(auto& [_, mod] : compiler->shader_modules) {
        mod.shader_module->release();
      }

      compiler->session->release();
      compiler->global_session->release();
    }
  }  // namespace detail
  // namespace detail

  RhiShaderModule* rhi_compiler_compile_shader_module(RhiShaderCompiler* compiler, const char* module_name) {
    std::string module_name_string = module_name;
    if (!compiler->shader_modules.empty() && compiler->shader_modules.contains(module_name_string)) {
      return &compiler->shader_modules.at(module_name_string);
    }

    Slang::ComPtr<ISlangBlob> diagnostics_blob; 
    RhiShaderModule out_module{
      .parent = compiler,
      .shader_module = compiler->session->loadModule(module_name, diagnostics_blob.writeRef())
    };

    if(diagnostics_blob) {
      if(!out_module.shader_module) {
        throw Error("Failed to compile shader module '{}': {}", module_name, (const char*)diagnostics_blob->getBufferPointer());
      } else {
        AV_LOG(warn, "'{}' shader compilation produced following errors and warnings: {}", module_name, (const char*)diagnostics_blob->getBufferPointer());
      }
    }

    compiler->shader_modules[module_name_string] = out_module;
    return &compiler->shader_modules.at(module_name_string);
  }
}  // namespace avio