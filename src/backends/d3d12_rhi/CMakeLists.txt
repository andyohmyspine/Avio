create_static_module(d3d12_rhi)

target_link_libraries(d3d12_rhi PUBLIC 
  avio::rhi 
  DirectX-Headers 
  DirectX-Guids
  dxgi
  d3d12)
add_library(avio::d3d12_rhi ALIAS d3d12_rhi)