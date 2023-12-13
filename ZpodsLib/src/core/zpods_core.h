//
// Created by code4love on 23-10-23.
//

#ifndef ZPODS_ZPODS_CORE_H
#define ZPODS_ZPODS_CORE_H

#include "compress.h"
#include "config.h"
#include "crypto.h"
#include "pch.h"

namespace zpods
{

// 获取当前的Unix时间戳
inline long get_current_timestamp()
{
    // 获取当前时间点
    let now = std::chrono::system_clock::now();

    // 转换为时间戳
    let duration = now.time_since_epoch();

    // 将时间戳转换为秒
    let seconds = std::chrono::duration_cast<std::chrono::seconds>(duration);

    // 返回秒数
    return seconds.count();
}

constexpr static auto CHECKSUM_SIZE = 16;
struct PodHeader
{
    static constexpr auto IV_SIZE = CryptoConfig::IV_SIZE;
    constexpr static auto MAGIC_SIZE = 4;
    constexpr static auto CHECKSUM_SIZE = 16;
    constexpr static auto PASSWORD_VERIFY_SIZE = 16;
    byte magic[MAGIC_SIZE] = {'Z', 'P', 'O', 'D'};
    byte iv[IV_SIZE]{}; // iv is also taken as pods' id!
    byte checksum[CHECKSUM_SIZE]{};
    byte password_verify_token[PASSWORD_VERIFY_SIZE]{};
    byte backup_policy = 0;

    // make up a header according to the backup config
    static auto from(const BackupConfig& config) -> PodHeader
    {
        let_mut header = PodHeader();
        if (config.compress)
        {
            header.backup_policy |= BackupConfig::COMPRESS;
        }
        if (config.crypto_config)
        {
            header.backup_policy |= BackupConfig::ENCRYPT;
            memcpy(header.iv, config.crypto_config->iv_.c_str(),
                   BackupConfig::IV_SIZE);
        }
        return header;
    }

    auto fill_config(BackupConfig& config) const
    {
        config.compress = !!(this->backup_policy & BackupConfig::COMPRESS);
        if (this->backup_policy & BackupConfig::ENCRYPT)
        {
            if (!config.crypto_config.has_value())
            {
                return Status::PASSWORD_NEEDED;
            }
            config.crypto_config->iv_ =
                std::string_view{as_c_str(this->iv), IV_SIZE};
            spdlog::info("IV: {} PASSWORD: {}", config.crypto_config->iv_,
                         config.crypto_config->key_);
        }

        return Status::OK;
    }
};

struct PeaHeader
{
    union
    {
        //            uint32_t data;
        uint8_t bytes[4];
    } last_modified_ts; // the last modified timestamp of file
    union
    {
        //            uint64_t data;
        uint8_t bytes[8];
    } data_len;        // the length of data
    uint8_t path_len;  // the length of path
    uint8_t flags = 0; // 0: normal, 1: delete
    char path[];       // the path of file

    void set_normal() { flags = 0; }

    void set_delete() { flags |= 1; }

    void set_path_len(uint8_t len) { path_len = len; }

    void set_last_modified_ts(uint32_t ts)
    {
        memcpy(last_modified_ts.bytes, &ts, sizeof(ts));
    }

    uint64_t get_data_len() const
    {
        uint64_t len;
        memcpy(&len, data_len.bytes, sizeof(len));
        return len;
    }

    uint32_t get_last_modified_ts() const
    {
        uint32_t ts;
        memcpy(&ts, last_modified_ts.bytes, sizeof(ts));
        return ts;
    }

    void set_data_len(uint64_t len)
    {
        memcpy(data_len.bytes, &len, sizeof(len));
    }

    static auto as_header(auto* p) { return reinterpret_cast<PeaHeader*>(p); }

    static constexpr auto compact_size()
    {
        static_assert(sizeof(last_modified_ts) + sizeof(data_len) +
                          sizeof(path_len) + sizeof(flags) ==
                      sizeof(PeaHeader));
        return sizeof(PeaHeader);
    }

    bool is_delete() const { return flags & 1; }

    auto get_path() const
    {
        let p = reinterpret_cast<const char*>(this) + compact_size();
        return std::string(p, path_len);
    }

    [[nodiscard]] auto size() const { return compact_size() + path_len; }

    std::string_view get_data() const
    {
        let p = as_c_str(this);
        return {p + size(), get_data_len()};
    }

    [[nodiscard]] auto empty() const
    {
        return get_data_len() == 0 && path_len == 0;
    }
};

struct Pea
{
    long last_modified_ts;
    std::string rel_path;
    std::string abs_path;
    bool deleted = false;

    // one pea will only be archived in one pod
    fs::zpath resident_pod_path;

    bool operator==(const Pea& rhs) const
    {
        return rel_path == rhs.rel_path &&
               last_modified_ts == rhs.last_modified_ts;
    }
};

void calculate_password_verify_token(PodHeader& header,
                                     const std::string& password);

void calculate_checksum(byte (&checksum)[CHECKSUM_SIZE], std::span<byte> bytes);

// TODO: refactor std::string to span<byte>
Status read_pod_file(const char* path, zpods::PodHeader& header,
                     std::string& bytes);

Status
process_origin_zpods_bytes(const char* path, BackupConfig& config,
                           const std::function<Status(std::string&)>& func);

Status
foreach_pea_in_pod_bytes(byte* bytes,
                         const std::function<Status(const PeaHeader&)>& func);

//    Status foreach_file_in_zpods_file(const char *path, ref_mut <BackupConfig>
//    config,
//                                      ref <std::function<Status(ref <
//                                      fs::zpath > , std::string_view)>> func);

} // namespace zpods

namespace std
{
template <>
struct hash<zpods::Pea>
{
    size_t operator()(const zpods::Pea& pod) const
    {
        return hash<std::string>()(pod.rel_path) ^
               hash<long>()(pod.last_modified_ts);
    }
};
} // namespace std

template <>
struct fmt::formatter<zpods::Pea>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.end(); }

    template <typename FormatContext>
    auto format(const zpods::Pea& p, FormatContext& ctx) const
    {
        return format_to(ctx.out(), "rel_path: {}, last_modified_ts: {}",
                         p.rel_path, p.last_modified_ts);
    }
};

#endif // ZPODS_ZPODS_CORE_H
