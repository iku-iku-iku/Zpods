//
// Created by code4love on 23-12-7.
//

#ifndef ZPODS_POD_SERVICE_CLIENT_H
#define ZPODS_POD_SERVICE_CLIENT_H

#include "pch.h"
#include "client_pch.h"

using zpods::Pod;
using zpods::UploadStatus;
using zpods::PodService;

class PodServiceClient {
public:
    explicit PodServiceClient(const std::shared_ptr<Channel>& channel) : stub_(PodService::NewStub(channel)) {}

    zpods::Status UploadPod(const std::string& pod_path);

private:
    std::unique_ptr<PodService::Stub> stub_;
};

#endif //ZPODS_POD_SERVICE_CLIENT_H
