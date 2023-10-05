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

    // 2. compress if needed
    if (config.compress) {
        compress_file(archive_path.c_str(), archive_path.c_str());
        spdlog::info("file compression succeeded : {}", archive_path);
    }

    // 3. encrypt if needed
    if (config.crypto_config) {
        let status = encrypt_file(archive_path.c_str(), archive_path.c_str(), *config.crypto_config);
    if (status != Status::OK) {
            spdlog::error("file encryption failed: {}", archive_path);
            return status;
        }
        spdlog::info("file encryption succeeded : {}", archive_path);
    }

//    // set header
//    {
//        let content = fs::read_from_file(archive_path.c_str());
//        let_mut ofs = fs::open_or_create_file_as_ofs(archive_path.c_str(), fs::ios::binary);
//        let header = config.get_header();
//        ofs.write(as_c_str(header), sizeof(header));
//        ofs << content;
//    }


//    if (fs::exists(backup.c_str())) {
//        fs::remove_all(backup.c_str());
//    }

    return Status::OK;
}

Status zpods::restore(const char *src_path, const char *target_dir, BackupConfig config) {
    fs::create_directory_if_not_exist(target_dir);
    ZPODS_ASSERT(fs::is_directory(target_dir));

//    // read header
//    {
//        let status = config.read_header(src_path);
//        if (status == Status::PASSWORD_NEEDED) {
//            return status;
//        }
//    }
//
//    // remove header
//    {
//        let content = fs::read_from_file(src_path);
//
//        let_mut ofs = fs::open_or_create_file_as_ofs(src_path, fs::ios::binary);
//        ofs << content.substr(sizeof(BackupConfig::Header));
//    }

    if (config.crypto_config) {
        let status = decrypt_file(src_path, src_path, *config.crypto_config);
        if (status != Status::OK) {
            spdlog::error("file decryption failed: {}", src_path);
            return status;
        }
        spdlog::info("file decryption succeeded : {}", src_path);
    }

    if (config.compress) {
        decompress_file(src_path, src_path);
        spdlog::info("file decompression succeeded : {}", src_path);
    }

    zpods::unarchive(src_path, target_dir);

    return Status::OK;
}
