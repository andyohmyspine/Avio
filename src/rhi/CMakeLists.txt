avio_create_static_module(avio_rhi)
add_library(avio::rhi ALIAS avio_rhi)

target_link_libraries(avio_rhi PUBLIC avio::core)
add_dependencies(avio_rhi copy_slang_headers)

if(WIN32)
  set(AVIO_D3D12_AVAILABLE ON CACHE BOOL "")
else()
  set(AVIO_D3D12_AVAILABLE OFF CACHE BOOL "")
endif()

find_package(Vulkan QUIET)
if(Vulkan_FOUND)
  message(STATUS "Vulkan found.")
  set(AVIO_VULKAN_AVAILABLE ON CACHE BOOL "")
else()
  set(AVIO_VULKAN_AVAILABLE OFF CACHE BOOL "")
endif()

target_link_libraries(avio_rhi PUBLIC avio::slang)


if(AVIO_ENABLE_GPU_VALIDATION)
  target_compile_definitions(avio_rhi PUBLIC AVIO_ENABLE_GPU_VALIDATION)
endif()

if(AVIO_D3D12_AVAILABLE)
  target_link_libraries(avio_rhi PUBLIC avio::d3d12_rhi)
  target_compile_definitions(avio_rhi PUBLIC AVIO_D3D12_AVAILABLE)
endif()

if(AVIO_VULKAN_AVAILABLE)
  target_link_libraries(avio_rhi PUBLIC avio::vulkan_rhi)
  target_compile_definitions(avio_rhi PUBLIC AVIO_VULKAN_AVAILABLE)
endif()

if(AVIO_ENABLE_GLFW)
  target_compile_definitions(avio_rhi PUBLIC AVIO_ENABLE_GLFW)
  target_link_libraries(avio_rhi PUBLIC glfw)
endif()

if(UNIX AND NOT APPLE)
  find_package(X11 REQUIRED)
  if(X11_FOUND)
    message(STATUS "Found X11. Setting as one of the available targets.")
    set(AVIO_X11_AVAILABLE ON CACHE BOOL "")
    else()
    message(STATUS "X11 not found.")
    set(AVIO_X11_AVAILABLE OFF CACHE BOOL "")
    target_link_libraries(avio_rhi PUBLIC ${X11_LIBRARIES})
    target_include_directories(avio_rhi PUBLIC ${X11_INCLUDE_DIR})
  endif()

  find_package(Wayland QUIET)

  # target_include_directories(avio_rhi PUBLIC ${X11_INCLUDE_DIR})
endif()

if(AVIO_X11_AVAILABLE)
  target_compile_definitions(avio_rhi PUBLIC AVIO_X11_AVAILABLE)
endif()