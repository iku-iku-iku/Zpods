//
// Created by code4love on 23-9-30.
//

#ifndef ZPODS_CONFIG_H
#define ZPODS_CONFIG_H

#include "pch.h"
#include "fs.h"

namespace zpods {

    constexpr auto PODS_FILE_SUFFIX = ".pods";

    struct CryptoConfig {
        std::string key_;
        std::string iv_;

        explicit CryptoConfig(std::string key) : key_(std::move(key)) {
//            std::random_device rd;
//            std::mt19937 gen(rd());
//            iv_ = std::to_string(gen());
            iv_ = "0123456789012345";
        }
    };

    struct BackupConfig {
        enum /* class BackupPolicy */: uint8_t{
            NONE = 0,
            COMPRESS = 1 << 0,
            ENCRYPT = 1 << 1,
        };
        bool compress = true; ///< compress the backup file
        std::optional<CryptoConfig> crypto_config; ///< encrypt the backup file
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

        struct Header {
            char magic[4] = {'Z', 'P', 'O', 'D'};
            char backup_policy = 0;
            char iv[32]{};
        };
        auto get_header() const -> Header{
            let_mut header = Header();
            if (this->compress) {
                header.backup_policy |= COMPRESS;
            }
            if (this->crypto_config) {
                ZPODS_ASSERT(this->crypto_config.has_value());
                header.backup_policy |= ENCRYPT;
                strcpy(header.iv, this->crypto_config->iv_.c_str());
            }
            return header;
        }
        auto read_header(const char* path) {
            let_mut ifs = fs::open_or_create_file_as_ifs(path, fs::ios::binary);
            Header header;
            ifs.read((char*)(&header), sizeof(header));
            this->compress = !!(header.backup_policy & COMPRESS);
            if (header.backup_policy & ENCRYPT) {
                if (!this->crypto_config.has_value()) {
                    return Status::PASSWORD_NEEDED;
                }
//                this->crypto_config->iv_ = header.iv;
                spdlog::info("IV: {} PASSWORD: {}", this->crypto_config->iv_, this->crypto_config->key_);
            }

            return Status::OK;
        }
    };

}

#endif //ZPODS_CONFIG_H
