#include <fstream>
#include "ZpodsLib/src/base/fs.h"
#include "grpc_client.h"
#include "network/client/pod_service_client.h"
#include "pch.h"

ABSL_FLAG(std::string, target, "127.0.0.1:50051", "Server address");

auto MakeClientSslCredentials()
{
    auto ReadFile = [](const std::string& filename) {
        let_mut filepath = filename;

        const char* appDir = getenv("APPDIR");
        if (appDir != nullptr)
        {
            filepath = zpods::fs::path(getenv("APPDIR")) / filename;
        }

        std::ifstream file(filepath);
        std::stringstream buffer;
        buffer << file.rdbuf();
        return buffer.str();
    };

    std::string server_cert = ReadFile("server.crt"); // 服务端的证书
    std::string client_key = ReadFile("client.key");  // 客户端的密钥
    std::string client_cert = ReadFile("client.crt"); // 客户端的证书
    grpc::SslCredentialsOptions ssl_opts;
    ssl_opts.pem_root_certs = server_cert;
    ssl_opts.pem_private_key = client_key;
    ssl_opts.pem_cert_chain = client_cert;

    return grpc::SslCredentials(ssl_opts);
}

static std::string get_target()
{
    std::string target;
    if (is_server_addr_set)
    {
        target = get_server_addr();
    }
    else if (zpods::DbHandle::Instance().get_cached_addr(&target) ==
             zpods::Status::OK)
    {
    }
    else
    {
        target = absl::GetFlag(FLAGS_target);
    }
    return target;
}

PodServiceClient get_pod_service_client()
{
    std::string target_str = get_target();

    PodServiceClient client(
        grpc::CreateChannel(target_str, MakeClientSslCredentials()));

    return client;
}

UserServiceClient get_user_service_client()
{
    std::string target_str = get_target();

    UserServiceClient client(
        grpc::CreateChannel(target_str, MakeClientSslCredentials()));

    return client;
}

// int main(int argc, char** argv) {
//   absl::ParseCommandLine(argc, argv);
//   // Instantiate the client. It requires a channel, out of which the actual
//   RPCs
//   // are created. This channel models a connection to an endpoint specified
//   by
//   // the argument "--target=" which is the only expected argument.
////  bool reply = client.UploadPod("/tmp/xxx");
////  if (reply) {
////    std::cout << "success" << std::endl;
////  } else {
////    std::cout << "failed" << std::endl;
////  }
//
//  return 0;
//}
//
zpods::RpcUser::RpcUser() : client_(get_pod_service_client()) {}

zpods::Status zpods::RpcUser::register_() const
{
    UserServiceClient client = get_user_service_client();

    client.Register(username, password);

    return Status::OK;
}

zpods::Status zpods::RpcUser::login() const
{
    UserServiceClient client = get_user_service_client();

    let status = client.Login(username, password);

    if (status != zpods::Status::OK)
    {
        return status;
    }

    return Status::OK;
}

zpods::Status zpods::RpcUser::upload_pod(const std::string& pod_path)
{
    CHECK_STATUS(client_.UploadPod(pod_path));
    return Status::OK;
}

zpods::Status zpods::RpcUser::upload_pods(const std::string& pods_dir)
{

    spdlog::info("uploading pods: {}", pods_dir);
    for (const auto& item : std::filesystem::directory_iterator(pods_dir))
    {
        if (item.is_directory())
        {
            continue;
        }
        CHECK_STATUS(upload_pod(item.path()));
    }

    return Status::OK;
}

zpods::Status zpods::RpcUser::query_pods(PodsQueryResult& result)
{
    let_mut client = get_pod_service_client();

    return client.QueryPods(result);
}

zpods::Status zpods::RpcUser::download_pod(const std::string& pods_name,
                                           const std::string& pod_name,
                                           const std::string& dir)
{
    let_mut client = get_pod_service_client();
    return client.DownloadPod(pods_name, pod_name, dir);
}

zpods::DbHandle& zpods::DbHandle::Instance()
{
    static DbHandle db_handle;
    return db_handle;
}

zpods::DbHandle::DbHandle()
{
    zpods::fs::create_directory_if_not_exist(get_db_path().c_str());
}

auto zpods::DbHandle::Put(const std::string& key, const std::string& value)
    -> zpods::Status
{
    let key_path = zpods::fs::path(get_db_path()) / key;
    let_mut ofs = zpods::fs::open_or_create_file_as_ofs(key_path.c_str(),
                                                        zpods::fs::ios::text);
    if (ofs.is_open())
    {
        ofs << value;
        return zpods::Status::OK;
    }
    else
    {
        return zpods::Status::ERROR;
    }
}

auto zpods::DbHandle::Get(const std::string& key, std::string* value)
    -> zpods::Status
{
    let key_path = zpods::fs::path(get_db_path()) / key;
    if (!zpods::fs::exists(key_path.c_str()))
    {
        return zpods::Status::NOT_FOUND;
    }

    std::ifstream ifs(key_path.c_str());

    if (ifs.is_open())
    {
        ifs >> *value;
        return zpods::Status::OK;
    }
    else
    {
        return zpods::Status::ERROR;
    }
}
