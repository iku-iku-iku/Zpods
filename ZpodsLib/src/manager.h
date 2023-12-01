//
// Created by code4love on 23-11-15.
//

#ifndef ZPODS_MANAGER_H
#define ZPODS_MANAGER_H

#include "zpods_core.h"

namespace zpods {
    class PodsManager {
    public:
        void load_pods(const std::string& path);

        std::vector<Pod> get_pods() const;
    private:
        std::unordered_map<Pods::Id, Pods> pods_map_;
        // maintain a global view of all pods
        std::unordered_set<Pod> current_pods_;
    };
}

#endif //ZPODS_MANAGER_H
