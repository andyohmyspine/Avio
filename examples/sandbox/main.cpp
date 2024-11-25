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
  avio::infos::RhiSurfaceInfo surface_info {
#ifdef WIN32
  .hwnd = glfwGetWin32Window(window),
#endif
  };

  avio::RhiSurface* surface = avio::rhi_create_surface(engine.rhi, surface_info);

  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();
  }

  avio::rhi_destroy_surface(engine.rhi, surface);

  glfwTerminate();
}

int main() {
  avio::Engine engine;

  try {
    AV_ASSERT_MSG(avio::init_engine(engine,
                                    {
                                        .render_api = avio::RenderAPI::vulkan,
                                    }),
                  "Failed to initialize engine!");
    sandbox_main(engine);

  } catch (const avio::Error& error) {
    AV_LOG(critical, "Exception caught: {}", error.what());
    return 1;
  } catch (const std::runtime_error& error) {
    AV_LOG(critical, "Exception caught: {}", error.what());
    return 1;
  }

  avio::shutdown_engine(engine);
  return 0;
}