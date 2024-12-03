avio_create_static_module(avio_vulkan_rhi)

find_package(Vulkan REQUIRED)
target_link_libraries(avio_vulkan_rhi PUBLIC 
  avio::rhi
  Vulkan::Vulkan)
add_library(avio::vulkan_rhi ALIAS avio_vulkan_rhi)

target_precompile_headers(avio_vulkan_rhi PRIVATE vulkan_pch.hpp)