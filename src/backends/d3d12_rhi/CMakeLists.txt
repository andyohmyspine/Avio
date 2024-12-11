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

  message(STATUS "Downloading and configuring DirectX headers")
  FetchContent_Declare(DirectX-Headers
    GIT_REPOSITORY https://github.com/microsoft/DirectX-Headers
    GIT_TAG 48a762973271c5a75869946bf1fdbc489a628a5c)

FetchContent_MakeAvailable(D3D12MA DirectX-Headers)

# There are a few errors in D3D12MA (fix them somehow)
# target_link_libraries(avio_d3d12_rhi PRIVATE D3D12MemoryAllocator)