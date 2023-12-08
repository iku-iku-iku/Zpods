project(zpods_grpc_client)
add_library(${PROJECT_NAME}
        user_service_client.cpp
        file_service_client.cpp
        grpc_client.cpp
)

target_link_libraries(${PROJECT_NAME} zpods_grpc_proto)

add_subdirectory(cli)
add_subdirectory(gui)