#include "backup.h"
#include "compress.h"
#include "archive.h"
#include "fs.h"

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
    compress_file(archive_path.c_str(), archive_path.c_str());

    // TODO: add header for metadata

//    if (fs::exists(backup.c_str())) {
//        fs::remove_all(backup.c_str());
//    }

    return Status::OK;
}

Status zpods::restore(const char *src_path, const char *target_dir) {
    fs::create_directory_if_not_exist(target_dir);
    ZPODS_ASSERT(fs::is_directory(target_dir));

    decompress_file(src_path, src_path);

    zpods::unarchive(src_path, target_dir);

    return Status::OK;
}
