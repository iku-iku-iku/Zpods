//
// Created by code4love on 23-10-23.
//

#ifndef ZPODS_ZPODS_CORE_H
#define ZPODS_ZPODS_CORE_H

#include "pch.h"
#include "config.h"
#include "crypto.h"
#include "compress.h"

namespace zpods {

    inline constexpr auto CHECKSUM_SIZE = ZpodsHeader::CHECKSUM_SIZE;

    void calculate_checksum(byte (&checksum)[CHECKSUM_SIZE], std::span<byte> bytes);

    void calculate_password_verify_token(ZpodsHeader &header, const std::string &password);


    // TODO: refactor std::string to span<byte>
    Status read_zpods_file(const char *path, zpods::ZpodsHeader &header, std::string &bytes);

    Status process_origin_zpods_bytes(const char *path, BackupConfig &config,
                                      const std::function<Status(std::string & )> &func);

    Status foreach_file_in_zpods_bytes(byte *bytes, const std::function<Status(const PodHeader &)> &func);

//    Status foreach_file_in_zpods_file(const char *path, ref_mut <BackupConfig> config,
//                                      ref <std::function<Status(ref < fs::zpath > , std::string_view)>> func);

    struct Pod {
        using Id = std::string;
        long last_modified_ts;

        Id rel_path;

        bool operator==(const Pod &rhs) const {
            return rel_path == rhs.rel_path;
        }
    };


    struct Pods {
        using Id = std::string;
        Id pods_id;

        std::unordered_map<Pod::Id, Pod> map;
    };

}

namespace std {
    template<>
    struct hash<zpods::Pod> {
        size_t operator()(const zpods::Pod &pod) const {
            return hash<std::string>()(pod.rel_path);
        }
    };
}

#endif //ZPODS_ZPODS_CORE_H
