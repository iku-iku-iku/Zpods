project(zpods_lib)

FILE(GLOB_RECURSE SRC *.cpp *.c)

find_package(OpenSSL REQUIRED)

add_library(${PROJECT_NAME} STATIC zpods_lib.h ${SRC})
target_link_libraries(${PROJECT_NAME} PRIVATE OpenSSL::SSL OpenSSL::Crypto)

target_precompile_headers(${PROJECT_NAME} PRIVATE pch.h)