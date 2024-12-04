cmake_minimum_required(VERSION 3.25)
project(Avio)

if(MSVC)
    add_compile_options(/MP)
endif()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_LIST_DIR}/cmake/modules)

# Options
option(AVIO_ENABLE_GPU_VALIDATION "whether to enable GPU validation." ON)

add_definitions(-DCMAKE_EXPORT_COMPILE_COMMANDS=ON)
add_compile_definitions(_CRT_SECURE_NO_WARNINGS)

include("cmake/Macros.cmake")
add_subdirectory(src)

# 
# if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_CURRENT_LIST_DIR})
#     set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
#     set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
#     set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/arch)
# endif() 
# 

option(AVIO_BUILD_EXAMPLES "Whether to build examples" ON)
if(AVIO_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()
