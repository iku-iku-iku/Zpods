//
// Created by code4love on 23-9-24.
//

#include "gtest/gtest.h"
#include "archive.h"

using namespace zpods;

void test_helper(const char *dir) {
    zpods::BackupConfig config;
    let src_dir = fmt::format("{}/{}", test_data_path(), dir);
    config.archive_path = fs::path(temp_path()) / fmt::format("{}.zpods", dir);
    EXPECT_EQ(zpods::Status::OK, zpods::archive(src_dir.c_str(), zpods::temp_path(), config) );
    EXPECT_EQ(zpods::Status::OK, zpods::unarchive(config.archive_path.c_str(), zpods::temp_path()) );
}

//
TEST(ArchiveTest, test_archive_and_unarchive) {
    let dir = "single";
    test_helper(dir);
}

TEST(ArchiveTest, test_recursive_archive_and_unarchive) {
    let dir = "recursive";
    test_helper(dir);
}
