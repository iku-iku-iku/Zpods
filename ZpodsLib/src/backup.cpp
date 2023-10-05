#include "backup.h"
#include "compress.h"
#include "archive.h"
#include "fs.h"
#include "crypto.h"

using namespace zpods;

using namespace zpods::fs;

Status zpods::backup(const char *src_path, const char *target_dir, ref <BackupConfig> config) {
    fs::create_directory_if_not_exist(target_dir);
    ZPODS_ASSERT(fs::is_directory(target_dir));
    // check src exist
    let src = fs::path(src_path);
    let target = fs::path(target_dir);

    if (!fs::exists(src.c_str())) {
        return Status::ERROR;
    }

    // 1. archive files of src_path to a single file in target_dir
    zpods::archive(src_path, target_dir, config);
    ZPODS_ASSERT(config.backup_filename.has_value());
    let archive_path = fmt::format("{}/{}", target_dir, config.backup_filename.value());

    let_mut bytes = fs::read_from_file(archive_path.c_str());
    std::unique_ptr<byte[]> buf;
    size_t buf_len;
    // 2. compress if needed
    if (config.compress) {
        std::tie(buf_len, buf) = compress({(p_byte) bytes.data(), bytes.size()});
        bytes = {as_c_str(buf.get()), buf_len};
        spdlog::info("file compression succeeded : {}", archive_path);
    }

    // 3. encrypt if needed
    if (config.crypto_config) {
        let_ref conf = config.crypto_config;
        let cipher = encrypt({as_c_str(bytes.data()), bytes.size()}, conf->key_, conf->iv_);
        if (!cipher.has_value()) {
            spdlog::error("file encryption failed: {}", archive_path);
            return Status::ERROR;
        }
        bytes = {cipher->data(), cipher->size()};
        spdlog::info("file encryption succeeded : {}", archive_path);
    }

    // output to target file
    {
        let_mut ofs = fs::open_or_create_file_as_ofs(archive_path.c_str(), fs::ios::binary);

        let header = config.get_header();
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
    {
        BackupConfig::Header header;
        ifs.read((char *) (&header), sizeof(header));
        let status = config.read_header(header);
        if (status == Status::PASSWORD_NEEDED) {
            return status;
        }
    }
    // read content
    std::string bytes((std::istreambuf_iterator<char>(ifs)),
                      (std::istreambuf_iterator<char>()));

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

    // write to target file
    {
        let_mut ofs = fs::open_or_create_file_as_ofs(src_path, fs::ios::binary);
        ofs << bytes;
    }

    zpods::unarchive(src_path, target_dir);

    return Status::OK;
}
