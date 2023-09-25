#include "backup.h"
#include <filesystem>
#include "fs.h"

using namespace zpods;

using namespace zpods::fs;

Status zpods::backup(const char *src_path, const char *target_dir, const BackupConfig &config) {
    // check src exist
    let src = fs::path(src_path);
    let target = fs::path(target_dir);

    if (!fs::exists(src.c_str())) {
        return Status::ERROR;
    }

    let_mut backup = target / src.filename();
    backup.replace_extension(".pods");

    if (fs::exists(backup.c_str())) {
        fs::remove_all(backup.c_str());
    }

    auto options = fs::copy_options::recursive | fs::copy_options::skip_symlinks;

    fs::copy(src.c_str(), backup.c_str(), options);
    return Status::OK;
}

Status zpods::restore(const char *src_path, const char *target_path) {
    return Status();
}
