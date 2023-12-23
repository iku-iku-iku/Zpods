project(zpods_lib)

FILE(GLOB_RECURSE SRC *.cpp *.c)

find_package(OpenSSL REQUIRED)

include_directories(./base ./core ./transform ./)
add_library(${PROJECT_NAME} SHARED zpods_lib.h ${SRC})

collect_and_set_global_include_dirs(${CMAKE_CURRENT_SOURCE_DIR} ZPODS_LIB_INCLUDE_DIRS)

target_link_libraries(${PROJECT_NAME} PRIVATE OpenSSL::SSL OpenSSL::Crypto)

target_precompile_headers(${PROJECT_NAME} PRIVATE ${PCH_PATH})
