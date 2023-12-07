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


find_path(ROCKSDB_INCLUDE_DIR rocksdb/db.h PATHS)
include_directories(${ROCKSDB_INCLUDE_DIR})
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
