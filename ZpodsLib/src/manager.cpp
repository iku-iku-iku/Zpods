//
// Created by code4love on 23-11-15.
//
#include "manager.h"

using namespace zpods;

PodsManager* PodsManager::Instance() {
    static PodsManager instance;
    return &instance;
}

void PodsManager::load_pods(const std::string &path) {
    let_mut paths = fs::get_file_family(path.c_str());
    std::sort(paths.begin(), paths.end(), [](const auto &lhs, const auto &rhs) {
        return strcmp(lhs.c_str(), rhs.c_str()) < 0;
    });

    for (const auto &pod_path: paths) {
        ZpodsHeader header;
        std::string bytes;

        read_zpods_file(pod_path.c_str(), header, bytes);

        let key = std::string((const char*)header.iv, 16);

        foreach_file_in_zpods_bytes((byte *) bytes.c_str(), [&](const PodHeader&pod_header) {
            if (pod_header.is_delete()) {
                let path = pod_header.get_path();

                erase_if(current_pods_, [&](const auto &pod) {
                    return pod.rel_path == path;
                });

            } else {
                let pod = Pod{pod_header.get_last_modified_ts(), pod_header.get_path()};
                current_pods_.insert(std::move(pod));
            }
            return Status::OK;
        });
    }
}

std::vector<Pod> PodsManager::get_pods() const {
    std::vector<Pod> res;
    for (const auto &[_, pods] : pods_map_) {
    }
    return res;
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
    for (const auto &[k,v ]: path_mapping_) {
        ofs << k << '\n';
        ofs << v << '\n';
    }
}

void PodsManager::record_mapping(const fs::zpath &src_path, const fs::zpath &dst_path) {
    path_mapping_[src_path] = dst_path;
    store_pods_path();
}
