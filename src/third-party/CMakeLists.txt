option(AVIO_USES_CUSTOM_SPDLOG "Whether the custom spdlog version is specified. This will not include spdlog in that case.")
if(NOT AVIO_USES_CUSTOM_SPDLOG)
    add_library(spdlog INTERFACE)
endif()
target_include_directories(spdlog INTERFACE spdlog/include)

add_subdirectory(DirectX-Headers)

# TODO: Remove slang when compiling shipping build
option(AVIO_ENABLE_SLANG "Whether to enable slang shader compiler" ON)

if(AVIO_ENABLE_SLANG)
    add_subdirectory(slang)
endif()