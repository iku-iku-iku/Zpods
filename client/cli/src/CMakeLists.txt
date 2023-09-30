project(zpods_cli)

FILE(GLOB_RECURSE SRC *.cpp *.c)

add_executable(${PROJECT_NAME} ${SRC})
target_link_libraries(${PROJECT_NAME} zpods_lib gtest gtest_main spdlog)
target_precompile_headers(${PROJECT_NAME} PRIVATE ${PCH_PATH})
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/ZpodsLib/src)