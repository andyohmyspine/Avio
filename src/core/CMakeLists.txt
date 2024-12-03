avio_create_static_module(avio_core)

add_library(avio::core ALIAS avio_core)
target_precompile_headers(avio_core PUBLIC core_pch.hpp)

target_link_libraries(avio_core
  PUBLIC
    spdlog)