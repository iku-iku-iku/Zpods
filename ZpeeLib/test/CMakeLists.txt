cmake_minimum_required(VERSION 3.10)

add_executable(ZpeeLib_test test1.cpp)

target_link_libraries(ZpeeLib_test ZpeeLib gtest gtest_main)

include_directories("../")