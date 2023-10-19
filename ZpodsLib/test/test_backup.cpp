// 测试代码
#include <gtest/gtest.h>
#include <filesystem>
#include "zpods_lib.h"

TEST(BackupTest, BackupRestore) {
    auto src_path = fmt::format("{}/recursive", zpods::test_data_path());
    auto target_path = zpods::temp_path();

    zpods::BackupConfig config;
    config.compress = false;
    let status1 = zpods::backup(target_path, config);
    EXPECT_EQ(status1, zpods::Status::OK);
    let backup_path = fmt::format("{}/{}", zpods::temp_path(), config.backup_filename.value());
    let status2 = zpods::restore(backup_path.c_str(), zpods::temp_path());
    EXPECT_EQ(status2, zpods::Status::OK);
}

TEST(BackupTest, Magic) {
    let src_path = fmt::format("{}/single/man_pthreads.txt", zpods::test_data_path());
    EXPECT_EQ(zpods::restore(src_path.c_str(), zpods::temp_path()),
              zpods::Status::NOT_ZPODS_FILE);
}

TEST(BackupTest, BackupRestoreCompression) {
    auto src_path = fmt::format("{}/recursive", zpods::test_data_path());
    auto target_path = zpods::temp_path();

    zpods::BackupConfig config;
    config.compress = true;
    let status1 = zpods::backup(target_path, config);
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

    // backup with password
    {
        EXPECT_EQ(zpods::backup(target_path, config),
                  zpods::Status::OK);
    }

    let backup_path = fmt::format("{}/{}", zpods::temp_path(), config.backup_filename.value());

    // restore with right password
    {
        EXPECT_EQ(zpods::restore(backup_path.c_str(), zpods::temp_path(), config),
                  zpods::Status::OK);
    }

    // restore with wrong password
    {
        config.crypto_config = zpods::CryptoConfig("WrongPassword");
        EXPECT_EQ(zpods::restore(backup_path.c_str(), zpods::temp_path(), config),
                  zpods::Status::WRONG_PASSWORD);
    }
}
