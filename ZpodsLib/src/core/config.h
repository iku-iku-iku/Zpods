//
// Created by code4love on 23-9-30.
//

#ifndef ZPODS_CONFIG_H
#define ZPODS_CONFIG_H

#include <optional>
#include "ZpodsLib/src/base/fs.h"
#include "pch.h"

namespace zpods
{

constexpr auto POD_FILE_SUFFIX = ".pod";

struct CryptoConfig
{
    std::string key_;
    std::string iv_;

    static constexpr auto IV_SIZE = 16;
    static constexpr auto KEY_SIZE = 32;

    CryptoConfig() = default;

    explicit CryptoConfig(std::string key)
    {
        key_ = std::move(key);
        key_.resize(KEY_SIZE);

        std::random_device rd;
        std::mt19937 gen(rd());
        iv_ = std::to_string(gen());
        iv_.resize(IV_SIZE);
    }
};

class PodsManager;
struct BackupConfig
{
    static constexpr auto IV_SIZE = CryptoConfig::IV_SIZE;
    enum /* class BackupPolicy */ : uint8_t
    {
        NONE = 0,
        COMPRESS = 1 << 0,
        ENCRYPT = 1 << 1,
    };

    // this is only used in restore to implement incremental backup
    // pod with larger timestamp will be filtered
    long timestamp = -1;

    bool compress = false;                     ///< compress the backup file
    std::optional<CryptoConfig> crypto_config; ///< encrypt the backup file
    ///< the name of backup file,
    ///< if not set, default to ${src_path.filename()}.pods
    ///< will be overwritten by the real name after backup
    fs::FilesFilter filter; ///< filter the files to backup

    mutable fs::zpath current_pod_path;
    ///< the current_pod_path is the path to the current
    ///< pod file of this backup, it's generated with
    ///< 'ts.pods', where 'ts' is the timestamp of this
    ///< backup
    mutable fs::zpath tree_dir;
    ///< the tree_dir is the path to the root of the
    ///< tree to backup

    template <typename OStream>
        requires requires(OStream& os) { os << std::declval<std::string>(); }
    void serialize(OStream& os) const
    {
        os << "compress: " << compress << '\n';
        os << "crypto: " << crypto_config.has_value() << '\n';
        if (crypto_config.has_value())
        {
            os << crypto_config->key_ << '\n';
            os << crypto_config->iv_ << '\n';
        }
        filter.serialize(os);
    }

    template <typename IStream>
        requires requires(IStream& is, std::string s) { is >> s; }
    void deserialize(IStream& is)
    {
        std::istringstream iss;
        std::string line, s;
        bool crypto;

        std::getline(is, line);
        iss = std::istringstream(line);
        iss >> s >> compress;

        std::getline(is, line);
        iss = std::istringstream(line);
        iss >> s >> crypto;

        if (crypto)
        {
            crypto_config = {};
            std::getline(is, crypto_config->key_);
            std::getline(is, crypto_config->iv_);
        }

        std::getline(is, line);
        filter.deserialize(is);
    }
};
} // namespace zpods

#endif // ZPODS_CONFIG_H
