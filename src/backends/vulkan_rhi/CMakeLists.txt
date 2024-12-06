avio_create_static_module(avio_vulkan_rhi)

find_package(Vulkan REQUIRED)
target_link_libraries(avio_vulkan_rhi PUBLIC 
  avio::rhi
  Vulkan::Vulkan)
add_library(avio::vulkan_rhi ALIAS avio_vulkan_rhi)

target_precompile_headers(avio_vulkan_rhi PRIVATE vulkan_pch.hpp)

include(FetchContent)

message(STATUS "Downloading and configuring VulkanMemoryAllocator.")
FetchContent_Declare(VMA 
  GIT_REPOSITORY https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator
  GIT_TAG 009ecd192c1289c7529bff248a16cfe896254816)

FetchContent_MakeAvailable(VMA)

target_link_libraries(avio_vulkan_rhi PRIVATE GPUOpen::VulkanMemoryAllocator)
