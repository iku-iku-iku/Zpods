//
// Created by code4love on 23-12-7.
//

#ifndef ZPODS_FILE_SERVICE_CLIENT_H
#define ZPODS_FILE_SERVICE_CLIENT_H

#include "client_pch.h"

using zpods::FileChunk;
using zpods::UploadStatus;
using zpods::FileService;

class FileServiceClient {
public:
    explicit FileServiceClient(const std::shared_ptr<Channel>& channel) : stub_(FileService::NewStub(channel)) {}

    bool UploadFile(const std::string& file_name);

private:
    std::unique_ptr<FileService::Stub> stub_;
};

#endif //ZPODS_FILE_SERVICE_CLIENT_H
