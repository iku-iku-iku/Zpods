//
// Created by code4love on 23-11-15.
//
#include "manager.h"

using namespace zpods;

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