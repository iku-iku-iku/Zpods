#include <filesystem>
#include "config.h"
#include "pch.h"

namespace zpods
{
/*
 * @brief backup the src_path to target_dir
 * @param src_path is the path to back up, can be a file or a directory
 * @param target_dir is the path to put the backup file
 * @param config is the config of backup, default to BackupConfig::DEFAULT
 * @return Status::OK if backup successfully, otherwise Status::ERROR
 *
 * e.g. backup("/home/code4love/PROGRAM/cpp/Zpods", "/home/code4love/tmp/")
 * this will create a file called Zpods.pods in /home/code4love/tmp/
 * if the file (/home/code4love/tmp/Zpods.pods) already exists, it will be
 * overwritten by the new one!
 *
 * @note src_path must exist
 */
Status backup(const char* dest_dir, const BackupConfig& config);

/*
 * @brief sync backup the src_path to target_dir
 * @param src_path is the path to back up, can be a file or a directory
 * @param target_dir is the path to put the backup file
 * @param config is the config of backup, default to BackupConfig::DEFAULT
 * @return Status::OK if backup successfully, otherwise Status::ERROR
 */
Status sync_backup(const char* target_dir, const BackupConfig& config);

/*
 * @brief restore the backup file to target_dir
 * @param pods_dir is the path to the pods dir
 * @param target_dir is the path to restore
 * @return Status::OK if restore successfully, otherwise Status::ERROR
 */
Status restore(const char* pods_dir, const char* target_dir,
               BackupConfig config = {});
} // namespace zpods
