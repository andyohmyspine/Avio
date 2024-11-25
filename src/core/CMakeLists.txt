create_static_module(core)

add_library(avio::core ALIAS core)

target_link_libraries(core
  PUBLIC
    spdlog)