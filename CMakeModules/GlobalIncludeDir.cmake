function(collect_and_set_global_include_dirs source_dir property_name)
    # 使用GLOB_RECURSE来获取所有头文件
    file(GLOB_RECURSE HEADER_LIST "${source_dir}/*.h")

    # 创建一个空的列表来保存所有头文件目录
    set(LOCAL_INCLUDE_DIRS "")

    # 遍历头文件列表，提取目录并添加到列表中
    foreach(HEADER ${HEADER_LIST})
        get_filename_component(DIR ${HEADER} DIRECTORY)
        list(APPEND LOCAL_INCLUDE_DIRS ${DIR})
    endforeach()

    # 移除重复的目录
    list(REMOVE_DUPLICATES LOCAL_INCLUDE_DIRS)

    # 设置全局属性
    set_property(GLOBAL PROPERTY ${property_name} ${LOCAL_INCLUDE_DIRS})
endfunction()

function(include_directories_from_global_property property_name)
    # 获取全局属性
    get_property(LOCAL_INCLUDE_DIRS GLOBAL PROPERTY ${property_name})

    # 添加到当前目录
    include_directories(${LOCAL_INCLUDE_DIRS})
endfunction()