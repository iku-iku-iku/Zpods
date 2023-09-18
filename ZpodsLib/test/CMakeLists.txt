cmake_minimum_required(VERSION 3.10)

project(zpods_lib_test)

add_executable(${PROJECT_NAME} test1.cpp)

target_link_libraries(${PROJECT_NAME} zpods_lib gtest gtest_main spdlog)

include_directories("../src")