//
// Created by code4love on 23-9-24.
//

#include "gtest/gtest.h"
#include "archive.h"

TEST(ArchiveTest, test_archive_and_unarchive) {
    zpods::BackupConfig config;
    config.filter.paths.emplace_back(fmt::format("{}/single", zpods::test_data_path()));
    config.backup_filename = "single.zpods";
    EXPECT_EQ(zpods::Status::OK, zpods::archive(zpods::temp_path(), config));
    EXPECT_EQ(zpods::Status::OK,
              zpods::unarchive(fmt::format("{}/{}", zpods::temp_path(), *config.backup_filename).c_str(),
                               zpods::temp_path()));
}

TEST(ArchiveTest, test_recursive_archive_and_unarchive) {
    zpods::BackupConfig config;
    config.filter.paths.emplace_back(fmt::format("{}/recursive", zpods::test_data_path()));
    config.backup_filename = "recursive.zpods";
    EXPECT_EQ(zpods::Status::OK, zpods::archive(zpods::temp_path(), config));
    EXPECT_EQ(zpods::Status::OK,
              zpods::unarchive(fmt::format("{}/{}", zpods::temp_path(), *config.backup_filename).c_str(),
                               zpods::temp_path()));
}
