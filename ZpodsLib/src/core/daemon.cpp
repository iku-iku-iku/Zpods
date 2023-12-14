#include <chrono>
#include <thread>
#include "backup.h"
#include "config.h"
#include "daemon.h"
#include "fs.h"
#include "manager.h"

using namespace zpods;
namespace
{
class PodsSynchronizer
{
  public:
    PodsSynchronizer(const DaemonConfig& daemon_config, std::string pods_dir)
        : daemon_config_(daemon_config), pods_dir_(std::move(pods_dir))
    {
        pods_name_ = fs::path(pods_dir_).filename();
    }

    Status local_remote_sync() const
    {
        PodsQueryResult result;
        CHECK_STATUS(daemon_config_.query_pods(result));

        std::unordered_set<std::string> local_pods;
        for (let_ref entry : fs::directory_iterator(pods_dir_))
        {
            local_pods.insert(entry.path().filename().string());
        }

        std::unordered_set<std::string> local_new_pods;
        std::unordered_set<std::string> remote_new_pods;
        std::unordered_set<std::string> remote_pods;

        for (let_ref[k, v] : result)
        {
            if (k == pods_name_)
            {
                for (let_ref[pod_name, last_modified_ts] : v)
                {
                    remote_pods.insert(pod_name);
                    if (!local_pods.contains(pod_name))
                    {
                        remote_new_pods.insert(pod_name);
                    }
                }
                break;
            }
        }

        for (let_ref local_pod : local_pods)
        {
            if (!remote_pods.contains(local_pod))
            {
                local_new_pods.insert(local_pod);
            }
        }

        // push local new pods
        for (let_ref local_new_pod : local_new_pods)
        {
            CHECK_STATUS(
                daemon_config_.upload_pod(fs::path(pods_dir_) / local_new_pod));
            spdlog::info("push {}", local_new_pod);
        }

        // pull remote new pods
        has_pull_ = false;
        for (let_ref remote_new_pod : remote_new_pods)
        {
            CHECK_STATUS(daemon_config_.download_pod(pods_name_, remote_new_pod,
                                                     pods_dir_));
            spdlog::info("pull {}", remote_new_pod);
            has_pull_ = true;
        }

        return Status::OK;
    }

    bool has_pull() const { return has_pull_; }

  private:
    const DaemonConfig& daemon_config_;
    std::string pods_dir_;
    std::string pods_name_;
    mutable bool has_pull_ = false;
};
void thread_for_pods(std::string tree_dir, std::string pods_dir,
                     const DaemonConfig& daemon_config)
{
    constexpr int interval = 5000; // 5s
    constexpr int offset = 1000;   // add some random offset

    let dest_dir = fs::path(pods_dir).parent_path();

    BackupConfig config;
    config.tree_dir = tree_dir;

    let synchronizer = PodsSynchronizer(daemon_config, pods_dir);
    while (true)
    {
        { // sample from [interval - offset, interval + offset]
            let_mut sleep_time = interval + rand() % (offset * 2) - offset;
            std::this_thread::sleep_for(std::chrono::milliseconds(sleep_time));
        }

        spdlog::info("start sync for {}...", pods_dir);

        { // backup
            let status = backup(dest_dir.c_str(), config);
            if (status != Status::OK)
            {
                spdlog::error("backup failed : {}", (int)status);
            }
        }

        { // synchronize local and remote
            let status = synchronizer.local_remote_sync();
            if (status != Status::OK)
            {
                spdlog::error("local_remote_sync failed : {}", (int)status);
            }
        }

        if (synchronizer.has_pull())
        { // synchronize locally
            let status = restore(pods_dir.c_str(), tree_dir.c_str(), config);
            if (status != Status::OK)
            {
                spdlog::error("restore failed : {}", (int)status);
            }
            else
            {
                spdlog::info("synchronize successfully");
            }
        }
    }
}
void init_daemon(const DaemonConfig& config)
{
    let manager = PodsManager::Instance();
    manager->load_pods_mapping();
    // create one thread for each path
    for (const auto& [tree_dir, pods_dir] : manager->get_path_mapping())
    {
        std::thread(thread_for_pods, tree_dir, pods_dir, config).detach();
    }
}
} // namespace

void zpods::zpods_daemon_entry(DaemonConfig config)
{
    init_daemon(config);
    while (true)
    {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}
