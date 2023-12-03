//
// Created by code4love on 23-9-30.
//

#ifndef ZPODS_CONFIG_H
#define ZPODS_CONFIG_H

#include "pch.h"
#include "ZpodsLib/src/base/fs.h"

namespace zpods {

    constexpr auto POD_FILE_SUFFIX = ".pod";

    struct CryptoConfig {
        std::string key_;
        std::string iv_;

        static constexpr auto IV_SIZE = 16;
        static constexpr auto KEY_SIZE = 32;

        explicit CryptoConfig(std::string key) {
            key_ = std::move(key);
            key_.resize(KEY_SIZE);

            std::random_device rd;
            std::mt19937 gen(rd());
            iv_ = std::to_string(gen());
            iv_.resize(IV_SIZE);
        }
    };

    class PodsManager;
    struct BackupConfig {
        enum /* class BackupPolicy */: uint8_t {
            NONE = 0,
            COMPRESS = 1 << 0,
            ENCRYPT = 1 << 1,
        };

        static constexpr auto IV_SIZE = CryptoConfig::IV_SIZE;

        long timestamp = -1;

        bool compress = false; ///< compress the backup file
        std::optional<CryptoConfig> crypto_config; ///< encrypt the backup file
        mutable fs::zpath current_pod_path;
        mutable fs::zpath dir_to_backup;
        ///< the name of backup file,
        ///< if not set, default to ${src_path.filename()}.pods
        ///< will be overwritten by the real name after backup
        fs::FilesFilter filter; ///< filter the files to backup

    };
}

#endif //ZPODS_CONFIG_H
