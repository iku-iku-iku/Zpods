project(zpods_cli)

FILE(GLOB_RECURSE SRC *.cpp *.c)

#include_directories(${ZPODS_LIB_SOURCE_DIR}/base ${ZPODS_LIB_SOURCE_DIR}/core ${ZPODS_LIB_SOURCE_DIR}/network ${ZPODS_CLI_SOURCE_DIR}/)
include_directories_from_global_property(ZPODS_LIB_INCLUDE_DIRS)

add_executable(${PROJECT_NAME} ${SRC})
target_link_libraries(${PROJECT_NAME} PRIVATE zpods_lib zpods_grpc_client gtest gtest_main spdlog)
target_precompile_headers(${PROJECT_NAME} PRIVATE ${PCH_PATH})