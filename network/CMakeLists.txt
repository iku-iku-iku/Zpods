set(GENERATED ${CMAKE_CURRENT_SOURCE_DIR}/generated/)

include(common.cmake)
# Proto file
get_filename_component(zpods_proto "zpods.proto" ABSOLUTE)
get_filename_component(zpods_proto_path "${zpods_proto}" PATH)

# Generated sources
set(zpods_proto_srcs "${GENERATED}/zpods.pb.cc")
set(zpods_proto_hdrs "${GENERATED}/zpods.pb.h")
set(zpods_grpc_srcs "${GENERATED}/zpods.grpc.pb.cc")
set(zpods_grpc_hdrs "${GENERATED}/zpods.grpc.pb.h")
add_custom_command(
        OUTPUT "${zpods_proto_srcs}" "${zpods_proto_hdrs}" "${zpods_grpc_srcs}" "${zpods_grpc_hdrs}"
        COMMAND ${_PROTOBUF_PROTOC}
        ARGS --grpc_out "${GENERATED}"
        --cpp_out "${GENERATED}"
        -I "${zpods_proto_path}"
        --plugin=protoc-gen-grpc="${_GRPC_CPP_PLUGIN_EXECUTABLE}"
        "${zpods_proto}"
        DEPENDS "${zpods_proto}")

# Include generated *.pb.h files
include_directories("${GENERATED}")

add_library(zpods_grpc_proto
        ${zpods_grpc_srcs}
        ${zpods_grpc_hdrs}
        ${zpods_proto_srcs}
        ${zpods_proto_hdrs})
target_link_libraries(zpods_grpc_proto
        ${_REFLECTION}
        ${_GRPC_GRPCPP}
        ${_PROTOBUF_LIBPROTOBUF})

add_subdirectory(server)
add_subdirectory(client)