//
// Created by code4love on 23-12-7.
//

#ifndef ZPODS_POD_SERVICE_H
#define ZPODS_POD_SERVICE_H

#include "server_pch.h"

using zpods::PodService;
using zpods::Pod;
using zpods::UploadStatus;


class PodServiceImpl final : public PodService::Service {
public:
    Status UploadPod(ServerContext *context, ServerReader<Pod> *reader, UploadStatus *response) override {
        std::optional<std::ofstream> out;
        std::optional<std::string> username;
        Pod pod;

        bool first_read = true;
        while (reader->Read(&pod)) {
            if (first_read) {
                first_read = false;
                if (!is_token_valid(pod.token())) {
                    return Status::CANCELLED;
                }
            }
            if (!username) {
                zpods::DbHandle::Instance().get_user_by_token(pod.token(), &*username);

                spdlog::info("user {} uploading {}/{}", *username, pod.pods_name(), pod.pod_name());
            }
            if (!out) {
                let pod_path = std::filesystem::path(fmt::format("{}/{}/{}", username->c_str(), pod.pods_name(), pod.pod_name()));
                spdlog::info("the path: {}", pod_path.c_str());
                if (!std::filesystem::exists(pod_path.parent_path())) {
                    std::filesystem::create_directories(pod_path.parent_path());
                    spdlog::info("create directory: {}", pod_path.parent_path().c_str());
                }
                out = std::ofstream(pod_path, std::ios::binary);
            }
            out->write(pod.content().data(), pod.content().size());
        }
        out->close();
        response->set_success(true);
        return Status::OK;
    }
};

#endif //ZPODS_POD_SERVICE_H
