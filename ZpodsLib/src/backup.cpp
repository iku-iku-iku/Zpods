#include "backup.h"
#include <filesystem>

using namespace zpods;

const BackupConfig BackupConfig::DEFAULT = {.encrypt = true, .compress = true, .filter = nullptr};

Status zpods::backup(const char *src_path, const char *target_dir, const BackupConfig &config) {
    // check src exist
    const auto src = std::filesystem::path(src_path);
    const auto target = std::filesystem::path(target_dir);

    if (!std::filesystem::exists(src)) {
        return Status::ERROR;
    }

    auto backup = target / src.filename();
    backup.replace_extension(".pods");

    if (std::filesystem::exists(backup)) {
        std::filesystem::remove_all(backup);
    }

    auto options = std::filesystem::copy_options::recursive |
                   std::filesystem::copy_options::skip_symlinks;

    std::filesystem::copy(src, backup, options);
    return Status::OK;
}

Status zpods::restore(const char *src_path, const char *target_path) {
    return Status();
}
