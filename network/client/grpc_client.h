//
// Created by code4love on 23-12-7.
//

#ifndef ZPODS_GRPC_CLIENT_H
#define ZPODS_GRPC_CLIENT_H

#include "pch.h"
#include "file_service_client.h"
#include "user_service_client.h"

namespace zpods {
    struct RpcUser {
        std::string username;
        std::string password;

//        auto to_json_str() const -> std::string;

//        auto request(const char *path) const;

        zpods::Status register_() const;

        zpods::Status login() const;

//        Status unregister() const;
//
//        Status upload_pods(const char *pods_dir);
//
//        Status download_pods(const char *remote_pods_dir, const char *local_pods_dir);
    };
}

FileServiceClient get_file_service_client();
UserServiceClient get_user_service_client();

#endif //ZPODS_GRPC_CLIENT_H
