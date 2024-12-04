if(AVIO_D3D12_AVAILABLE)
    message(STATUS "D3D12 available.")
    add_subdirectory(d3d12_rhi)
endif()

if(AVIO_VULKAN_AVAILABLE)
    message(STATUS "Vulkan available.")
    add_subdirectory(vulkan_rhi)
endif()
