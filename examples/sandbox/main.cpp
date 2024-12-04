#include "engine.hpp"

#ifdef WIN32
#define GLFW_EXPOSE_NATIVE_WIN32
#endif

#ifdef AVIO_X11_AVAILABLE
#define GLFW_EXPOSE_NATIVE_X11
#endif

#include "GLFW/glfw3.h"
#include "GLFW/glfw3native.h"

#include <filesystem>

void sandbox_main(avio::Engine& engine) {
  using namespace avio;

  glfwInit();
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  // glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
  GLFWwindow* window = glfwCreateWindow(1280, 720, "Sandbox", nullptr, nullptr);

  // Create surface
  infos::RhiSurfaceInfo surface_info{
#ifdef WIN32
      .hwnd = glfwGetWin32Window(window),
#endif

#ifdef __linux__
      .surface_type = infos::LinuxSurfaceType::x11,
      #ifdef AVIO_X11_AVAILABLE
      .x11 = {
        .x11_display = glfwGetX11Display(),
        .x11_window = glfwGetX11Window(window)
      },
      #endif
#endif
  };

  RhiSurface* surface = rhi::create_surface(engine.rhi, surface_info);
  RhiSwapchain* swapchain = rhi::create_swapchain(engine.rhi, {.surface = surface, .allow_vsync = false});

  RhiShaderModule* test_shader = rhi::compile_shader_module(engine.rhi, "hello-world");

  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();

    rhi::begin_frame(engine.rhi);
    {
      {
        rhi::cmd::begin_draw_to_swapchain(engine.rhi, swapchain, true, avio::colors::magenta);

        rhi::cmd::end_draw_to_swapchain(engine.rhi, swapchain);
      }

      rhi::submit_frame(engine.rhi);
      rhi::present_swapchain(engine.rhi, swapchain);
    }
    rhi::end_frame(engine.rhi);
  }

  rhi::destroy_swapchain(engine.rhi, swapchain);
  rhi::destroy_surface(engine.rhi, surface);

  glfwTerminate();
}

int main(int argc, char** argv) {
  using namespace avio;

  Engine engine;

  // These should be relative to root directory
  const char* shader_search_paths[] = {"examples/sandbox/shaders/", "shaders/"};

  AV_COMMON_CATCH()[&] {
    init_engine(engine, {
                            .args = make_launch_args(argc, argv),
                            .shader_search_paths = shader_search_paths,
                        });
    sandbox_main(engine);
    shutdown_engine(engine);
  };

  return 0;
}