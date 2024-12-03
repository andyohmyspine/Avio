avio_create_static_module(avio_engine)
add_library(avio::engine ALIAS avio_engine)

target_link_libraries(avio_engine PUBLIC avio::core avio::rhi)

macro(avio_copy_required_dlls _module_name)
  copy_slang_dlls(${_module_name})
endmacro()