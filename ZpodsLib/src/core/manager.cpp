//
// Created by code4love on 23-11-15.
//
#include "config.h"
#include "fs.h"
#include "manager.h"

using namespace zpods;

bool Tree2PodsMapping::operator==(const Tree2PodsMapping& rhs) const
{
    return std::hash<Tree2PodsMapping>()(*this) ==
           std::hash<Tree2PodsMapping>()(rhs);
}
PodsManager* PodsManager::Instance()
{
    static PodsManager instance;
    return &instance;
}

Status PodsManager::load_pods(const fs::zpath& pods_path,
                              const BackupConfig& config)
{
    ZPODS_ASSERT(fs::is_directory(pods_path));
    if (!cur_state_of_tree_per_pods.contains(pods_path))
    {
        cur_state_of_tree_per_pods.insert({pods_path, {}});
    }

    std::vector<fs::zpath> pod_paths;
    for (let_ref pod : fs::directory_iterator(pods_path))
    {
        let_ref pod_path = pod.path();
        // find all files with pod suffix
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

    let_mut_ref cur_pod = tree_state_of_pods(pods_path);
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

const auto PODS_MAPPING_PATH()
{
    return ZPODS_HOME_PATH / "tree2pods";
}

void PodsManager::load_pods_mapping()
{
    let pods_mapping = PODS_MAPPING_PATH();

    if (fs::exists(pods_mapping.c_str()))
    {
        let_mut ifs = std::ifstream(pods_mapping);
        std::string line1;
        std::string line2;
        while (std::getline(ifs, line1) && std::getline(ifs, line2))
        {
            Tree2PodsMapping mapping;
            spdlog::info("line1 {} line2 {}", line1, line2);
            mapping.tree_path = line1;
            mapping.pods_path = line2;

            let_mut meta_ifs = std::ifstream(mapping.pods_path / ".META");
            mapping.config.deserialize(meta_ifs);

            tree2pods_.insert(std::move(mapping));
        }
    }
}

void PodsManager::store_pods_mapping()
{
    let pods_mapping = PODS_MAPPING_PATH();

    let_mut ofs = fs::open_or_create_file_as_ofs(pods_mapping, fs::ios::text);

    for (let_ref mapping : tree2pods_)
    {
        ofs << mapping.tree_path.c_str() << '\n';
        ofs << mapping.pods_path.c_str() << '\n';

        let_mut meta_ofs = fs::open_or_create_file_as_ofs(
            mapping.pods_path / ".META", fs::ios::text);
        mapping.config.serialize(meta_ofs);
    }
}

void PodsManager::record_mapping(const fs::zpath& tree_path,
                                 const fs::zpath& pods_path,
                                 BackupConfig config)
{
    tree2pods_.insert({.tree_path = tree_path,
                       .pods_path = pods_path,
                       .config = std::move(config)});
    store_pods_mapping();
}

void PodsManager::create_pods(const fs::zpath& pods_path)
{
    ZPODS_ASSERT(!fs::exists(pods_path.c_str()));

    fs::create_directory_if_not_exist(pods_path.c_str());

    cur_state_of_tree_per_pods.insert({pods_path, {}});
}
