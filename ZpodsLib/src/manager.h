//
// Created by code4love on 23-11-15.
//

#ifndef ZPODS_MANAGER_H
#define ZPODS_MANAGER_H

#include "zpods_core.h"

namespace zpods {
    class PodsManager {
    public:
        static PodsManager *Instance();

        void create_pods(const fs::zpath& pods_path);

        void load_pods_from_tracked_paths();

        void load_pods(const fs::zpath &pea, const BackupConfig &config);

        void load_pods_path();

        void store_pods_path();

        void record_mapping(const fs::zpath &src_path, const fs::zpath &dst_path);

        const std::unordered_set<Pea> &current_pod(const fs::zpath &pod_path) const {
            ZPODS_ASSERT(cur_state_of_peas_per_pods.contains(pod_path));
            return cur_state_of_peas_per_pods.find(pod_path)->second;
        }

        std::unordered_set<Pea> &current_pod(const fs::zpath &pod_path) {
            ZPODS_ASSERT(cur_state_of_peas_per_pods.contains(pod_path));
            return cur_state_of_peas_per_pods.find(pod_path)->second;
        }

        std::unordered_set<Pea>
        filter_archived_peas(const fs::zpath &pod_path, const std::unordered_set<Pea> &in_pod) const {
            if (!cur_state_of_peas_per_pods.contains(pod_path)) {
                return in_pod;
            }
            std::unordered_set<Pea> res;
            let_ref cur_pod = current_pod(pod_path);
            for (const auto &pea: in_pod) {
                if (!cur_pod.contains(pea)) {
                    res.insert(pea);
                }
            }
            return res;
        }

    private:
        // maintain current state of peas for each pods
        std::unordered_map<fs::zpath, std::unordered_set<Pea>> cur_state_of_peas_per_pods;
        std::unordered_map<fs::zpath, fs::zpath> path_mapping_;
    };
}

#endif //ZPODS_MANAGER_H
