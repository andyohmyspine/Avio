create_static_module(vulkan_rhi)

find_package(Vulkan REQUIRED)
target_link_libraries(vulkan_rhi PUBLIC 
  avio::rhi
  Vulkan::Vulkan)
add_library(avio::vulkan_rhi ALIAS vulkan_rhi)

target_precompile_headers(vulkan_rhi PRIVATE vulkan_pch.hpp)