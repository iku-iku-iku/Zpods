project(zpods_lib)

FILE(GLOB_RECURSE ALL_SRC *.cpp *.c)

FOREACH (F ${ALL_SRC})

    SET(LIB_SRC ${LIB_SRC} ${F})

    MESSAGE("Use " ${F})

ENDFOREACH (F)

add_library(${PROJECT_NAME} STATIC ${LIB_SRC})