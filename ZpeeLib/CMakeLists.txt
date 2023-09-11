cmake_minimum_required(VERSION 3.10)
project(ZpeeLib)

set(CMAKE_CXX_STANDARD 23)

add_subdirectory(test)

add_library(ZpeeLib library.cpp)
