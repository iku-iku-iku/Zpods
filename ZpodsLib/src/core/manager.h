//
// Created by code4love on 23-11-15.
//

#ifndef ZPODS_MANAGER_H
#define ZPODS_MANAGER_H

#include "ZpodsLib/src/core/zpods_core.h"

namespace zpods
{

struct Tree2PodsMapping
{
    fs::zpath tree_path;
    fs::zpath pods_path;
    BackupConfig config;

    bool operator==(const Tree2PodsMapping& rhs) const;
};

} // namespace zpods

namespace std
{
template <>
struct hash<zpods::Tree2PodsMapping>
{
    std::size_t operator()(const zpods::Tree2PodsMapping& mapping) const
    {
        return std::hash<std::string>()(mapping.tree_path.string());
    }
};
} // namespace std

namespace zpods
{
class PodsManager
{
  public:
    static PodsManager* Instance();

    auto& get_path_mapping() { return tree2pods_; }

    void create_pods(const fs::zpath& pods_path);

    void load_pods_from_tracked_paths();

    Status load_pods(const fs::zpath& pods_path, const BackupConfig& config);

    void load_pods_mapping();

    void store_pods_mapping();

    void record_mapping(const fs::zpath& tree_path, const fs::zpath& pods_path,
                        BackupConfig config);

    const std::unordered_set<Pea>& current_pod(const fs::zpath& pod_path) const
    {
        ZPODS_ASSERT(cur_state_of_tree_per_pods.contains(pod_path));
        return cur_state_of_tree_per_pods.find(pod_path)->second;
    }

    std::unordered_set<Pea>& current_pod(const fs::zpath& pod_path)
    {
        ZPODS_ASSERT(cur_state_of_tree_per_pods.contains(pod_path));
        return cur_state_of_tree_per_pods.find(pod_path)->second;
    }

    std::unordered_set<Pea>
    filter_archived_peas(const fs::zpath& pod_path,
                         const std::unordered_set<Pea>& in_pod) const
    {
        if (!cur_state_of_tree_per_pods.contains(pod_path))
        {
            return in_pod;
        }
        std::unordered_set<Pea> res;
        let_ref cur_pod = current_pod(pod_path);
        for (const auto& pea : in_pod)
        {
            if (!cur_pod.contains(pea))
            {
                res.insert(pea);
            }
        }

        std::unordered_set<fs::zpath> in_pea_paths;
        for (const auto& item : in_pod)
        {
            in_pea_paths.insert(item.rel_path);
        }

        for (const auto& pea : cur_pod)
        {
            if (!in_pea_paths.contains(pea.rel_path))
            {
                const_cast<Pea&>(*res.insert(pea).first).deleted = true;
            }
        }
        return res;
    }

  private:
    // maintain current state of peas for each pods
    std::unordered_map<fs::zpath, std::unordered_set<Pea>>
        cur_state_of_tree_per_pods;
    std::unordered_set<Tree2PodsMapping> tree2pods_;
};
} // namespace zpods

// implement hash for Mapping

#endif // ZPODS_MANAGER_H
