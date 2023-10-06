// 测试代码
#include <gtest/gtest.h>
#include <filesystem>
#include "zpods_lib.h"

TEST(BackupTest, BackupRestore) {
    auto src_path = fmt::format("{}/recursive", zpods::test_data_path());
    auto target_path = zpods::temp_path();

    zpods::BackupConfig config;
    config.compress = false;
    let status1 = zpods::backup(src_path.c_str(), target_path, config);
    EXPECT_EQ(status1, zpods::Status::OK);
    let backup_path = fmt::format("{}/{}", zpods::temp_path(), config.backup_filename.value());
    let status2 = zpods::restore(backup_path.c_str(), zpods::temp_path());
    EXPECT_EQ(status2, zpods::Status::OK);
}

TEST(BackupTest, BackupRestoreCompression) {
    auto src_path = fmt::format("{}/recursive", zpods::test_data_path());
    auto target_path = zpods::temp_path();

    zpods::BackupConfig config;
    config.compress = true;
    let status1 = zpods::backup(src_path.c_str(), target_path, config);
    EXPECT_EQ(status1, zpods::Status::OK);
    let backup_path = fmt::format("{}/{}", zpods::temp_path(), config.backup_filename.value());
    let status2 = zpods::restore(backup_path.c_str(), zpods::temp_path());
    EXPECT_EQ(status2, zpods::Status::OK);
}

TEST(BackupTest, BackupRestoreCompressionEncryption) {
    auto src_path = fmt::format("{}/recursive", zpods::test_data_path());
    auto target_path = zpods::temp_path();

    zpods::BackupConfig config;
    config.compress = true;
    config.crypto_config = zpods::CryptoConfig("123456");
    spdlog::info("IV: {} PASSWORD: {}", config.crypto_config->iv_, config.crypto_config->key_);
    let status1 = zpods::backup(src_path.c_str(), target_path, config);
    EXPECT_EQ(status1, zpods::Status::OK);
    let backup_path = fmt::format("{}/{}", zpods::temp_path(), config.backup_filename.value());
    let status2 = zpods::restore(backup_path.c_str(), zpods::temp_path(), config);
    EXPECT_EQ(status2, zpods::Status::OK);
}
