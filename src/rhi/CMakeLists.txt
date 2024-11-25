create_static_module(rhi)
add_library(avio::rhi ALIAS rhi)

target_link_libraries(rhi PUBLIC avio::core)

if(AVIO_ENABLE_GPU_VALIDATION)
  target_compile_definitions(rhi PUBLIC AVIO_ENABLE_GPU_VALIDATION)
endif()

if(AVIO_USING_D3D12)
  target_link_libraries(rhi PUBLIC avio::d3d12_rhi)
elseif(AVIO_USING_VULKAN)
  target_link_libraries(rhi PUBLIC avio::vulkan_rhi)
endif()