create_static_module(rhi)
add_library(avio::rhi ALIAS rhi)


target_link_libraries(rhi PUBLIC avio::core)

if(AVIO_D3D12_AVAILABLE)
  target_link_libraries(rhi PUBLIC avio::d3d12_rhi)
endif()