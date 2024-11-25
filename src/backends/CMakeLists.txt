if(WIN32)
  set(AVIO_D3D12_AVAILABLE ON CACHE BOOL "")
else()
  set(AVIO_D3D12_AVAILABLE OFF CACHE BOOL "")
endif()

if(AVIO_D3D12_AVAILABLE)
  add_subdirectory(d3d12_rhi)
endif()