//
// Created by code4love on 23-9-30.
//

#ifndef ZPODS_CONFIG_H
#define ZPODS_CONFIG_H

#include "pch.h"
#include "fs.h"

class zpath;
namespace zpods {

    constexpr auto PODS_FILE_SUFFIX = ".pods";

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

    struct ZpodsHeader {
        static constexpr auto IV_SIZE = CryptoConfig::IV_SIZE;
        constexpr static auto MAGIC_SIZE = 4;
        constexpr static auto CHECKSUM_SIZE = 16;
        constexpr static auto PASSWORD_VERIFY_SIZE = 16;
        byte magic[MAGIC_SIZE] = {'Z', 'P', 'O', 'D'};
        byte iv[IV_SIZE]{};
        byte checksum[CHECKSUM_SIZE]{};
        byte password_verify_token[PASSWORD_VERIFY_SIZE]{};
        byte backup_policy = 0;
    };

    struct PodHeader {
        constexpr static auto CHECKSUM_SIZE = 16;

        size_t data_len;
        uint8_t path_len;
        byte checksum[CHECKSUM_SIZE];
        char path[];

        static auto as_header(auto *p) {
            return reinterpret_cast<PodHeader *>(p);
        }

        static constexpr auto compact_size() {
            return sizeof(PodHeader::data_len) + sizeof(PodHeader::path_len) + sizeof(checksum);
        }

        auto get_path() const {
            let p = reinterpret_cast<const char *>(this) + compact_size();
            return std::string(p, path_len);
        }

        [[nodiscard]] auto size() const {
            return compact_size() + path_len;
        }

        auto get_data() -> std::string_view const {
            let p = as_c_str(this);
            return {p + size(), data_len};
        }

        [[nodiscard]] auto empty() const {
            return data_len == 0 && path_len == 0;
        }
    };

    struct BackupConfig {
        enum /* class BackupPolicy */: uint8_t {
            NONE = 0,
            COMPRESS = 1 << 0,
            ENCRYPT = 1 << 1,
        };

        static constexpr auto IV_SIZE = CryptoConfig::IV_SIZE;

        bool delta_backup = false;

        bool compress = false; ///< compress the backup file
        std::optional<CryptoConfig> crypto_config; ///< encrypt the backup file
        mutable std::optional<std::string> backup_filename;
        ///< the name of backup file,
        ///< if not set, default to ${src_path.filename()}.pods
        ///< will be overwritten by the real name after backup
        fs::FilesFilter filter; ///< filter the files to backup

        auto get_header() const -> ZpodsHeader {
            let_mut header = ZpodsHeader();
            if (this->compress) {
                header.backup_policy |= COMPRESS;
            }
            if (this->crypto_config) {
                ZPODS_ASSERT(this->crypto_config.has_value());
                header.backup_policy |= ENCRYPT;
                memcpy(header.iv, this->crypto_config->iv_.c_str(), IV_SIZE);
            }
            return header;
        }

        auto read_header(ref<ZpodsHeader> header) {
            this->compress = !!(header.backup_policy & COMPRESS);
            if (header.backup_policy & ENCRYPT) {
                if (!this->crypto_config.has_value()) {
                    return Status::PASSWORD_NEEDED;
                }
                this->crypto_config->iv_ = std::string_view{as_c_str(header.iv), IV_SIZE};
                spdlog::info("IV: {} PASSWORD: {}", this->crypto_config->iv_, this->crypto_config->key_);
            }

            return Status::OK;
        }
    };
}

#endif //ZPODS_CONFIG_H
