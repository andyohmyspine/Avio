cmake_minimum_required(VERSION 3.20)
project(Avio)

add_definitions(-DCMAKE_EXPORT_COMPILE_COMMANDS=ON)

include("cmake/Macros.cmake")
add_subdirectory(src)

if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_CURRENT_LIST_DIR})
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/arch)
endif() 

option(AVIO_BUILD_EXAMPLES "Whether to build examples" ON)
if(AVIO_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()