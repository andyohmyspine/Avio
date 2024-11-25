create_static_module(rhi)
add_library(avio::rhi ALIAS rhi)


target_link_libraries(rhi PUBLIC core)