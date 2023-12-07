/*
 *
 * Copyright 2015 gRPC authors.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

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
using zpods::FileService;
using zpods::FileChunk;
using zpods::UploadStatus;
using zpods::UserService;
using zpods::RegisterRequest;
using zpods::RegisterResponse;
using zpods::LoginRequest;
using zpods::LoginResponse;

ABSL_FLAG(uint16_t, port, 50051, "Server port for the service");

class FileServiceImpl final : public FileService::Service {
public:
  Status UploadFile(ServerContext* context, ServerReader<FileChunk>* reader, UploadStatus* response) override {
    std::optional<std::ofstream> out;
    FileChunk chunk;
    while (reader->Read(&chunk)) {
      if (!out) {
        out = std::ofstream(chunk.filename(), std::ios::binary);
      }
      out->write(chunk.content().data(), chunk.content().size());
    }
    out->close();
    response->set_success(true);
    return Status::OK;
  }
};

std::string sha256(const std::string& str) {
    unsigned char hash[EVP_MAX_MD_SIZE];
    unsigned int lengthOfHash = 0;

    EVP_MD_CTX* sha256 = EVP_MD_CTX_new();
    if (!sha256) {
        // 处理错误
    }

    if (1 != EVP_DigestInit_ex(sha256, EVP_sha256(), nullptr)) {
        // 处理错误
    }

    if (1 != EVP_DigestUpdate(sha256, str.c_str(), str.size())) {
        // 处理错误
    }

    if (1 != EVP_DigestFinal_ex(sha256, hash, &lengthOfHash)) {
        // 处理错误
    }

    EVP_MD_CTX_free(sha256);

    std::stringstream ss;
    for (unsigned int i = 0; i < lengthOfHash; ++i) {
        ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(hash[i]);
    }

    return ss.str();
}

class UserServiceImpl final : public UserService::Service {
    rocksdb::DB* db;
public:
    UserServiceImpl() {
        rocksdb::Options options;
        options.create_if_missing = true;
        rocksdb::Status status = rocksdb::DB::Open(options, "userdb", &db);
        assert(status.ok());
    }

    ~UserServiceImpl() override {
        delete db;
    }

    std::string make_username_key(const std::string& username) {
      return "user:" + username;
    }

    grpc::Status Register(grpc::ServerContext* context, const RegisterRequest* request,
                          RegisterResponse* response) override {
        std::string username = make_username_key(request->username());
        std::string password = sha256(request->password());

        // Check if user already exists
        std::string value;
        rocksdb::Status s = db->Get(rocksdb::ReadOptions(), username, &value);
        if (s.ok()) {
            response->set_success(false);
            response->set_message("User already exists");
            return grpc::Status::OK;
        }

        // Put in a database
        db->Put(rocksdb::WriteOptions(), username, password);

        response->set_success(true);
        response->set_message("User registered successfully");
        return grpc::Status::OK;
    }

    grpc::Status Login(grpc::ServerContext* context, const LoginRequest* request,
                       LoginResponse* response) override {
        std::string username = make_username_key(request->username());
        std::string password = sha256(request->password());

        std::string stored_password;
        rocksdb::Status s = db->Get(rocksdb::ReadOptions(), username, &stored_password);
        if (s.ok() && stored_password == password) {
            response->set_success(true);
            response->set_message("Login successful");
        } else {
            response->set_success(false);
            response->set_message("Invalid username or password");
        }
        return grpc::Status::OK;
    }
};

auto MakeServerSslCredentials() {
  auto ReadFile = [](const std::string& filename) {
      std::ifstream file(filename, std::ios::in | std::ios::binary);
      std::stringstream buffer;
      buffer << file.rdbuf();
      return buffer.str();
  };

  // read server key and certificate
  std::string server_key = ReadFile("server.key");
  std::string server_cert = ReadFile("server.crt");
  std::string client_cert = ReadFile("client.crt"); 

  // make a key/cert pair
  grpc::SslServerCredentialsOptions::PemKeyCertPair key_cert_pair{server_key, server_cert};
  grpc::SslServerCredentialsOptions ssl_opts;

  ssl_opts.pem_root_certs = client_cert; // 用于验证客户端
  // add the key/cert pair to the credentials
  ssl_opts.pem_key_cert_pairs.push_back(key_cert_pair);
  ssl_opts.client_certificate_request = GRPC_SSL_REQUEST_AND_REQUIRE_CLIENT_CERTIFICATE_AND_VERIFY;

  return grpc::SslServerCredentials(ssl_opts);
}

void RunServer(uint16_t port) {
  std::string server_address = absl::StrFormat("0.0.0.0:%d", port);
  FileServiceImpl service;

  grpc::EnableDefaultHealthCheckService(true);
  grpc::reflection::InitProtoReflectionServerBuilderPlugin();
  UserServiceImpl user_service;


  grpc::ServerBuilder builder;
  // Listen on the given address without any authentication mechanism.
  builder.AddListeningPort(server_address, MakeServerSslCredentials());
  // Register "service" as the instance through which we'll communicate with
  // clients. In this case it corresponds to an *synchronous* service.
  builder.RegisterService(&service);
  builder.RegisterService(&user_service);
  // Finally assemble the server.
  std::unique_ptr<Server> server(builder.BuildAndStart());
  std::cout << "Server listening on " << server_address << std::endl;

  // Wait for the server to shut down. Note that some other thread must be
  // responsible for shutting down the server for this call to ever return.
  server->Wait();
}
int main(int argc, char** argv) {
  absl::ParseCommandLine(argc, argv);
  RunServer(absl::GetFlag(FLAGS_port));
  return 0;
}
