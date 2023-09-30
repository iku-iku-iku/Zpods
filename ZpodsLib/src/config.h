//
// Created by code4love on 23-9-30.
//

#ifndef ZPODS_CONFIG_H
#define ZPODS_CONFIG_H

#include "pch.h"
#include "fs.h"

namespace zpods {

    constexpr auto PODS_FILE_SUFFIX = ".pods";

    struct BackupConfig {
        bool encrypt = true; ///< raw_encrypt the backup file
        bool compress = true; ///< compress the backup file
        mutable std::optional<std::string> backup_filename;
        ///< the name of backup file,
        ///< if not set, default to ${src_path.filename()}.pods
        ///< will be overwritten by the real name after backup

        struct Filter {
            std::vector<fs::zpath> ignored_paths; ///< ignore these paths
            std::vector<FileType> ignored_types; ///< ignore these types
            std::vector<std::string> ignored_names; ///< ignore these names
            uint64_t min_time; ///< only backup files that are modified after min_time
            uint64_t max_time; ///< only backup files that are modified before max_time
            uint64_t min_size; ///< only backup files that are larger than min_size
            uint64_t max_size; ///< only backup files that are smaller than max_size
        } filter;
    };

}

#endif //ZPODS_CONFIG_H
