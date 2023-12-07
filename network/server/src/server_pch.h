//
// Created by code4love on 23-12-7.
//

#ifndef ZPODS_SERVER_PCH_H
#define ZPODS_SERVER_PCH_H

#include <iostream>
#include <memory>
#include <string>
#include <fstream>
#include <rocksdb/db.h>
#include <openssl/evp.h>
#include <iomanip>
#include <optional>

#include "absl/flags/flag.h"
#include "absl/flags/parse.h"
#include "absl/strings/str_format.h"

#include <grpcpp/ext/proto_server_reflection_plugin.h>
#include <grpcpp/grpcpp.h>
#include <grpcpp/health_check_service_interface.h>
#include <grpcpp/security/credentials.h>
#include "zpods.grpc.pb.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;
using grpc::ServerReader;


#endif //ZPODS_PCH_H
