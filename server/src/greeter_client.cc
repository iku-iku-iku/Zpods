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

#include "absl/flags/flag.h"
#include "absl/flags/parse.h"
#include <grpcpp/security/credentials.h>
#include <grpcpp/grpcpp.h>

#ifdef BAZEL_BUILD
#include "examples/protos/helloworld.grpc.pb.h"
#else
#include "helloworld.grpc.pb.h"
#endif

ABSL_FLAG(std::string, target, "localhost:50051", "Server address");

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using grpc::ClientWriter;
using helloworld::Greeter;
using helloworld::HelloReply;
using helloworld::HelloRequest;
using helloworld::FileChunk;
using helloworld::UploadStatus;
using helloworld::Zpods;


class GreeterClient {
 public:
  GreeterClient(std::shared_ptr<Channel> channel)
      : stub_(Greeter::NewStub(channel)) {}

  // Assembles the client's payload, sends it and presents the response back
  // from the server.
  std::string SayHello(const std::string& user) {
    // Data we are sending to the server.
    HelloRequest request;
    request.set_name(user);

    // Container for the data we expect from the server.
    HelloReply reply;

    // Context for the client. It could be used to convey extra information to
    // the server and/or tweak certain RPC behaviors.
    ClientContext context;

    // The actual RPC.
    Status status = stub_->SayHello(&context, request, &reply);

    // Act upon its status.
    if (status.ok()) {
      return reply.message();
    } else {
      std::cout << status.error_code() << ": " << status.error_message()
                << std::endl;
      return "RPC failed";
    }
  }

 private:
  std::unique_ptr<Greeter::Stub> stub_;
};

class FileTransferClient {
public:
  FileTransferClient(std::shared_ptr<Channel> channel) : stub_(Zpods::NewStub(channel)) {}

  bool UploadFile(const std::string& file_name) {
    std::ifstream file(file_name, std::ios::binary);
    ClientContext context;
    UploadStatus status;
    std::unique_ptr<ClientWriter<FileChunk>> writer(stub_->UploadFile(&context, &status));

    FileChunk chunk;
    chunk.set_filename(file_name);
    char buffer[4096];  // Buffer size can be adjusted
    while (file.read(buffer, sizeof(buffer))) {
      chunk.set_content(buffer, sizeof(buffer));
      if (!writer->Write(chunk)) {
        // Broken stream
        break;
      }
    }
    // Handle the last piece of the file
    if (file.gcount() > 0) {
      chunk.set_content(buffer, file.gcount());
      writer->Write(chunk);
    }
    file.close();
    writer->WritesDone();
    Status finish_status = writer->Finish();
    return finish_status.ok() && status.success();
  }

private:
  std::unique_ptr<Zpods::Stub> stub_;
};

class UserServiceClient {
public:
  UserServiceClient(std::shared_ptr<Channel> channel) : stub_(helloworld::UserService::NewStub(channel)) {}

  void Register(const std::string& user, const std::string& password) {
    helloworld::RegisterRequest request;
    request.set_username(user);
    request.set_password(password);

    helloworld::RegisterResponse response;
    grpc::ClientContext context;

    grpc::Status status = stub_->Register(&context, request, &response);

    if (status.ok()) {
        std::cout << "Register status: " << response.message() << std::endl;
    } else {
        std::cout << "Register failed: " << status.error_code() << ": " << status.error_message() << std::endl;
    }
  }

  void Login(const std::string& user, const std::string& passworld) {
    helloworld::LoginRequest request;
    request.set_username(user);
    request.set_password(passworld);

    helloworld::LoginResponse response;
    grpc::ClientContext context;

    grpc::Status status = stub_->Login(&context, request, &response);

    if (status.ok()) {
        std::cout << "Login status: " << response.message() << std::endl;
    } else {
        std::cout << "Login failed: " << status.error_code() << ": " << status.error_message() << std::endl;
    }
  }

private:
  std::shared_ptr<helloworld::UserService::Stub> stub_;
};

auto MakeClientSslCredentials() {
  auto ReadFile = [](const std::string& filename) {
    std::ifstream file(filename);
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
  };

  std::string server_cert = ReadFile("server.crt"); // 服务端的证书
  std::string client_key = ReadFile("client.key"); // 客户端的密钥
  std::string client_cert = ReadFile("client.crt"); // 客户端的证书
  grpc::SslCredentialsOptions ssl_opts;
  ssl_opts.pem_root_certs = server_cert;
  ssl_opts.pem_private_key = client_key;
  ssl_opts.pem_cert_chain = client_cert;

  return grpc::SslCredentials(ssl_opts);
}

int main(int argc, char** argv) {
  absl::ParseCommandLine(argc, argv);
  // Instantiate the client. It requires a channel, out of which the actual RPCs
  // are created. This channel models a connection to an endpoint specified by
  // the argument "--target=" which is the only expected argument.
  std::string target_str = absl::GetFlag(FLAGS_target);

  FileTransferClient client(grpc::CreateChannel(target_str, MakeClientSslCredentials()));
  bool reply = client.UploadFile("/tmp/xxx");
  if (reply) {
    std::cout << "success" << std::endl;
  } else {
    std::cout << "failed" << std::endl;
  }

  return 0;
}
