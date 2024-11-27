create_static_module(rhi)
add_library(avio::rhi ALIAS rhi)

target_link_libraries(rhi PUBLIC avio::core)

if(WIN32)
  set(AVIO_D3D12_AVAILABLE ON CACHE BOOL "")
else()
  set(AVIO_D3D12_AVAILABLE OFF CACHE BOOL "")
endif()

find_package(Vulkan QUIET)
if(Vulkan_FOUND)
  set(AVIO_VULKAN_AVAILABLE ON CACHE BOOL "")
else()
  set(AVIO_VULKAN_AVAILABLE OFF CACHE BOOL "")
endif()

if(AVIO_ENABLE_GPU_VALIDATION)
  target_compile_definitions(rhi PUBLIC AVIO_ENABLE_GPU_VALIDATION)
endif()

if(AVIO_D3D12_AVAILABLE)
  target_link_libraries(rhi PUBLIC avio::d3d12_rhi)
  target_compile_definitions(rhi PUBLIC AVIO_D3D12_AVAILABLE)
endif()

if(AVIO_VULKAN_AVAILABLE)
  target_link_libraries(rhi PUBLIC avio::vulkan_rhi)
  target_compile_definitions(rhi PUBLIC AVIO_VULKAN_AVAILABLE)
endif()