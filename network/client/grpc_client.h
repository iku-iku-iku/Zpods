//
// Created by code4love on 23-12-7.
//

#ifndef ZPODS_GRPC_CLIENT_H
#define ZPODS_GRPC_CLIENT_H

#include "pch.h"
#include "pod_service_client.h"
#include "user_service_client.h"

inline auto get_db_path()
{
    return std::filesystem::path(getenv("HOME")) / ".ZPODS/client_db";
}

inline bool is_server_addr_set;

inline std::string& get_server_addr()
{
    static std::string s;
    return s;
}

inline void set_server_addr(const std::string& s)
{
    is_server_addr_set = true;
    get_server_addr() = s;
}

namespace zpods
{
class DbHandle
{
  public:
    static DbHandle& Instance();

    auto Put(const std::string& key, const std::string& value) -> zpods::Status;

    auto Get(const std::string& key, std::string* value) -> zpods::Status;

    auto put_cached_username(const std::string& username) -> zpods::Status
    {
        return Put("cache_username", username);
    }

    auto get_cached_username(std::string* username) -> zpods::Status
    {
        return Get("cache_username", username);
    }

    auto put_cached_token(const std::string& token) -> zpods::Status
    {
        return Put("cache_token", token);
    }

    auto get_cached_token(std::string* token) -> zpods::Status
    {
        return Get("cache_token", token);
    }

    auto put_cached_addr(const std::string& addr) -> zpods::Status
    {
        return Put("cache_addr", addr);
    }

    auto get_cached_addr(std::string* addr) -> zpods::Status
    {
        return Get("cache_addr", addr);
    }

    DbHandle(const DbHandle&) = delete;
    DbHandle& operator=(const DbHandle&) = delete;

  private:
    DbHandle();
};

struct RpcUser
{
    std::string username;
    std::string password;

    RpcUser();
    PodServiceClient client_;

    zpods::Status register_() const;

    zpods::Status login() const;

    //        Status unregister() const;
    //
    Status upload_pod(const std::string& pod_path);
    Status upload_pods(const std::string& pods_dir);
    Status query_pods(PodsQueryResult& result);
    Status download_pod(const std::string& pods_name,
                        const std::string& pod_name, const std::string& dir);
    //
    //        Status download_pods(const char *remote_pods_dir, const char
    //        *local_pods_dir);
};
} // namespace zpods

PodServiceClient get_pod_service_client();

UserServiceClient get_user_service_client();

#endif // ZPODS_GRPC_CLIENT_H
