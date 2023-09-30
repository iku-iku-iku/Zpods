//
// Created by code4love on 23-9-24.
//

#include "gtest/gtest.h"
#include "archive.h"

TEST(ArchiveTest, test_archive_and_unarchive) {
    zpods::BackupConfig config;
    zpods::archive(fmt::format("{}/tiny", zpods::test_data_path()).c_str(),
                   zpods::temp_path(), config);
    zpods::unarchive(fmt::format("{}/{}", zpods::temp_path(), *config.backup_filename).c_str(),
                     zpods::temp_path());
}

TEST(ArchiveTest, test_recursive_archive_and_unarchive) {
    zpods::BackupConfig config;
    zpods::archive(fmt::format("{}/recursive", zpods::test_data_path()).c_str(),
                   zpods::temp_path(), config);
    zpods::unarchive(fmt::format("{}/{}", zpods::temp_path(), *config.backup_filename).c_str(),
                     zpods::temp_path());
}
