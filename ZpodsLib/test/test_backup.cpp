// 测试代码
#include <gtest/gtest.h>
#include <filesystem>
#include "zpods_lib.h"

using namespace zpods;

TEST(BackupTest, BackupRestore) {
    Status status;
    let dir = "recursive";
    let src_path = fmt::format("{}/{}", zpods::test_data_path(), dir);
    let dest_path = zpods::temp_path();
    let pods_path = fs::path(dest_path) / fmt::format("{}-PODS", dir);
    fs::remove_all(pods_path.c_str());

    BackupConfig config;
    config.compress = false;
    config.filter.paths.emplace_back(src_path);
    status = backup(dest_path, config);
    EXPECT_EQ(status, zpods::Status::OK);

    status = zpods::restore(pods_path.c_str(), temp_path());
    EXPECT_EQ(status, Status::OK);
}

TEST(BackupTest, BackupRestoreSoftLink) {
    Status status;
    let dir = "filter";
    let src_path = fmt::format("{}/{}", zpods::test_data_path(), dir);
    let dest_path = zpods::temp_path();
    let pods_path = fs::path(dest_path) / fmt::format("{}-PODS", dir);
    fs::remove_all(pods_path.c_str());

    zpods::BackupConfig config;
    config.compress = false;
    config.filter.types.emplace(zpods::fs::FileType::symlink);
    config.filter.paths.emplace_back(src_path);
    status = backup(dest_path, config);
    EXPECT_EQ(status, zpods::Status::OK);

    status = zpods::restore(pods_path.c_str(), temp_path());
    EXPECT_EQ(status, Status::OK);
}

TEST(BackupTest, BackupRestoreCompression) {
    Status status;
    let dir = "recursive";
    let src_path = fmt::format("{}/{}", zpods::test_data_path(), dir);
    let dest_path = zpods::temp_path();
    let pods_path = fs::path(dest_path) / fmt::format("{}-PODS", dir);
    fs::remove_all(pods_path.c_str());

    BackupConfig config;
    config.compress = true;
    config.filter.paths.emplace_back(src_path);
    status = backup(dest_path, config);
    EXPECT_EQ(status, zpods::Status::OK);

    status = zpods::restore(pods_path.c_str(), temp_path());
    EXPECT_EQ(status, Status::OK);
}

TEST(BackupTest, BackupRestoreCompressionEncryption) {
    let dir = "recursive";
    let src_path = fmt::format("{}/{}", zpods::test_data_path(), dir);
    let dest_path = zpods::temp_path();
    let pods_path = fs::path(dest_path) / fmt::format("{}-PODS", dir);
    fs::remove_all(pods_path.c_str());

    BackupConfig config;
    config.compress = true;
    config.crypto_config = zpods::CryptoConfig("123456");
    config.filter.paths.emplace_back(src_path);

    // backup with password
    EXPECT_EQ(backup(dest_path, config), Status::OK);

    // restore with right password
    EXPECT_EQ(zpods::restore(pods_path.c_str(), temp_path(), config), zpods::Status::OK);

    // restore with wrong password
    config.crypto_config = zpods::CryptoConfig("WrongPassword");
    EXPECT_EQ(zpods::restore(pods_path.c_str(), temp_path(), config), zpods::Status::WRONG_PASSWORD);
}
