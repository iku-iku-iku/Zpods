//
// Created by code4love on 23-11-15.
//
#include "manager.h"

using namespace zpods;

PodsManager *PodsManager::Instance() {
    static PodsManager instance;
    return &instance;
}

void PodsManager::load_pods(const fs::zpath &pods_path) {
//    let_mut paths = fs::get_file_family(pods_path.c_str());
    ZPODS_ASSERT(fs::is_directory(pods_path));

    std::vector<fs::zpath> pod_paths;
    for (let_ref pod: fs::directory_iterator(pods_path)) {
        let_ref pod_path = pod.path();
        if (pod_path.string().ends_with(POD_FILE_SUFFIX)) {
            pod_paths.push_back(pod_path);
        }
    }

    std::sort(pod_paths.begin(), pod_paths.end(), [](const auto &lhs, const auto &rhs) {
        let_mut ss1 = std::stringstream(lhs.filename().c_str());
        let_mut ss2 = std::stringstream(rhs.filename().c_str());
        long ts1, ts2;
        ss1 >> ts1;
        ss2 >> ts2;
        return ts1 < ts2;
    });

    let_mut_ref cur_pod = current_pod(pods_path);
    for (let_ref pod_path: pod_paths) {
        PodHeader header;
        std::string bytes;

        read_pod_file(pod_path.c_str(), header, bytes);

//        let key = std::string((const char*)header.iv, 16);

        foreach_pea_in_pod_bytes((byte *) bytes.c_str(), [&](const PeaHeader &pea_header) {
            spdlog::info("GET {}", pea_header.get_path());
            if (pea_header.is_delete()) {
                let path = pea_header.get_path();

                erase_if(cur_pod, [&](const auto &pea) {
                    return pea.rel_path == path;
                });

            } else {
                let_mut pea = Pea{
                        .last_modified_ts= pea_header.get_last_modified_ts(),
                        .rel_path = pea_header.get_path()
                };
                cur_pod.insert(std::move(pea));
            }
            return Status::OK;
        });
    }
}

void PodsManager::load_pods_path() {
    if (fs::exists("./pods_paths")) {
        let_mut ifs = std::ifstream("./pods_paths");
        std::string line1;
        std::string line2;
        while (std::getline(ifs, line1) && std::getline(ifs, line2)) {
            path_mapping_[line1] = line2;
        }
    }
}

void PodsManager::store_pods_path() {
    let_mut ofs = fs::open_or_create_file_as_ofs("./pods_paths", fs::ios::binary);
    for (const auto &[k, v]: path_mapping_) {
        ofs << k << '\n';
        ofs << v << '\n';
    }
}

void PodsManager::record_mapping(const fs::zpath &src_path, const fs::zpath &dst_path) {
    path_mapping_[src_path] = dst_path;
    store_pods_path();
}

void PodsManager::load_pods_from_tracked_paths() {
    load_pods_path();

    for (const auto &[k, v]: path_mapping_) {
        for (const auto &entry: fs::directory_iterator(v)) {
            if (entry.is_regular_file()) {
                let_ref path = entry.path();
                if (path.string().ends_with(POD_FILE_SUFFIX)) {
                    load_pods(path);
                }
            }
        }
    }
}

void PodsManager::create_pods(const fs::zpath &pods_path) {
    ZPODS_ASSERT(!fs::exists(pods_path.c_str()));

    fs::create_directory_if_not_exist(pods_path.c_str());

    cur_state_of_peas_per_pods.insert({pods_path, {}});
}
