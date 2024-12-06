#include "rhi_shader_compiler.hpp"

#include <slang-com-ptr.h>
#include <slang-gfx.h>
#include <slang/slang.h>

#include <unordered_map>

namespace avio {
#define SHADER_COMPILER_ASSERT(x) avio::check(SLANG_SUCCEEDED((x)))

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
        target_desc.profile = compiler->global_session->findProfile("sm_6_6");
       log::info("Initializing Slang Shader Compiler to work with SPIR-V (sm_6_6)");
      }
#endif

#ifdef AVIO_D3D12_AVAILABLE
      if (info.render_api == RenderAPI::d3d12) {
        target_desc.format = SLANG_DXIL;
        target_desc.profile = compiler->global_session->findProfile("sm_6_6");
       log::info("Initializing Slang Shader Compiler to work with DXIL (sm_6_6)");
      }
#endif

      avio::check(target_desc.format != SLANG_TARGET_UNKNOWN);
      session_desc.searchPaths = info.shader_search_paths.data();
      session_desc.searchPathCount = (SlangInt)info.shader_search_paths.size();

      session_desc.targetCount = 1;
      session_desc.targets = &target_desc;

      compiler->global_session->createSession(session_desc, &compiler->session);

      // Try to compile default library module
      try {
        rhi_compiler_compile_shader_module(compiler, "avio_shader");
      } catch (const Error& error) {
        log::critical(
               "Failed to load default slang module. Make sure to add avio shaders path to the shader includes.");
        throw;
      }
    }

    void rhi_shutdown_shader_compiler(RhiShaderCompiler* compiler) {
      for (auto& [_, mod] : compiler->shader_modules) {
        mod.shader_module->release();
      }

      compiler->session->release();
      compiler->global_session->release();
    }
  }  // namespace detail
  // namespace detail

  static slang::IComponentType* link_shader_entry_points(RhiShaderCompiler* compiler, slang::IModule* in_module) {
    std::vector<slang::IComponentType*> linked_components{in_module};
    if (in_module) {
      const SlangInt entry_point_count = in_module->getDefinedEntryPointCount();
      for (int32_t index = 0; index < entry_point_count; ++index) {
        slang::IEntryPoint* entry_point = {};
        SHADER_COMPILER_ASSERT(in_module->getDefinedEntryPoint(index, &entry_point));
        linked_components.push_back(entry_point);
      }
    }

    // Linked program
    Slang::ComPtr<slang::IComponentType> program;
    SHADER_COMPILER_ASSERT(compiler->session->createCompositeComponentType(
        linked_components.data(), (SlangInt)linked_components.size(), program.writeRef()));

    Slang::ComPtr<ISlangBlob> diagnostics_blob{};
    slang::IComponentType* linked_program;
    SHADER_COMPILER_ASSERT(program->link(&linked_program, diagnostics_blob.writeRef()));

    if (diagnostics_blob) {
      if (!linked_program) {
        throw Error("Failed to link shader program: {}", (const char*)diagnostics_blob->getBufferPointer());
      } else {
        log::warn("Shader program linking produced following errors and warnings: {}",
               (const char*)diagnostics_blob->getBufferPointer());
      }
    }

    return linked_program;
  }


  struct RhiShaderModuleReflection {
    
  };

  static auto collect_linked_module_reflection(slang::IComponentType* program) {
    Slang::ComPtr<ISlangBlob> diag_blob;
    slang::ProgramLayout* layout = program->getLayout(0, diag_blob.writeRef());

    for (SlangUInt index = 0; index < layout->getEntryPointCount(); ++index) {
      slang::EntryPointReflection* entry_point = layout->getEntryPointByIndex(index);
      const SlangStage stage = entry_point->getStage();
    }
  }

  RhiShaderModule* rhi_compiler_compile_shader_module(RhiShaderCompiler* compiler, const char* module_name) {
    std::string module_name_string = module_name;
    if (!compiler->shader_modules.empty() && compiler->shader_modules.contains(module_name_string)) {
      return &compiler->shader_modules.at(module_name_string);
    }

    Slang::ComPtr<ISlangBlob> diagnostics_blob;
    RhiShaderModule out_module{
        .parent = compiler, .shader_module = compiler->session->loadModule(module_name, diagnostics_blob.writeRef())};

    if (diagnostics_blob) {
      if (!out_module.shader_module) {
        throw Error("Failed to compile shader module '{}': {}", module_name,
                    (const char*)diagnostics_blob->getBufferPointer());
      } else {
        log::warn("'{}' shader compilation produced following errors and warnings: {}", module_name,
               (const char*)diagnostics_blob->getBufferPointer());
      }
    }

    // Link the shader program
    slang::IComponentType* program = link_shader_entry_points(compiler, out_module.shader_module);
    
    // Collect module reflection info
    collect_linked_module_reflection(program);

    compiler->shader_modules[module_name_string] = out_module;
    return &compiler->shader_modules.at(module_name_string);
  }
}  // namespace avio