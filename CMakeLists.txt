cmake_minimum_required(VERSION 3.10)
project(Zpods)
set(CMAKE_CXX_STANDARD 23)

include(CMakeModules/GlobalIncludeDir.cmake)

add_compile_definitions(PROJECT_PATH="${PROJECT_SOURCE_DIR}")
add_compile_definitions(TEST_DATA_PATH="${PROJECT_SOURCE_DIR}/test_data")
add_compile_definitions(TEMP_PATH="${PROJECT_SOURCE_DIR}/tmp")

add_subdirectory(extern/googletest)
add_subdirectory(extern/spdlog)

include_directories(extern/ormpp/include)
#include_directories(/usr/include/mysql)
#include_directories(/usr/include/mysql++)
include_directories(extern/spdlog/include ./)

set(PCH_PATH ${CMAKE_SOURCE_DIR}/pch.h)

add_subdirectory(ZpodsLib)
add_subdirectory(server)
add_subdirectory(client)