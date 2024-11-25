create_static_module(engine)
add_library(avio::engine ALIAS engine)

target_link_libraries(engine PUBLIC avio::core avio::rhi)