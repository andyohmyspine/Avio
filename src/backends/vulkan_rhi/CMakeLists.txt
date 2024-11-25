create_static_module(vulkan_rhi)

find_package(Vulkan REQUIRED)
target_link_libraries(vulkan_rhi PUBLIC 
  avio::rhi
  Vulkan::Vulkan)
add_library(avio::d3d12_rhi ALIAS d3d12_rhi)