//
// Created by code4love on 23-12-7.
//

#ifndef ZPODS_USER_SERVICE_CLIENT_H
#define ZPODS_USER_SERVICE_CLIENT_H

#include "pch.h"
#include "client_pch.h"

class UserServiceClient {
public:
    explicit UserServiceClient(const std::shared_ptr<Channel>& channel) : stub_(zpods::UserService::NewStub(channel)) {}

    zpods::Status Register(const std::string& user, const std::string& password);

    zpods::Status Login(const std::string& username, const std::string& password);

private:
    std::shared_ptr<zpods::UserService::Stub> stub_;
};

#endif //ZPODS_USER_SERVICE_CLIENT_H
