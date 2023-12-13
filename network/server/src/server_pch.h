//
// Created by code4love on 23-12-7.
//

#ifndef ZPODS_SERVER_PCH_H
#define ZPODS_SERVER_PCH_H

#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>
#include <optional>
#include <string>
#include "pch.h"
#include <openssl/evp.h>
#include <rocksdb/db.h>

#include "absl/flags/flag.h"
#include "absl/flags/parse.h"
#include "absl/strings/str_format.h"

#include "zpods.grpc.pb.h"
#include <grpcpp/ext/proto_server_reflection_plugin.h>
#include <grpcpp/grpcpp.h>
#include <grpcpp/health_check_service_interface.h>
#include <grpcpp/security/credentials.h>

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::ServerReader;
using grpc::Status;

const char* TTL = "ttl:";

namespace zpods
{

class DbHandle
{
  public:
    DbHandle()
    {
        rocksdb::Options options;
        options.create_if_missing = true;
        rocksdb::Status status =
            rocksdb::DB::Open(options, "zpods_server_db", &db);
        assert(status.ok());
    }

    static DbHandle& Instance()
    {
        static DbHandle db_handle;
        return db_handle;
    }

    auto Put(const std::string& key, const std::string& value)
    {
        ZPODS_ASSERT(db != nullptr);
        rocksdb::Status status = db->Put(rocksdb::WriteOptions(), key, value);
        return status;
    }

    auto Get(const std::string& key, std::string* value)
    {
        ZPODS_ASSERT(db != nullptr);
        rocksdb::Status status = db->Get(rocksdb::ReadOptions(), key, value);
        return status;
    }

    static std::string username_key(const std::string& username)
    {
        return "username:" + username;
    }

    auto put_username2password_hash(const std::string& username,
                                    const std::string& password_hash)
    {
        return Put(username_key(username), password_hash);
    }

    auto get_password_hash_by_username(const std::string& username,
                                       std::string* password_hash)
    {
        return Get(username_key(username), password_hash);
    }

    static std::string token2user(const std::string& key)
    {
        return "token2user:" + key;
    }

    auto put_token2user(const std::string& key, const std::string& value)
    {
        return Put(token2user(key), value);
    }

    auto get_user_by_token(const std::string& key, std::string* value)
    {
        return Get(token2user(key), value);
    }

    ~DbHandle() { delete db; }

    DbHandle(const DbHandle&) = delete;
    DbHandle& operator=(const DbHandle&) = delete;

  private:
    rocksdb::DB* db;
};
} // namespace zpods

// put_token(db, token, 60 * 60 * 24 * 7);
inline void put_token(const std::string& token, int ttl = 60 * 60 * 24 * 7)
{
    let now = std::chrono::system_clock::now();
    let expiry =
        std::chrono::system_clock::to_time_t(now + std::chrono::seconds(ttl));

    let expiry_value = std::to_string(expiry);
    zpods::DbHandle::Instance().Put(TTL + token, expiry_value);
}

inline bool is_token_valid(const std::string& token)
{
    let now = std::chrono::system_clock::now();
    let now_value = std::chrono::system_clock::to_time_t(now);

    std::string expiry_value;
    rocksdb::Status s =
        zpods::DbHandle::Instance().Get(TTL + token, &expiry_value);
    if (!s.ok())
    {
        return false;
    }

    let expiry = std::stoi(expiry_value);
    return now_value < expiry;
}

#endif // ZPODS_PCH_H
