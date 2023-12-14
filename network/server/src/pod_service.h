//
// Created by code4love on 23-12-7.
//

#ifndef ZPODS_POD_SERVICE_H
#define ZPODS_POD_SERVICE_H

#include "ZpodsLib/src/base/fs.h"
#include "server_pch.h"
#include "zpods.pb.h"

class PodServiceImpl final : public zpods::PodService::Service
{
  public:
    Status UploadPod(ServerContext* context,
                     ServerReader<zpods::UploadPodRequest>* reader,
                     zpods::UploadStatus* response) override
    {
        std::ofstream out;
        std::string username;
        zpods::UploadPodRequest pod;

        bool first_read = true;
        while (reader->Read(&pod))
        {
            if (first_read)
            {
                first_read = false;
                // check whether the token is valid
                if (!is_token_valid(pod.token()))
                {
                    return Status::CANCELLED;
                }

                // get username by token
                zpods::DbHandle::Instance().get_username_by_token(pod.token(),
                                                                  &username);
                spdlog::info("user {} uploading {}/{}", username,
                             pod.pods_name(), pod.pod_name());

                // get ofstream for writing file
                let pod_path = std::filesystem::path(
                    fmt::format("{}/{}/{}", username.c_str(), pod.pods_name(),
                                pod.pod_name()));
                spdlog::info("the path: {}", pod_path.c_str());
                if (!std::filesystem::exists(pod_path.parent_path()))
                {
                    std::filesystem::create_directories(pod_path.parent_path());
                    spdlog::info("create directory: {}",
                                 pod_path.parent_path().c_str());
                }
                out = std::ofstream(pod_path, std::ios::binary);
            }

            out.write(pod.content().data(), pod.content().size());
        }
        out.close();
        response->set_success(true);
        return Status::OK;
    }

    Status QueryPods(ServerContext* context,
                     const zpods::QueryPodsRequest* request,
                     zpods::QueryPodsResponse* response) override
    {
        std::string username;
        {
            let status = zpods::DbHandle::Instance().get_username_by_token(
                request->token(), &username);
            if (status != rocksdb::Status::OK())
            {
                return Status::CANCELLED;
            }
        }

        let pods_path = zpods::fs::path(username);
        if (!std::filesystem::exists(pods_path))
        {
            // empty
            return Status::OK;
        }

        for (const auto& pods_entry :
             std::filesystem::directory_iterator(pods_path))
        {
            let pods_path = pods_entry.path();
            let pods_name = pods_path.filename().string();
            response->add_pods_name(pods_name);

            let pod_list = response->add_pod_list();

            for (const auto& entry :
                 std::filesystem::directory_iterator(pods_path))
            {
                let pod_name = entry.path().filename().string();
                spdlog::info("querying {}/{}", pods_name, pod_name);
                pod_list->add_pod_name(pod_name);
                let ts =
                    zpods::fs::last_modified_timestamp(entry.path().c_str());
                pod_list->add_last_modified_time(ts);
            }
        }
        return Status::OK;
    }

    Status
    DownloadPod(ServerContext* context,
                const zpods::DownloadPodRequest* request,
                grpc::ServerWriter<zpods::DownloadPodResponse>* writer) override
    {
        std::string username;
        {
            let status = zpods::DbHandle::Instance().get_username_by_token(
                request->token(), &username);
            if (status != rocksdb::Status::OK())
            {
                return Status::CANCELLED;
            }
        }

        let pod_path = zpods::fs::path(fmt::format("{}/{}/{}", username.c_str(),
                                                   request->pods_name(),
                                                   request->pod_name()));
        if (!std::filesystem::exists(pod_path))
        {
            spdlog::info("{} is trying to download the wrong path: {}",
                         username, pod_path.c_str());
            return Status::CANCELLED;
        }
        else
        {
            spdlog::info("{} is downloading the path: {}", username,
                         pod_path.c_str());
        }
        std::ifstream in(pod_path, std::ios::binary);
        std::string content;
        while (true)
        {
            content.resize(1024 * 1024);
            in.read(content.data(), content.size());
            content.resize(in.gcount());
            if (content.empty())
            {
                break;
            }
            zpods::DownloadPodResponse response;
            response.set_content(content);
            writer->Write(response);
        }
        return Status::OK;
    }
};

#endif // ZPODS_POD_SERVICE_H
