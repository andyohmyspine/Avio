create_static_module(core)

add_library(avio::core ALIAS core)
target_precompile_headers(core PUBLIC core_pch.hpp)

target_link_libraries(core
  PUBLIC
    spdlog)