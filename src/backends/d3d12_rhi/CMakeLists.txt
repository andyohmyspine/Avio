avio_create_static_module(avio_d3d12_rhi)

target_link_libraries(avio_d3d12_rhi PUBLIC 
  avio::rhi 
  DirectX-Headers 
  DirectX-Guids
  dxgi
  d3d12)
add_library(avio::d3d12_rhi ALIAS avio_d3d12_rhi)
target_precompile_headers(avio_d3d12_rhi PRIVATE d3d12_pch.hpp)

include(FetchContent)

message(STATUS "Downloading and configuring D3D12 memory allocator")
FetchContent_Declare(D3D12MA
  GIT_REPOSITORY https://github.com/GPUOpen-LibrariesAndSDKs/D3D12MemoryAllocator
  GIT_TAG 6a61103936dfadedca584e1a521fab6943d3b5ad)

FetchContent_MakeAvailable(D3D12MA)