cmake_minimum_required(VERSION 3.10)

add_executable(ZpodsLib_test test1.cpp)

target_link_libraries(ZpodsLib_test ZpodsLib gtest gtest_main)

include_directories("../")