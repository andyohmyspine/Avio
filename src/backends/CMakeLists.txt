find_package(Vulkan QUIET)

if(WIN32)
  set(AVIO_D3D12_AVAILABLE ON CACHE BOOL "")
else()
  set(AVIO_D3D12_AVAILABLE OFF CACHE BOOL "")
endif()

if(Vulkan_FOUND)
  set(AVIO_VULKAN_AVAILABLE ON CACHE BOOL "")
else()
  set(AVIO_VULKAN_AVAILABLE OFF CACHE BOOL "")
endif()

option(AVIO_COMPILE_RHI "Which RHI to compile. Variants are: d3d12, vulkan" vulkan)

if(AVIO_D3D12_AVAILABLE)
  if(AVIO_COMPILE_RHI STREQUAL d3d12)
    add_subdirectory(d3d12_rhi)
    set(AVIO_USING_D3D12 ON CACHE BOOL "")
  endif()
endif()

if(AVIO_VULKAN_AVAILABLE)
  if(AVIO_COMPILE_RHI STREQUAL vulkan)
    add_subdirectory(vulkan_rhi)
    set(AVIO_USING_VULKAN ON CACHE BOOL "")
  endif()
endif()