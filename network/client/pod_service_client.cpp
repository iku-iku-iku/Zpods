//
// Created by code4love on 23-12-7.
//

#include "ZpodsLib/src/base/fs.h"
#include "grpc_client.h"
#include "pch.h"
#include "pod_service_client.h"
#include "zpods.pb.h"
#include <grpcpp/client_context.h>

namespace
{
zpods::Status get_cached_token(std::string* token)
{
    let status = zpods::DbHandle::Instance().get_cached_token(token);
    if (status != zpods::Status::OK)
    {
        spdlog::info("fail to get cached token");
        return status;
    }
    return zpods::Status::OK;
}
} // namespace

zpods::Status PodServiceClient::UploadPod(const std::string& pod_path)
{
    std::ifstream file(pod_path, std::ios::binary);
    ClientContext context;
    zpods::UploadStatus upload_status;
    std::unique_ptr<ClientWriter<zpods::UploadPodRequest>> writer(
        stub_->UploadPod(&context, &upload_status));

    zpods::UploadPodRequest request;
    let pod_name = std::filesystem::path(pod_path).filename().string();
    let pods_name =
        std::filesystem::path(pod_path).parent_path().filename().string();
    spdlog::info("uploading {}/{}", pods_name, pod_name);
    request.set_pod_name(pod_name);
    request.set_pods_name(pods_name);

    CHECK_STATUS(get_cached_token(request.mutable_token()));

    char buffer[4096]; // Buffer size can be adjusted
    while (file.read(buffer, sizeof(buffer)))
    {
        request.set_content(buffer, sizeof(buffer));
        if (!writer->Write(request))
        {
            // Broken stream
            break;
        }
    }
    // Handle the last piece of the file
    if (file.gcount() > 0)
    {
        request.set_content(buffer, file.gcount());
        writer->Write(request);
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
    ClientContext context;
    zpods::QueryPodsRequest request;
    zpods::QueryPodsResponse response;

    CHECK_STATUS(get_cached_token(request.mutable_token()));

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

zpods::Status PodServiceClient::DownloadPod(const std::string& pods_name,
                                            const std::string& pod_name,
                                            const std::string& dir)
{
    ClientContext context;
    zpods::DownloadPodRequest request;

    CHECK_STATUS(get_cached_token(request.mutable_token()));
    request.set_pods_name(pods_name);
    request.set_pod_name(pod_name);

    // note that the response is a stream
    std::unique_ptr<grpc::ClientReader<zpods::DownloadPodResponse>> reader(
        stub_->DownloadPod(&context, request));

    // read from stream and write to file
    let dir_path = zpods::fs::path(dir);
    let pods_path = dir_path / pods_name;
    let pod_path = pods_path / pod_name;
    zpods::fs::create_directory_if_not_exist(dir_path.c_str());
    zpods::fs::create_directory_if_not_exist(pods_path.c_str());
    std::ofstream out(pod_path, std::ios::binary);
    zpods::DownloadPodResponse response;
    while (reader->Read(&response))
    {
        out.write(response.content().data(), response.content().size());
    }

    Status status = reader->Finish();
    if (!status.ok())
    {
        spdlog::info("download pod {} fail: {}", pod_name,
                     (int)status.error_code());
        return zpods::Status::ERROR;
    }
    spdlog::info("download pod {} succeeded!", pod_name);
    return zpods::Status::OK;
}
