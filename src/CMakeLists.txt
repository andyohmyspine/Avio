add_library(avio INTERFACE)

add_subdirectory(core)

target_link_libraries(avio
    INTERFACE
        core)