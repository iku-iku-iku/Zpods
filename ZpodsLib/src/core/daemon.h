#pragma once

#include <functional>
namespace zpods
{

using PodsQueryResult = std::vector<
    std::pair<std::string, std::vector<std::pair<std::string, long>>>>;
struct DaemonConfig
{
    std::function<Status(PodsQueryResult&)> query_pods;
    std::function<Status(const std::string& /*pod_path*/)> upload_pod;
    std::function<Status(const std::string& /*pods_name*/,
                         const std::string& /*pod_name*/,
                         const std::string& /*dir*/)>
        download_pod;
};

// the entry of the daemon
// the daemon will automatically sync the pods with remote
void zpods_daemon_entry(DaemonConfig config);
} // namespace zpods
