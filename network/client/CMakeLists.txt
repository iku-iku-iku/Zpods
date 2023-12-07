add_library(zpods_grpc_client SHARED
        user_service_client.cpp
        file_service_client.cpp
        grpc_client.cpp
)

add_subdirectory(cli)
add_subdirectory(gui)