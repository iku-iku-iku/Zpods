//
// Created by code4love on 23-11-15.
//

#ifndef ZPODS_MANAGER_H
#define ZPODS_MANAGER_H

#include "zpods_core.h"

namespace zpods {
    class PodsManager {
    public:
        static PodsManager* Instance();
        void load_pods_from_tracked_paths();
        void load_pods(const std::string& path);

        void load_pods_path();
        void store_pods_path();
        void record_mapping(const fs::zpath& src_path, const fs::zpath& dst_path);
        const std::unordered_set<Pod>& current_pods() const { return current_pods_; }

        std::unordered_set<Pod> filter_archived_pods(const std::unordered_set<Pod>& pods) const {
            std::unordered_set<Pod> res;
            for (const auto& pod: pods) {
                if (current_pods_.find(pod) == current_pods_.end()) {
                    res.insert(pod);
                }
            }
            return res;
        }
    private:
        std::unordered_map<Pods::Id, Pods> pods_map_;
        // maintain a global view of all pods
        std::unordered_set<Pod> current_pods_;
        std::unordered_map<fs::zpath, fs::zpath> path_mapping_;
    };
}

#endif //ZPODS_MANAGER_H
