cmake_minimum_required(VERSION 3.10)
project(Zpods)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

SET(CMAKE_BUILD_TYPE "Debug")
SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g2 -ggdb")
SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")

include(CMakeModules/GlobalIncludeDir.cmake)

set(TEST_DATA_PATH ${PROJECT_SOURCE_DIR}/test_data)
set(TEMP_PATH ${PROJECT_SOURCE_DIR}/tmp)

add_compile_definitions(PROJECT_PATH="${PROJECT_SOURCE_DIR}")
add_compile_definitions(TEST_DATA_PATH="${TEST_DATA_PATH}")
add_compile_definitions(TEMP_PATH="${TEMP_PATH}")

message("project path is" ${PROJECT_SOURCE_DIR})
message("test data path is" ${TEST_DATA_PATH})
message("temp path is" ${TEMP_PATH})

add_subdirectory(extern/googletest)
add_subdirectory(extern/spdlog)

include_directories(extern/ormpp/include)
include_directories(extern/spdlog/include ${CMAKE_SOURCE_DIR})

set(PCH_PATH ${CMAKE_SOURCE_DIR}/pch.h)

add_subdirectory(ZpodsLib)
add_subdirectory(network)
