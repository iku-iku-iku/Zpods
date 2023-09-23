project(zpods_lib)

FILE(GLOB_RECURSE SRC *.cpp *.c)

#FOREACH (F ${ALL_SRC})
#
#    SET(LIB_SRC ${LIB_SRC} ${F})
#
#    MESSAGE("Use " ${F})
#
#ENDFOREACH (F)

add_library(${PROJECT_NAME} STATIC zpods_lib.h ${SRC})
include_directories(../../extern/spdlog/include)

target_precompile_headers(${PROJECT_NAME} PRIVATE pch.h)