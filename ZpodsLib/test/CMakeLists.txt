project(zpods_lib_test)

FILE(GLOB_RECURSE SRC *.cpp *.c *.ixx)

include_directories("../src" "../src/base" "../src/network" "../src/core")

add_executable(${PROJECT_NAME} ${SRC})
target_link_libraries(${PROJECT_NAME} zpods_lib gtest gtest_main spdlog)