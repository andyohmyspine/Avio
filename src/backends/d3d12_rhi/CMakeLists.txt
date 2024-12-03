create_static_module(avio_d3d12_rhi)

target_link_libraries(avio_d3d12_rhi PUBLIC 
  avio::rhi 
  DirectX-Headers 
  DirectX-Guids
  dxgi
  d3d12)
add_library(avio::d3d12_rhi ALIAS avio_d3d12_rhi)
target_precompile_headers(avio_d3d12_rhi PRIVATE d3d12_pch.hpp)