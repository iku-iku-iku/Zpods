#pragma once

#include "pch.h"
#include <rocksdb/db.h>

#define TTL "ttl:"
#define MAKE_STORE_PATH(x) (get_zpods_storage_path() / x)

inline auto get_zpods_storage_path()
{
    return std::filesystem::path("server_storage");
}

namespace zpods
{

class DbHandle
{
  public:
    DbHandle();

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

    static std::string token2username(const std::string& key)
    {
        return "token2username:" + key;
    }

    auto put_token2username(const std::string& key, const std::string& username)
    {
        return Put(token2username(key), username);
    }

    auto get_username_by_token(const std::string& key, std::string* username)
    {
        return Get(token2username(key), username);
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
