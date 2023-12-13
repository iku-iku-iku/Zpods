//
// Created by code4love on 23-12-7.
//

#ifndef ZPODS_CLIENT_PCH_H
#define ZPODS_CLIENT_PCH_H

#include <fstream>
#include <iostream>
#include <memory>
#include <string>

#include "absl/flags/flag.h"
#include "absl/flags/parse.h"
#include <grpcpp/grpcpp.h>
#include <grpcpp/security/credentials.h>

#include "zpods.grpc.pb.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::ClientWriter;
using grpc::Status;

#endif // ZPODS_CLIENT_PCH_H
