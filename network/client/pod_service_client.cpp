//
// Created by code4love on 23-12-7.
//

#include "grpc_client.h"
#include "pod_service_client.h"

zpods::Status PodServiceClient::UploadPod(const std::string& pod_path)
{
    std::ifstream file(pod_path, std::ios::binary);
    ClientContext context;
    UploadStatus upload_status;
    std::unique_ptr<ClientWriter<Pod>> writer(
        stub_->UploadPod(&context, &upload_status));

    Pod pod;
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
