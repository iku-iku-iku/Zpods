//
// Created by code4love on 23-12-7.
//

#ifndef ZPODS_GRPC_CLIENT_H
#define ZPODS_GRPC_CLIENT_H

#include "pch.h"
#include "pod_service_client.h"
#include "user_service_client.h"
#include <rocksdb/db.h>

namespace zpods
{
class DbHandle
{
  public:
    static DbHandle& Instance();

    ~DbHandle() { delete db; }

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

    DbHandle(const DbHandle&) = delete;
    DbHandle& operator=(const DbHandle&) = delete;

  private:
    rocksdb::DB* db = nullptr;
    DbHandle();
};

struct RpcUser
{
    std::string username;
    std::string password;

    RpcUser();
    PodServiceClient client_;

    //        auto to_json_str() const -> std::string;

    //        auto request(const char *path) const;

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
  private:
    rocksdb::DB* db;
};
} // namespace zpods

PodServiceClient get_pod_service_client();

UserServiceClient get_user_service_client();

#endif // ZPODS_GRPC_CLIENT_H
