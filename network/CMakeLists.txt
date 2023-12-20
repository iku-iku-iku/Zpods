set(GENERATED ${CMAKE_CURRENT_SOURCE_DIR}/generated/)

include(common.cmake)
# Proto file
get_filename_component(ZPODS_PROTO "zpods.proto" ABSOLUTE)
get_filename_component(ZPODS_PROTO_PATH "${ZPODS_PROTO}" PATH)

# Generated sources
set(ZPODS_PROTO_SRCS "${GENERATED}/zpods.pb.cc")
set(ZPODS_PROTO_HDRS "${GENERATED}/zpods.pb.h")
set(ZPODS_GRPC_SRCS "${GENERATED}/zpods.grpc.pb.cc")
set(ZPODS_GRPC_HDRS "${GENERATED}/zpods.grpc.pb.h")
add_custom_command(
        OUTPUT "${ZPODS_PROTO_SRCS}" "${ZPODS_PROTO_HDRS}" "${ZPODS_GRPC_SRCS}" "${ZPODS_GRPC_HDRS}"
        COMMAND ${_PROTOBUF_PROTOC}
        ARGS --grpc_out "${GENERATED}"
        --cpp_out "${GENERATED}"
        -I "${ZPODS_PROTO_PATH}"
        --plugin=protoc-gen-grpc="${_GRPC_CPP_PLUGIN_EXECUTABLE}"
        "${ZPODS_PROTO}"
        DEPENDS "${ZPODS_PROTO}")

# Include generated *.pb.h files
include_directories("${GENERATED}")

add_library(zpods_grpc_proto
        ${ZPODS_GRPC_SRCS}
        ${ZPODS_GRPC_HDRS}
        ${ZPODS_PROTO_SRCS}
        ${ZPODS_PROTO_HDRS})
target_link_libraries(zpods_grpc_proto
        ${_REFLECTION}
        ${_GRPC_GRPCPP}
        ${_PROTOBUF_LIBPROTOBUF})

# add_subdirectory(server)
add_subdirectory(client)
