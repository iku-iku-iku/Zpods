//
// Created by code4love on 23-10-23.
//

#include "zpods_core.h"


namespace {
    using namespace zpods;

    Status check_header(const zpods::ZpodsHeader &header) {
        let std_header = zpods::ZpodsHeader();
        if (memcmp(header.magic, std_header.magic, sizeof(header.magic)) != 0) {
            return Status::NOT_ZPODS_FILE;
        }

        return Status::OK;
    }
}

zpods::Status zpods::read_zpods_file(const char *path, zpods::ZpodsHeader &header, std::string &bytes) {
    let_mut ifs = fs::open_or_create_file_as_ifs(path, fs::ios::binary);
    if (!ifs.is_open()) {
        return Status::ERROR;
    }
    // read header
    ifs.read((char *) (&header), sizeof(header));
    // read content
    bytes = {(std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>())};

    return Status::OK;
}

void zpods::calculate_checksum(byte (&checksum)[16], std::span<byte> bytes) {
    memset(checksum, 0, sizeof(checksum));
    ssize_t i = 0;
    for (let_ref byte: bytes) {
        checksum[i % CHECKSUM_SIZE] ^= byte;
        i++;
    }
}

Status zpods::process_origin_zpods_bytes(const char *path, BackupConfig &config,
                                         const std::function<Status(std::string & )> &func) {
    ZpodsHeader header;
    std::string bytes;

    {
        let status = read_zpods_file(path, header, bytes);
        if (status != Status::OK) {
            return status;
        }
    }

    // read header
    {
        // check header
        {
            let status = check_header(header);
            if (status != Status::OK) {
                return status;
            }
        }

        // parse header
        {
            let status = config.read_header(header);
            if (status != Status::OK) {
                return status;
            }
        }
    }

    // check checksum
    {
        byte checksum[CHECKSUM_SIZE];
        calculate_checksum(checksum, {(p_byte) bytes.data(), bytes.size()});
        if (memcmp(checksum, header.checksum, sizeof(checksum)) != 0) {
            return Status::CHECKSUM_ERROR;
        }
    }

    // check password validity if encrypted
    if (header.backup_policy & BackupConfig::ENCRYPT) {
        let current_password_verify_token = std::string{as_c_str(header.password_verify_token),
                                                        sizeof(header.password_verify_token)};
        calculate_password_verify_token(header, config.crypto_config->key_);
        if (memcmp(header.password_verify_token,
                   current_password_verify_token.c_str(),
                   ZpodsHeader::PASSWORD_VERIFY_SIZE) != 0) {
            return Status::WRONG_PASSWORD;
        }
    }

    // decrypt if needed
    if (config.crypto_config) {
        let_ref conf = config.crypto_config;
        let plain_text = decrypt({as_c_str(bytes.data()), bytes.size()}, conf->key_, conf->iv_);
        if (!plain_text.has_value()) {
            spdlog::error("file decryption failed: {}", path);
            return Status::ERROR;
        }
        bytes = {plain_text->data(), plain_text->size()};
        spdlog::info("file decryption succeeded : {}", path);
    }

    // decompress if needed
    if (config.compress) {
        let [buf_len, buf] = decompress((p_cbyte) bytes.data());
        bytes = {as_c_str(buf.get()), buf_len};
        spdlog::info("file decompression succeeded : {}", path);
    }

    return func(bytes);
}

//Status zpods::foreach_file_in_zpods_file(const char *path, ref_mut <BackupConfig> config,
//                                         ref <std::function<Status(ref < fs::zpath > , std::string_view)>> func) {
//    return process_origin_zpods_bytes(path, config, [&](auto &bytes) {
//        return foreach_file_in_zpods_bytes(as_p_byte(bytes.c_str()), func);
//    });
//}

void zpods::calculate_password_verify_token(ZpodsHeader& header, const std::string &password) {
    let_mut cipher = zpods::encrypt(
            {as_c_str(header), sizeof(header)},
            password,
            {as_c_str(header.iv), sizeof(header.iv)}
    );

    memcpy(header.password_verify_token, cipher->c_str(), sizeof(header.password_verify_token));
}

Status zpods::foreach_file_in_zpods_bytes(byte *bytes, const std::function<Status(const PodHeader &)> &func) {
    let_mut p = bytes;
    PodHeader *header;
    while (!(header = PodHeader::as_header(p))->empty()) {
        {
            let st = func(*header);
            if (st != Status::OK) {
                return st;
            }
        }

        p += header->size() + header->data_len;
    }
    return Status::OK;
}
