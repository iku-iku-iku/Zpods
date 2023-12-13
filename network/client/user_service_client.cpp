//
// Created by code4love on 23-12-7.
//

#include "grpc_client.h"
#include "user_service_client.h"

zpods::Status UserServiceClient::Register(const std::string& user,
                                          const std::string& password)
{
    zpods::RegisterRequest request;
    request.set_username(user);
    request.set_password(password);

    zpods::RegisterResponse response;
    grpc::ClientContext context;

    grpc::Status status = stub_->Register(&context, request, &response);

    if (!status.ok())
    {
        std::cout << "Register failed: " << status.error_code() << ": "
                  << status.error_message() << std::endl;
        return zpods::Status::ERROR;
    }

    return zpods::Status::OK;
}

zpods::Status UserServiceClient::Login(const std::string& username,
                                       const std::string& password)
{
    zpods::LoginRequest request;
    request.set_username(username);
    request.set_password(password);

    zpods::LoginResponse response;
    grpc::ClientContext context;

    grpc::Status status = stub_->Login(&context, request, &response);

    if (!status.ok())
    {
        std::cout << "Login failed: " << status.error_code() << ": "
                  << status.error_message() << std::endl;
        return zpods::Status::ERROR;
    }

    spdlog::info("Login success, token: {}", response.token());

    zpods::DbHandle::Instance().put_cached_username(username);
    zpods::DbHandle::Instance().put_cached_token(response.token());

    return zpods::Status::OK;
}
