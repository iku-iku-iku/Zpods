add_library(zpods_grpc_client SHARED
        grpc_client.cpp
)

add_subdirectory(cli)
add_subdirectory(gui)