project(zpods_lib)

FILE(GLOB_RECURSE SRC *.cpp *.c)

find_package(OpenSSL REQUIRED)

include_directories(./base ./core ./network ./)
add_library(${PROJECT_NAME} SHARED zpods_lib.h ${SRC})

target_link_libraries(${PROJECT_NAME} PRIVATE OpenSSL::SSL OpenSSL::Crypto)

target_precompile_headers(${PROJECT_NAME} PRIVATE ${PCH_PATH})