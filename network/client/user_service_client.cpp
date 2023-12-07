//
// Created by code4love on 23-12-7.
//

#include "user_service_client.h"

void UserServiceClient::Register(const std::string &user, const std::string &password) {
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

void UserServiceClient::Login(const std::string &user, const std::string &password) {
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
