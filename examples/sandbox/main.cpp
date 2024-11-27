#include "engine.hpp"

#ifdef WIN32
#define GLFW_EXPOSE_NATIVE_WIN32
#include "GLFW/glfw3.h"
#include "GLFW/glfw3native.h"
#endif

void sandbox_main(avio::Engine& engine) {
  glfwInit();
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  GLFWwindow* window = glfwCreateWindow(1280, 720, "Sandbox", nullptr, nullptr);

  // Create surface
  avio::infos::RhiSurfaceInfo surface_info{
#ifdef WIN32
      .hwnd = glfwGetWin32Window(window),
#endif
  };

  avio::RhiSurface* surface = avio::rhi_create_surface(engine.rhi, surface_info);
  avio::RhiSwapchain* swapchain = avio::rhi_create_swapchain(engine.rhi, {.surface = surface, .allow_vsync = false});

  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();

    avio::rhi_begin_frame(engine.rhi);
    {
      {
        avio::rhi_cmd_begin_draw_to_swapchain(engine.rhi, swapchain);

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

  avio::RenderAPI render_api = {};
  if (argc > 1) {
#ifdef AVIO_D3D12_AVAILABLE
    if (strcmp(argv[1], "d3d12") == 0) {
      render_api = avio::RenderAPI::d3d12;
    }
#endif

#ifdef AVIO_VULKAN_AVAILABLE
    if(strcmp(argv[1], "vulkan") == 0) {
      render_api = avio::RenderAPI::vulkan;
    }
#endif
  }

  AV_COMMON_CATCH() {
    AV_ASSERT_MSG(avio::init_engine(engine, {.render_api = render_api}), "Failed to initialize engine!");
    sandbox_main(engine);
    avio::shutdown_engine(engine);
  };

  return 0;
}