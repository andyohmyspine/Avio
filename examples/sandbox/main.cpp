#include "engine.hpp"

#ifdef WIN32
#define GLFW_EXPOSE_NATIVE_WIN32
#include "GLFW/glfw3.h"
#include "GLFW/glfw3native.h"
#endif

#include <filesystem>

void sandbox_main(avio::Engine& engine) {
  glfwInit();
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  // glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
  GLFWwindow* window = glfwCreateWindow(1280, 720, "Sandbox", nullptr, nullptr);

  // Create surface
  avio::infos::RhiSurfaceInfo surface_info{
#ifdef WIN32
      .hwnd = glfwGetWin32Window(window),
#endif
  };

  avio::RhiSurface* surface = avio::rhi_create_surface(engine.rhi, surface_info);
  avio::RhiSwapchain* swapchain = avio::rhi_create_swapchain(engine.rhi, {.surface = surface, .allow_vsync = false});

  avio::RhiShaderModule* test_shader = avio::rhi_compile_shader_module(engine.rhi, "hello-world");

  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();

    avio::rhi_begin_frame(engine.rhi);
    {
      {
        avio::rhi_cmd_begin_draw_to_swapchain(engine.rhi, swapchain, true, avio::colors::black);
        
        avio::rhi_cmd_end_draw_to_swapchain(engine.rhi, swapchain);
      }

      avio::rhi_submit_frame(engine.rhi);
      avio::rhi_present_swapchain(engine.rhi, swapchain);
    }
    avio::rhi_end_frame(engine.rhi);
  }

  avio::rhi_destroy_swapchain(engine.rhi, swapchain);
  avio::rhi_destroy_surface(engine.rhi, surface);

  glfwTerminate();
}

int main(int argc, char** argv) {
  avio::Engine engine;

  // These should be relative to root directory
  const char* shader_search_paths[] = {
    "examples/sandbox/assets/shaders/"
  };

  AV_COMMON_CATCH() {
    avio::init_engine(engine, {
                                  .args = avio::make_launch_args(argc, argv),
                                  .shader_search_paths = shader_search_paths,
                              });
    sandbox_main(engine);
    avio::shutdown_engine(engine);
  };

  return 0;
}