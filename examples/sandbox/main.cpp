#include "engine.hpp"

#include "GLFW/glfw3.h"

#include <filesystem>

void sandbox_main(avio::Engine& engine) {
  using namespace avio;

  /**
   * Create a window
   */
  glfwInit();
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  GLFWwindow* window = glfwCreateWindow(1280, 720, "Sandbox", nullptr, nullptr);

  /**
  * using GLFW helpers module to create window surface.
  */
  // Create surface
  RhiSurface* surface = rhi::create_surface_glfw(engine.rhi, window);
  RhiSwapchain* swapchain = rhi::create_swapchain(engine.rhi, {.surface = surface, .allow_vsync = false});

  /**
   * Compile a shader module like this.
   */
  RhiShaderModule* test_shader = rhi::compile_shader_module(engine.rhi, "hello-world");

  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();

    // rhi::begin_frame must be called at the beginning of every frame.
    rhi::begin_frame(engine.rhi);
    {
      // This function begins drawing to a window swapchain
      rhi::cmd::begin_draw_to_swapchain(engine.rhi, swapchain, true, avio::colors::magenta);

      // This function finishes drawing to a window swapchain
      rhi::cmd::end_draw_to_swapchain(engine.rhi, swapchain);

      // This call submits all recorded frame commands
      rhi::submit_frame(engine.rhi);

      // This call presents the swapchain
      rhi::present_swapchain(engine.rhi, swapchain);
    }

    // rhi::end_frame should be called at the end of the frame
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
  const char* shader_search_paths[] = {
    "examples/sandbox/shaders/",  // Path to example shaders
    "shaders/"                    // Path to avio shaders
    };

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