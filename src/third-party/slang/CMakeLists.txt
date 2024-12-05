include(FetchContent)

FetchContent_Declare(
    slang
    GIT_REPOSITORY https://github.com/shader-slang/slang.git
    GIT_TAG 4003d337a7e9b84a8c0609bb8069460f82f5d673
    EXCLUDE_FROM_ALL
    SYSTEM
)

message(STATUS "Downloading and configuring Slang Shader Compiler. This may take a few minutes.")
set(SLANG_ENABLE_TESTS OFF CACHE BOOL "")
set(SLANG_ENABLE_GFX OFF CACHE BOOL "")
set(SLANG_ENABLE_SLANG_RHI OFF CACHE BOOL "")
set(SLANG_ENABLE_EXAMPLES OFF CACHE BOOL "")
set(SLANG_ENABLE_SLANGC OFF CACHE BOOL "")
FetchContent_MakeAvailable(slang)

macro(copy_slang_dlls _module_name)
    add_custom_command(TARGET ${_module_name} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:slang> $<TARGET_FILE_DIR:${_module_name}>
        COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:slang-rt> $<TARGET_FILE_DIR:${_module_name}>)
endmacro()

FetchContent_GetProperties(slang
    SOURCE_DIR slang_source_dir)

message(STATUS "Slang source dir is set to ${slang_source_dir}")
add_library(slang_interface INTERFACE)
target_link_libraries(slang_interface INTERFACE slang slang-rt)
target_include_directories(slang_interface INTERFACE ${slang_source_dir}/include)
add_library(avio::slang ALIAS slang_interface)