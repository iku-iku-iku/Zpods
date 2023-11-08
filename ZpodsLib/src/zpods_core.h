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

    void calculate_password_verify_token(ref_mut <ZpodsHeader> header, ref <std::string> password);


    Status read_zpods_file(const char *path, zpods::ZpodsHeader &header, std::string &bytes);

    Status process_origin_zpods_bytes(const char *path, ref_mut <BackupConfig> config,
                                      ref <std::function<Status(std::string & )>> func);

    Status foreach_file_in_zpods_bytes(byte *bytes, ref <std::function<Status(ref<PodHeader>)>> func);

//    Status foreach_file_in_zpods_file(const char *path, ref_mut <BackupConfig> config,
//                                      ref <std::function<Status(ref < fs::zpath > , std::string_view)>> func);
}

#endif //ZPODS_ZPODS_CORE_H
