//
// Created by code4love on 23-12-7.
//

#ifndef ZPODS_FILE_SERVICE_H
#define ZPODS_FILE_SERVICE_H

#include "server_pch.h"

using zpods::FileService;
using zpods::FileChunk;
using zpods::UploadStatus;


class FileServiceImpl final : public FileService::Service {
public:
    Status UploadFile(ServerContext* context, ServerReader<FileChunk>* reader, UploadStatus* response) override {
        std::optional<std::ofstream> out;
        FileChunk chunk;
        while (reader->Read(&chunk)) {
            if (!out) {
                out = std::ofstream(chunk.filename(), std::ios::binary);
            }
            out->write(chunk.content().data(), chunk.content().size());
        }
        out->close();
        response->set_success(true);
        return Status::OK;
    }
};

#endif //ZPODS_FILE_SERVICE_H
