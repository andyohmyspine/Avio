find_package(Vulkan QUIET)

if(AVIO_D3D12_AVAILABLE)
    add_subdirectory(d3d12_rhi)
endif()

if(AVIO_VULKAN_AVAILABLE)
    add_subdirectory(vulkan_rhi)
endif()
