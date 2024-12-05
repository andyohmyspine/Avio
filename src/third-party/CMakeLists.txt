include(FetchContent)

option(AVIO_USES_CUSTOM_SPDLOG "Whether the custom spdlog version is specified. This will not include spdlog in that case.")
if(NOT AVIO_USES_CUSTOM_SPDLOG)
    add_library(spdlog INTERFACE)
endif()
target_include_directories(spdlog INTERFACE spdlog/include)

if(WIN32)
  message(STATUS "Downloading and configuring DirectX headers")
  FetchContent_Declare(DirectX-Headers
    GIT_REPOSITORY https://github.com/Microsoft/DirectX-Headers)
  FetchContent_MakeAvailable(DirectX-Headers)
endif()

# TODO: Remove slang when compiling shipping build
option(AVIO_ENABLE_SLANG "Whether to enable slang shader compiler" ON)

if(AVIO_ENABLE_SLANG)
    add_subdirectory(slang)
endif()

if(AVIO_ENABLE_GLFW AND NOT AVIO_USES_CUSTOM_GLFW)
  message(STATUS "Downloading and configuring GLFW. This may take a second or two.")
  set(GLFW_BUILD_DOCS OFF CACHE BOOL "")
  set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "")
  set(GLFW_BUILD_TESTS OFF CACHE BOOL "")
  FetchContent_Declare(glfw
    GIT_REPOSITORY https://github.com/glfw/glfw)
  FetchContent_MakeAvailable(glfw)
endif()