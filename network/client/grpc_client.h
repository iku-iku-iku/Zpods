//
// Created by code4love on 23-12-7.
//

#ifndef ZPODS_GRPC_CLIENT_H
#define ZPODS_GRPC_CLIENT_H

#include "file_service_client.h"
#include "user_service_client.h"

FileServiceClient get_file_service_client();
UserServiceClient get_user_service_client();

#endif //ZPODS_GRPC_CLIENT_H
