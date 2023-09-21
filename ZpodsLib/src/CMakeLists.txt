project(zpods_lib)

#FILE(GLOB_RECURSE ALL_SRC *.cpp *.c *.ixx)
FILE(GLOB_RECURSE SRC *.cpp *.c *.ixx)

#FOREACH (F ${ALL_SRC})
#
#    SET(LIB_SRC ${LIB_SRC} ${F})
#
#    MESSAGE("Use " ${F})
#
#ENDFOREACH (F)

add_library(${PROJECT_NAME} STATIC ${SRC})
include_directories(../../extern/spdlog/include)
