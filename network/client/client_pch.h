//
// Created by code4love on 23-12-7.
//

#ifndef ZPODS_CLIENT_PCH_H
#define ZPODS_CLIENT_PCH_H

#include <iostream>
#include <memory>
#include <string>
#include <fstream>

#include "absl/flags/flag.h"
#include "absl/flags/parse.h"
#include <grpcpp/security/credentials.h>
#include <grpcpp/grpcpp.h>

#include "zpods.grpc.pb.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using grpc::ClientWriter;

#endif //ZPODS_CLIENT_PCH_H
