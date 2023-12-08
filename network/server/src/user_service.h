//
// Created by code4love on 23-12-7.
//

#ifndef ZPODS_USER_SERVICE_H
#define ZPODS_USER_SERVICE_H

#include "pch.h"
#include "server_pch.h"

using zpods::UserService;
using zpods::RegisterRequest;
using zpods::RegisterResponse;
using zpods::LoginRequest;
using zpods::LoginResponse;


namespace {

    inline std::string sha256(const std::string &str) {
        unsigned char hash[EVP_MAX_MD_SIZE];
        unsigned int lengthOfHash = 0;

        EVP_MD_CTX *sha256 = EVP_MD_CTX_new();
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
}


class UserServiceImpl final : public UserService::Service {
    rocksdb::DB *db = nullptr;
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

    static std::string make_username_key(const std::string &username) {
        return "user:" + username;
    }

    grpc::Status Register(grpc::ServerContext *context, const RegisterRequest *request,
                          RegisterResponse *response) override {
        std::string username = make_username_key(request->username());
        std::string password = sha256(request->password());

        // Check if user already exists
        std::string value;
        rocksdb::Status s = db->Get(rocksdb::ReadOptions(), username, &value);
        if (s.ok()) {
            response->set_success(false);
            response->set_message("User already exists");
            return grpc::Status(grpc::StatusCode::ALREADY_EXISTS, "User already exists");
        }

        // Put in a database
        db->Put(rocksdb::WriteOptions(), username, password);

        response->set_success(true);
        response->set_message("User registered successfully");

        spdlog::info("A user registered: username: {}, password: {}", request->username(), request->password());
        return grpc::Status::OK;
    }

    grpc::Status Login(grpc::ServerContext *context, const LoginRequest *request,
                       LoginResponse *response) override {
        std::string username = make_username_key(request->username());
        std::string password = sha256(request->password());

        std::string stored_password;
        rocksdb::Status s = db->Get(rocksdb::ReadOptions(), username, &stored_password);
        if (s.ok() && stored_password == password) {
            response->set_success(true);
            response->set_message("Login successful");
            spdlog::info("A user loginned: username: {}, password: {}", request->username(), request->password());
        } else {
            response->set_success(false);
            response->set_message("Invalid username or password");
            return grpc::Status(grpc::StatusCode::UNAUTHENTICATED, "Invalid username or password");
        }
        return grpc::Status::OK;
    }
};

#endif //ZPODS_USER_SERVICE_H
