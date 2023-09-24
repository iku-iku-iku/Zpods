cmake_minimum_required(VERSION 3.10)
project(Zpods)
set(CMAKE_CXX_STANDARD 23)

add_compile_definitions(PROJECT_PATH="${PROJECT_SOURCE_DIR}")
add_compile_definitions(TEST_DATA_PATH="${PROJECT_SOURCE_DIR}/test_data")
add_compile_definitions(TEMP_PATH="${PROJECT_SOURCE_DIR}/tmp")

include_directories(extern/spdlog/include)

add_subdirectory(extern/googletest) 
add_subdirectory(ZpodsLib)
add_subdirectory(extern/spdlog)