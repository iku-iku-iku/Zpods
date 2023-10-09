project(zpods_server)

FILE(GLOB_RECURSE SRC *.cpp *.c)

add_executable(${PROJECT_NAME} ${SRC})

include_directories(/usr/include/mysql)
include_directories(/usr/include/mysql++)
target_link_directories(/usr/include/mysql++)


target_link_libraries(${PROJECT_NAME} zpods_lib gtest gtest_main spdlog ${RADOS_LIBRARIES} mysqlpp)
target_link_libraries(${PROJECT_NAME} /usr/lib/x86_64-linux-gnu/libmysqlpp.so)
target_precompile_headers(${PROJECT_NAME} PRIVATE ${PCH_PATH})
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/ZpodsLib/src ${RADOS_INCLUDE_DIRS})
