//
// Created by code4love on 23-12-7.
//

#ifndef ZPODS_POD_SERVICE_CLIENT_H
#define ZPODS_POD_SERVICE_CLIENT_H

#include "client_pch.h"

using PodsQueryResult = std::vector<
    std::pair<std::string, std::vector<std::pair<std::string, long>>>>;

class PodServiceClient
{
  public:
    explicit PodServiceClient(const std::shared_ptr<Channel>& channel)
        : stub_(zpods::PodService::NewStub(channel))
    {
    }

    /// UploadPod uploads a pod to the server.
    /// \param pod_path the path of the pod to be uploaded
    /// \return the status of the upload
    /// \note the pod_path must be a directory
    zpods::Status UploadPod(const std::string& pod_path);

    /// QueryPods queries the pods on the server.
    /// \param result the result of the query
    /// \return the status of the query
    zpods::Status QueryPods(PodsQueryResult& result);

    /// DownloadPod downloads a pod from the server.
    /// \param pods_name the name of the pods
    /// \param pod_name the name of the pod
    /// \return the status of the download
    zpods::Status DownloadPod(const std::string& pods_name,
                              const std::string& pod_name,
                              const std::string& dir);

  private:
    std::unique_ptr<zpods::PodService::Stub> stub_;
};

#endif // ZPODS_POD_SERVICE_CLIENT_H
