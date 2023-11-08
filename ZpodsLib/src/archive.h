//
// Created by code4love on 23-9-24.
//

#ifndef ZPODS_ARCHIVE_H
#define ZPODS_ARCHIVE_H

#include "pch.h"
#include "config.h"

namespace zpods {
    /*
     * @brief archive a directory (or a file) indicated by src_path to a single file in target_dir
     * @param src_path the path of the directory (or a file) to be archived
     * @param target_dir the directory to store the archived file
     * @return Status::OK if success, otherwise Status::ERROR
     */
    Status archive(const char *target_dir, const BackupConfig& config = {});

    /*
     * @brief unarchive a archive file indicated by src_path to a directory in target_dir
     * @param src_path the path of the archive file to be unarchived
     * @param target_dir the directory to store the unarchived files
     * @return Status::OK if success, otherwise Status::ERROR
     */
    Status unarchive(const char *src_path, const char *target_dir);

    Status unarchive(std::span<byte> src_bytes, const char *target_dir);
}

#endif //ZPODS_ARCHIVE_H
