#include "pch.h"
#include <filesystem>

namespace zpods {
    struct BackupConfig {
        bool encrypt; ///< raw_encrypt the backup file
        bool compress; ///< compress the backup file

        struct Filter {
            std::vector<std::filesystem::path> ignored_paths; ///< ignore these paths
            std::vector<FileType> ignored_types; ///< ignore these types
            std::vector<std::string> ignored_names; ///< ignore these names
            uint64_t min_time; ///< only backup files that are modified after min_time
            uint64_t max_time; ///< only backup files that are modified before max_time
            uint64_t min_size; ///< only backup files that are larger than min_size
            uint64_t max_size; ///< only backup files that are smaller than max_size
        }* filter;

        static const BackupConfig DEFAULT;
    };

    /*
     * @brief backup the src_path to target_dir
     * @param src_path is the path to back up, can be a file or a directory
     * @param target_dir is the path to put the backup file
     * @param config is the config of backup, default to BackupConfig::DEFAULT
     * @return Status::OK if backup successfully, otherwise Status::ERROR
     *
     * e.g. backup("/home/code4love/PROGRAM/cpp/Zpods", "/home/code4love/tmp/")
     * this will create a file called Zpods.pods in /home/code4love/tmp/
     * if the file (/home/code4love/tmp/Zpods.pods) already exists, it will be overwritten by the new one!
     *
     * @note src_path must exist
     */
    Status backup(const char *src_path, const char *target_dir, const BackupConfig &config = BackupConfig::DEFAULT);

    /*
     * @brief restore the backup file to target_dir
     * @param src_path is the path to the backup file
     * @param target_dir is the path to restore
     * @return Status::OK if restore successfully, otherwise Status::ERROR
     *
     * e.g. restore("/home/code4love/tmp/Zpods.pods", "/home/code4love/PROGRAM/cpp/")
     * this will restore the backup file to /home/code4love/PROGRAM/cpp/Zpods
     * if the file (/home/code4love/PROGRAM/cpp/Zpods) already exists, it will be overwritten by the new one!
     * no need to care whether the backup file is encrypted or compressed, this function will handle it,
     * i.e. raw_decrypt or decompress it automatically if needed
     *
     * @note src_path must exist and must be a valid pods file, target_dir must be a directory
     */
    Status restore(const char *src_path, const char *target_dir);
}