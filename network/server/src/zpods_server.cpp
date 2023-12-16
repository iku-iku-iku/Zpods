#include "pod_service.h"
#include "user_service.h"

#include "absl/flags/flag.h"
#include <absl/flags/parse.h>
#include <grpcpp/ext/proto_server_reflection_plugin.h>
#include <grpcpp/health_check_service_interface.h>
#include <grpcpp/security/server_credentials.h>
#include <grpcpp/server_builder.h>

ABSL_FLAG(uint16_t, port, 50051, "Server port for the service");

auto MakeServerSslCredentials()
{
    auto ReadFile = [](const std::string& filename) {
        std::ifstream file(filename, std::ios::in | std::ios::binary);
        std::stringstream buffer;
        buffer << file.rdbuf();
        return buffer.str();
    };

    // read a server key and certificate
    std::string server_key = ReadFile("server.key");
    std::string server_cert = ReadFile("server.crt");
    std::string client_cert = ReadFile("client.crt");

    // make a key/cert pair
    grpc::SslServerCredentialsOptions::PemKeyCertPair key_cert_pair{
        server_key, server_cert};
    grpc::SslServerCredentialsOptions ssl_opts;

    ssl_opts.pem_root_certs = client_cert; // 用于验证客户端
    // add the key/cert pair to the credentials
    ssl_opts.pem_key_cert_pairs.push_back(key_cert_pair);
    ssl_opts.client_certificate_request =
        GRPC_SSL_REQUEST_AND_REQUIRE_CLIENT_CERTIFICATE_AND_VERIFY;

    return grpc::SslServerCredentials(ssl_opts);
}

void RunServer(uint16_t port)
{
    std::string server_address = absl::StrFormat("0.0.0.0:%d", port);

    grpc::EnableDefaultHealthCheckService(true);
    grpc::reflection::InitProtoReflectionServerBuilderPlugin();

    grpc::ServerBuilder builder;
    // Listen on the given address without any authentication mechanism.
    builder.AddListeningPort(server_address, MakeServerSslCredentials());
    // Register "service" as the instance through which we'll communicate with
    // clients. In this case, it corresponds to a *synchronous* service.
    PodServiceImpl file_service;
    UserServiceImpl user_service;
    builder.RegisterService(&file_service);
    builder.RegisterService(&user_service);
    // Finally, assemble the server.
    std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
    std::cout << "Server listening on " << server_address << std::endl;

    // Wait for the server to shut down. Note that some other thread must be
    // responsible for shutting down the server for this call to ever return.
    server->Wait();
}

int main(int argc, char** argv)
{
    absl::ParseCommandLine(argc, argv);
    RunServer(absl::GetFlag(FLAGS_port));
    return 0;
}
