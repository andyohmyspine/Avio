macro(avio_collect_module_files_cpp)
    file(GLOB_RECURSE _module_files *.cpp *.hpp *.inl LIST_DIRECTORIES ON)
endmacro()

macro(avio_default_module_properties _name)
    target_compile_features(${_name} PUBLIC cxx_std_23)
    target_include_directories(${_name} PUBLIC ${CMAKE_CURRENT_LIST_DIR})
endmacro()

macro(avio_create_executable_module _name)
    avio_collect_module_files_cpp()
    add_executable(${_name} ${_module_files})
    avio_default_module_properties(${_name})
endmacro()

macro(avio_create_static_module _name)
    avio_collect_module_files_cpp()
    add_library(${_name} STATIC ${_module_files})
    avio_default_module_properties(${_name})
endmacro()

macro(avio_copy_assets _module_name)
    add_custom_command(TARGET ${_module_name} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory $<TARGET_PROPERTY:${_module_name},SOURCE_DIR>/assets/ $<TARGET_FILE_DIR:${_module_name}>/assets)
endmacro()