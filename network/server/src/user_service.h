//
// Created by code4love on 23-12-7.
//

#ifndef ZPODS_USER_SERVICE_H
#define ZPODS_USER_SERVICE_H

#include "pch.h"
#include "server_db.h"
#include "zpods.grpc.pb.h"
#include "zpods.pb.h"
#include <openssl/evp.h>

namespace
{

std::string generate_token(int length = 16)
{
    static const char alphanum[] = "0123456789"
                                   "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                   "abcdefghijklmnopqrstuvwxyz";

    std::string token;
    std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<> dist(0, sizeof(alphanum) - 2);

    for (int i = 0; i < length; ++i)
    {
        token += alphanum[dist(rng)];
    }

    return token;
}

inline std::string sha256(const std::string& str)
{
    unsigned char hash[EVP_MAX_MD_SIZE];
    unsigned int lengthOfHash = 0;

    EVP_MD_CTX* sha256 = EVP_MD_CTX_new();
    if (!sha256)
    {
        // 处理错误
    }

    if (1 != EVP_DigestInit_ex(sha256, EVP_sha256(), nullptr))
    {
        // 处理错误
    }

    if (1 != EVP_DigestUpdate(sha256, str.c_str(), str.size()))
    {
        // 处理错误
    }

    if (1 != EVP_DigestFinal_ex(sha256, hash, &lengthOfHash))
    {
        // 处理错误
    }

    EVP_MD_CTX_free(sha256);

    std::stringstream ss;
    for (unsigned int i = 0; i < lengthOfHash; ++i)
    {
        ss << std::hex << std::setw(2) << std::setfill('0')
           << static_cast<int>(hash[i]);
    }

    return ss.str();
}
} // namespace

class UserServiceImpl final : public zpods::UserService::Service
{
  public:
    grpc::Status Register(grpc::ServerContext* context,
                          const zpods::RegisterRequest* request,
                          zpods::RegisterResponse* response) override
    {
        std::string username = request->username();
        std::string password_hash = sha256(request->password());

        // Check if user already exists
        std::string value;
        let s = zpods::DbHandle::Instance().get_password_hash_by_username(
            username, &value);
        if (s.ok())
        {
            response->set_success(false);
            response->set_message("User already exists");
            return grpc::Status(grpc::StatusCode::ALREADY_EXISTS,
                                "User already exists");
        }

        // Put in a database
        zpods::DbHandle::Instance().put_username2password_hash(username,
                                                               password_hash);

        response->set_success(true);
        response->set_message("User registered successfully");

        spdlog::info("A user registered: username: {}, password_hash: {}",
                     username, password_hash);
        return grpc::Status::OK;
    }

    grpc::Status Login(grpc::ServerContext* context,
                       const zpods::LoginRequest* request,
                       zpods::LoginResponse* response) override
    {
        std::string username = request->username();
        std::string password_hash = sha256(request->password());

        std::string stored_password_hash;
        let s = zpods::DbHandle::Instance().get_password_hash_by_username(
            username, &stored_password_hash);
        if (s.ok() && stored_password_hash == password_hash)
        {
            let token = generate_token();
            zpods::DbHandle::Instance().put_token2username(token, username);
            put_token(token);

            response->set_success(true);
            response->set_message("Login successful");
            response->set_token(token);
            spdlog::info(
                "A user loginned: username: {}, password_hash: {}, token: {}",
                username, password_hash, token);
        }
        else
        {
            response->set_success(false);
            response->set_message("Invalid username or password_hash");
            return grpc::Status(grpc::StatusCode::UNAUTHENTICATED,
                                "Invalid username or password_hash");
        }
        return grpc::Status::OK;
    }
};

#endif // ZPODS_USER_SERVICE_H
