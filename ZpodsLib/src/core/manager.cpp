//
// Created by code4love on 23-11-15.
//
#include "ZpodsLib/src/core/zpods_core.h"
#include "manager.h"

using namespace zpods;

PodsManager* PodsManager::Instance()
{
    static PodsManager instance;
    return &instance;
}

Status PodsManager::load_pods(const fs::zpath& pods_path,
                              const BackupConfig& config)
{
    //    let_mut paths = fs::get_file_family(pods_path.c_str());
    ZPODS_ASSERT(fs::is_directory(pods_path));
    if (!cur_state_of_peas_per_pods.contains(pods_path))
    {
        cur_state_of_peas_per_pods.insert({pods_path, {}});
    }

    std::vector<fs::zpath> pod_paths;
    for (let_ref pod : fs::directory_iterator(pods_path))
    {
        let_ref pod_path = pod.path();
        if (pod_path.string().ends_with(POD_FILE_SUFFIX))
        {
            pod_paths.push_back(pod_path);
        }
    }

    // sort by timestamp
    std::sort(pod_paths.begin(), pod_paths.end(),
              [](const auto& lhs, const auto& rhs) {
                  let_mut ss1 = std::stringstream(lhs.filename().c_str());
                  let_mut ss2 = std::stringstream(rhs.filename().c_str());
                  long ts1, ts2;
                  ss1 >> ts1;
                  ss2 >> ts2;
                  return ts1 < ts2;
              });

    // filter pods newer than ts
    if (config.timestamp != -1)
    {
        erase_if(pod_paths, [&](const auto& pod_path) {
            let_mut ss = std::stringstream(pod_path.filename().c_str());
            long ts;
            ss >> ts;
            return ts > config.timestamp;
        });
    }

    let_mut_ref cur_pod = current_pod(pods_path);
    cur_pod.clear();

    std::unordered_map<std::string, decltype(cur_pod.begin())> pea_map;
    for (let_ref pod_path : pod_paths)
    {
        let status = process_origin_zpods_bytes(
            pod_path.c_str(), const_cast<BackupConfig&>(config),
            [&](auto& bytes) {
                return foreach_pea_in_pod_bytes(
                    (byte*)bytes.c_str(), [&](const PeaHeader& pea_header) {
                        if (pea_header.is_delete())
                        {
                            let path = pea_header.get_path();

                            // we must delete some existed pea
                            ZPODS_ASSERT(pea_map.contains(path));
                            cur_pod.erase(pea_map[path]);
                        }
                        else
                        {
                            // retain the newest pea
                            if (pea_map.contains(pea_header.get_path()))
                            {
                                cur_pod.erase(pea_map[pea_header.get_path()]);
                            }
                            let_mut pea = Pea{
                                .last_modified_ts =
                                    pea_header.get_last_modified_ts(),
                                .rel_path = pea_header.get_path(),
                                .resident_pod_path = pod_path,
                            };
                            let res = cur_pod.insert(std::move(pea));
                            pea_map.insert({res.first->rel_path, res.first});
                        }
                        return Status::OK;
                    });
            });
        if (status != Status::OK)
        {
            return status;
        }
    }

    return Status::OK;
}

void PodsManager::load_pods_path()
{
    if (fs::exists("./pods_paths"))
    {
        let_mut ifs = std::ifstream("./pods_paths");
        std::string line1;
        std::string line2;
        while (std::getline(ifs, line1) && std::getline(ifs, line2))
        {
            path_mapping_[line1] = line2;
        }
    }
}

void PodsManager::store_pods_path()
{
    let_mut ofs =
        fs::open_or_create_file_as_ofs("./pods_paths", fs::ios::binary);
    for (const auto& [k, v] : path_mapping_)
    {
        ofs << k << '\n';
        ofs << v << '\n';
    }
}

void PodsManager::record_mapping(const fs::zpath& src_path,
                                 const fs::zpath& dst_path)
{
    path_mapping_[src_path] = dst_path;
    store_pods_path();
}

void PodsManager::load_pods_from_tracked_paths()
{
    load_pods_path();

    for (const auto& [k, v] : path_mapping_)
    {
        for (const auto& entry : fs::directory_iterator(v))
        {
            if (entry.is_regular_file())
            {
                let_ref path = entry.path();
                if (path.string().ends_with(POD_FILE_SUFFIX))
                {
                    //                    load_pods(path, <#initializer#>);
                }
            }
        }
    }
}

void PodsManager::create_pods(const fs::zpath& pods_path)
{
    ZPODS_ASSERT(!fs::exists(pods_path.c_str()));

    fs::create_directory_if_not_exist(pods_path.c_str());

    cur_state_of_peas_per_pods.insert({pods_path, {}});
}
