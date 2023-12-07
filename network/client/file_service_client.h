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

    bool UploadFile(const std::string& file_name) {
        std::ifstream file(file_name, std::ios::binary);
        ClientContext context;
        UploadStatus status;
        std::unique_ptr<ClientWriter<FileChunk>> writer(stub_->UploadFile(&context, &status));

        FileChunk chunk;
        chunk.set_filename(file_name);
        char buffer[4096];  // Buffer size can be adjusted
        while (file.read(buffer, sizeof(buffer))) {
            chunk.set_content(buffer, sizeof(buffer));
            if (!writer->Write(chunk)) {
                // Broken stream
                break;
            }
        }
        // Handle the last piece of the file
        if (file.gcount() > 0) {
            chunk.set_content(buffer, file.gcount());
            writer->Write(chunk);
        }
        file.close();
        writer->WritesDone();
        Status finish_status = writer->Finish();
        return finish_status.ok() && status.success();
    }

private:
    std::unique_ptr<FileService::Stub> stub_;
};


#endif //ZPODS_FILE_SERVICE_CLIENT_H
