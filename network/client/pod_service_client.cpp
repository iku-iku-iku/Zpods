//
// Created by code4love on 23-12-7.
//

#include "grpc_client.h"
#include "pch.h"
#include "pod_service_client.h"

zpods::Status PodServiceClient::UploadPod(const std::string& pod_path)
{
    std::ifstream file(pod_path, std::ios::binary);
    ClientContext context;
    UploadStatus upload_status;
    std::unique_ptr<ClientWriter<UploadPodRequest>> writer(
        stub_->UploadPod(&context, &upload_status));

    UploadPodRequest pod;
    let pod_name = std::filesystem::path(pod_path).filename().string();
    let pods_name =
        std::filesystem::path(pod_path).parent_path().filename().string();
    spdlog::info("uploading {}/{}", pods_name, pod_name);
    pod.set_pod_name(pod_name);
    pod.set_pods_name(pods_name);

    {
        let status =
            zpods::DbHandle::Instance().get_cached_token(pod.mutable_token());
        if (status != zpods::Status::OK)
        {
            spdlog::info("fail to get cached token");
            return status;
        }
    }

    char buffer[4096]; // Buffer size can be adjusted
    while (file.read(buffer, sizeof(buffer)))
    {
        pod.set_content(buffer, sizeof(buffer));
        if (!writer->Write(pod))
        {
            // Broken stream
            break;
        }
    }
    // Handle the last piece of the file
    if (file.gcount() > 0)
    {
        pod.set_content(buffer, file.gcount());
        writer->Write(pod);
    }
    file.close();
    writer->WritesDone();
    Status finish_status = writer->Finish();
    if (finish_status.ok() && upload_status.success())
    {
        return zpods::Status::OK;
    }

    spdlog::info("fail to upload pod: {}", (int)finish_status.error_code());

    return zpods::Status::ERROR;
}

zpods::Status PodServiceClient::QueryPods(PodsQueryResult& result)
{
    std::string token;
    {

        let status = zpods::DbHandle::Instance().get_cached_token(&token);
        if (status != zpods::Status::OK)
        {
            spdlog::info("fail to get cached token");
            return status;
        }
    }

    ClientContext context;
    QueryPodsRequest request;
    QueryPodsResponse response;

    request.set_token(token);

    {
        let status = stub_->QueryPods(&context, request, &response);

        if (!status.ok())
        {
            spdlog::info("fail to query pods: {}", (int)status.error_code());
            return zpods::Status::ERROR;
        }
    }

    for (int i = 0; i < response.pods_name_size(); i++)
    {

        let_ref pod_name = response.pods_name(i);
        let_ref pods = response.pod_list(i);

        std::vector<std::pair<std::string, long>> pod_list;
        for (int j = 0; j < pods.pod_name_size(); j++)
        {
            let_ref pod_name = pods.pod_name(j);
            let_ref pod_last_modified_timestamp = pods.last_modified_time(j);
            pod_list.emplace_back(pod_name, pod_last_modified_timestamp);
        }

        result.emplace_back(pod_name, std::move(pod_list));
    }
    return zpods::Status::OK;
}
