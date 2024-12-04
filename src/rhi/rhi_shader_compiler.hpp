#pragma once

#include "core.hpp"
#include "rhi_types.hpp"
#include <span>

namespace avio {
    // Here we unfortunatelly must hide the struct contents, because they may be platform dependent.
    struct RhiShaderCompiler;
    struct RhiShaderModule;

    RhiShaderModule* rhi_compiler_compile_shader_module(RhiShaderCompiler* compiler, const char* module_name);
    
    namespace detail {
        namespace infos {
            struct RhiShaderCompilerInfo {
                RenderAPI render_api{};
                std::span<const char* const> shader_search_paths;
            };
        }

        RhiShaderCompiler* rhi_get_shader_compiler();

        void rhi_init_shader_compiler(RhiShaderCompiler* compiler, const infos::RhiShaderCompilerInfo& info);
        void rhi_shutdown_shader_compiler(RhiShaderCompiler* compiler);
    }
}