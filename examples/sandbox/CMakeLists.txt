avio_create_executable_module(sandbox)
target_link_libraries(sandbox PUBLIC avio glfw)
avio_copy_required_dlls(sandbox)

target_link_libraries(sandbox PUBLIC avio::glfw_helpers)