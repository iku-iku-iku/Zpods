# Copyright 2018 gRPC authors.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
# cmake build file for C++ helloworld example.
# Assumes protobuf and gRPC have been installed using cmake.
# See cmake_externalproject/CMakeLists.txt for all-in-one cmake build
# that automatically builds all the dependencies before building helloworld.

cmake_minimum_required(VERSION 3.8)

project(zpods_server C CXX)

set(CXX_STANDARD 17)

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
find_path(ROCKSDB_INCLUDE_DIR rocksdb/db.h PATHS)
include_directories(${ROCKSDB_INCLUDE_DIR})

add_library(zpods_grpc_proto
        ${zpods_grpc_srcs}
        ${zpods_grpc_hdrs}
        ${zpods_proto_srcs}
        ${zpods_proto_hdrs})
target_link_libraries(zpods_grpc_proto
        ${_REFLECTION}
        ${_GRPC_GRPCPP}
        ${_PROTOBUF_LIBPROTOBUF})

# Targets greeter_[async_](client|server)
add_executable(zpods_server "zpods_server.cpp")
target_link_libraries(zpods_server
        zpods_grpc_proto
        absl::flags
        absl::flags_parse
        ${_REFLECTION}
        ${_GRPC_GRPCPP}
        ${_PROTOBUF_LIBPROTOBUF}
        -lpthread
        -lrocksdb
        -ldl
        -lzstd
)
