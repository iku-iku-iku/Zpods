//
// Created by code4love on 23-12-7.
//

#ifndef ZPODS_USER_SERVICE_CLIENT_H
#define ZPODS_USER_SERVICE_CLIENT_H

#include "client_pch.h"

class UserServiceClient {
public:
    explicit UserServiceClient(const std::shared_ptr<Channel>& channel) : stub_(zpods::UserService::NewStub(channel)) {}

    void Register(const std::string& user, const std::string& password) {
        zpods::RegisterRequest request;
        request.set_username(user);
        request.set_password(password);

        zpods::RegisterResponse response;
        grpc::ClientContext context;

        grpc::Status status = stub_->Register(&context, request, &response);

        if (status.ok()) {
            std::cout << "Register status: " << response.message() << std::endl;
        } else {
            std::cout << "Register failed: " << status.error_code() << ": " << status.error_message() << std::endl;
        }
    }

    void Login(const std::string& user, const std::string& password) {
        zpods::LoginRequest request;
        request.set_username(user);
        request.set_password(password);

        zpods::LoginResponse response;
        grpc::ClientContext context;

        grpc::Status status = stub_->Login(&context, request, &response);

        if (status.ok()) {
            std::cout << "Login status: " << response.message() << std::endl;
        } else {
            std::cout << "Login failed: " << status.error_code() << ": " << status.error_message() << std::endl;
        }
    }

private:
    std::shared_ptr<zpods::UserService::Stub> stub_;
};

#endif //ZPODS_USER_SERVICE_CLIENT_H
