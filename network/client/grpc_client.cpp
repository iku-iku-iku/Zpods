#include "grpc_client.h"

ABSL_FLAG(std::string, target, "localhost:50051", "Server address");
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

FileServiceClient get_file_service_client() {
    std::string target_str = absl::GetFlag(FLAGS_target);

    FileServiceClient client(grpc::CreateChannel(target_str, MakeClientSslCredentials()));

    return client;
}

UserServiceClient get_user_service_client() {
    std::string target_str = absl::GetFlag(FLAGS_target);

    UserServiceClient client(grpc::CreateChannel(target_str, MakeClientSslCredentials()));

    return client;
}

//int main(int argc, char** argv) {
//  absl::ParseCommandLine(argc, argv);
//  // Instantiate the client. It requires a channel, out of which the actual RPCs
//  // are created. This channel models a connection to an endpoint specified by
//  // the argument "--target=" which is the only expected argument.
////  bool reply = client.UploadFile("/tmp/xxx");
////  if (reply) {
////    std::cout << "success" << std::endl;
////  } else {
////    std::cout << "failed" << std::endl;
////  }
//
//  return 0;
//}
zpods::Status zpods::RpcUser::register_() const {
    UserServiceClient client = get_user_service_client();

    client.Register(username, password);

    return Status::OK;
}

zpods::Status zpods::RpcUser::login() const {
    UserServiceClient client = get_user_service_client();

    client.Login(username, password);

    return Status::OK;
}
