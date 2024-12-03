add_library(spdlog INTERFACE)
target_include_directories(spdlog INTERFACE spdlog/include)

add_subdirectory(DirectX-Headers)

# TODO: Remove slang when compiling shipping build
option(AVIO_ENABLE_SLANG "Whether to enable slang shader compiler" ON)

if(AVIO_ENABLE_SLANG)
    add_subdirectory(slang)
endif()