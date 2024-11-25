add_library(avio INTERFACE)
add_library(avio::avio ALIAS avio)

add_subdirectory(core)
add_subdirectory(rhi)
add_subdirectory(engine)
add_subdirectory(third-party)

target_link_libraries(avio
    INTERFACE
        avio::core
        avio::rhi
        avio::engine)