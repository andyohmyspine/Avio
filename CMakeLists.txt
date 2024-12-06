cmake_minimum_required(VERSION 3.25)
project(Avio)

include(CheckIPOSupported)
check_ipo_supported(RESULT IPO_SUPPORTED)

if(MSVC)
    add_compile_options(/MP /Zc:nrvo)
endif()

include(CMakeDependentOption)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_LIST_DIR}/cmake/modules)

if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_CURRENT_LIST_DIR})
    set_property(GLOBAL PROPERTY INTERPROCEDURAL_OPTIMIZATION ON)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/archive)
endif()

# Options
option(AVIO_ENABLE_GPU_VALIDATION "whether to enable GPU validation." ON)
option(AVIO_ENABLE_EXAMPLES "Whether to build examples" OFF)

if(AVIO_ENABLE_EXAMPLES)
    set(AVIO_ENABLE_GLFW ON CACHE BOOL "" FORCE)
    set(AVIO_ENABLE_GLFW_HELPERS ON CACHE BOOL "" FORCE)
endif()

option(AVIO_ENABLE_GLFW "Whether to enable included glfw module." OFF)
option(AVIO_USES_CUSTOM_GLFW "Whether there is a custom glfw already in the project" OFF)

add_definitions(-DCMAKE_EXPORT_COMPILE_COMMANDS=ON)
add_compile_definitions(_CRT_SECURE_NO_WARNINGS)

include("cmake/Macros.cmake")
add_subdirectory(src)

if(AVIO_ENABLE_EXAMPLES)
    add_subdirectory(examples)
endif()
