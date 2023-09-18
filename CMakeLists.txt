cmake_minimum_required(VERSION 3.10)
project(Zpods)
set(CMAKE_CXX_STANDARD 23)

add_subdirectory(extern/googletest) 
add_subdirectory(ZpodsLib)
add_subdirectory(extern/spdlog)