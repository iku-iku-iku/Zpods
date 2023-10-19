#include "backup.h"
#include "compress.h"
#include "archive.h"
#include "fs.h"
#include "crypto.h"

using namespace zpods;

using namespace zpods::fs;

static constexpr auto CHECKSUM_SIZE = ZpodsHeader::CHECKSUM_SIZE;

namespace {
    Status check_header(ref <zpods::ZpodsHeader> header) {
        let std_header = zpods::ZpodsHeader();
        if (memcmp(header.magic, std_header.magic, sizeof(header.magic)) != 0) {
            return Status::NOT_ZPODS_FILE;
        }

        return Status::OK;
    }

    void calculate_checksum(byte (&checksum)[CHECKSUM_SIZE], std::span<byte> bytes) {
        memset(checksum, 0, sizeof(checksum));
        ssize_t i = 0;
        for (let_ref byte: bytes) {
            checksum[i % CHECKSUM_SIZE] ^= byte;
            i++;
        }
    }

    void calculate_password_verify_token(ref_mut <ZpodsHeader> header, ref <std::string> password) {
        let_mut cipher = zpods::encrypt(
                {as_c_str(header), sizeof(header)},
                password,
                {as_c_str(header.iv), sizeof(header.iv)}
        );

        memcpy(header.password_verify_token, cipher->c_str(), sizeof(header.password_verify_token));
    }
}

Status zpods::backup(const char *target_dir, ref <BackupConfig> config) {
    fs::create_directory_if_not_exist(target_dir);
    ZPODS_ASSERT(fs::is_directory(target_dir));
    let target = fs::path(target_dir);

    // check src not empty
    if (config.filter.paths.empty()) {
        return Status::EMPTY;
    }

    // check src exist
    for (const auto &item: config.filter.paths) {
        if (!fs::exists(item.c_str())) {
            return Status::PATH_NOT_EXIST;
        }
    }

    // decide backup file name
    if (!config.backup_filename.has_value()) {
        config.backup_filename = fmt::format("{}{}", config.filter.paths[0].filename().c_str(),
                                             PODS_FILE_SUFFIX);
    }

    let archive_path = target / *config.backup_filename;

    // remove target
    if (fs::exists(target.c_str())) {
        fs::remove_all(target.c_str());
    }

    // 1. archive files of src_path to a single file in target_dir
    zpods::archive(target_dir, config);
    ZPODS_ASSERT(config.backup_filename.has_value());

    let_mut bytes = fs::read_from_file(archive_path.c_str());
    std::unique_ptr<byte[]> buf;
    size_t buf_len;
    // 2. compress if needed
    if (config.compress) {
        std::tie(buf_len, buf) = compress({(p_byte) bytes.data(), bytes.size()});
        bytes = {as_c_str(buf.get()), buf_len};
        spdlog::info("file compression succeeded : {}", archive_path.c_str());
    }

    // 3. encrypt if needed
    if (config.crypto_config) {
        let_ref conf = config.crypto_config;
        let cipher = encrypt({as_c_str(bytes.data()), bytes.size()}, conf->key_, conf->iv_);
        if (!cipher.has_value()) {
            spdlog::error("file encryption failed: {}", archive_path.c_str());
            return Status::ERROR;
        }
        bytes = {cipher->data(), cipher->size()};
        spdlog::info("file encryption succeeded : {}", archive_path.c_str());
    }

    // output to target file
    {
        let_mut ofs = fs::open_or_create_file_as_ofs(archive_path.c_str(), fs::ios::binary);

        let_mut header = config.get_header();
        calculate_checksum(header.checksum, {(p_byte) bytes.data(), bytes.size()});
        calculate_password_verify_token(header, config.crypto_config->key_);
        ofs.write(as_c_str(header), sizeof(header));
        ofs << bytes;
    }

    return Status::OK;
}

Status zpods::restore(const char *src_path, const char *target_dir, BackupConfig config) {
    fs::create_directory_if_not_exist(target_dir);
    ZPODS_ASSERT(fs::is_directory(target_dir));

    let_mut ifs = fs::open_or_create_file_as_ifs(src_path, fs::ios::binary);
    // read header
    ZpodsHeader header;
    {
        ifs.read((char *) (&header), sizeof(header));

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
    // read content
    std::string bytes((std::istreambuf_iterator<char>(ifs)),
                      (std::istreambuf_iterator<char>()));

    // check checksum
    {
        byte checksum[CHECKSUM_SIZE];
        calculate_checksum(checksum, {(p_byte) bytes.data(), bytes.size()});
        if (memcmp(checksum, header.checksum, sizeof(checksum)) != 0) {
            return Status::CHECKSUM_ERROR;
        }
    }

    // check password validity
    {
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
            spdlog::error("file decryption failed: {}", src_path);
            return Status::ERROR;
        }
        bytes = {plain_text->data(), plain_text->size()};
        spdlog::info("file decryption succeeded : {}", src_path);
    }

    // decompress if needed
    if (config.compress) {
        let [buf_len, buf] = decompress((p_cbyte) bytes.data());
        bytes = {as_c_str(buf.get()), buf_len};
        spdlog::info("file decompression succeeded : {}", src_path);
    }

    zpods::unarchive({(p_byte) bytes.data(), bytes.size()}, target_dir);

    return Status::OK;
}

Status zpods::sync_backup(const char *target_dir, ref <BackupConfig> config) {
    // make sure the backup file is up-to-date
    zpods::backup(target_dir, config);

    // backup callback if the src_path is modified
    let callback = [&](const char *path) {
        zpods::backup(target_dir, config);
    };

    // create a watcher in a thread for each path
    std::vector<std::thread> threads;
    threads.reserve(config.filter.paths.size());
    for (let_ref path: config.filter.paths) {
        threads.emplace_back([=] {
            FsWatcher watcher(path.c_str(), {
                    .on_file_create = callback,
                    .on_file_delete = callback,
                    .on_file_modify = callback,
                    .on_dir_create = callback,
                    .on_dir_delete = callback,
            });
        });
    }
    for (let_mut_ref item: threads) { item.join(); }

    return Status::OK;
}
