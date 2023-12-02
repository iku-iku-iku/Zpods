// 测试代码
#include <gtest/gtest.h>
#include <filesystem>
#include "zpods_lib.h"

TEST(BackupTest, BackupRestore) {
    auto src_path = fmt::format("{}/recursive", zpods::test_data_path());
    auto target_path = zpods::temp_path();

    zpods::BackupConfig config;
    config.compress = false;
    config.filter.paths.emplace_back(src_path);
    let status1 = zpods::backup(target_path, config);
    EXPECT_EQ(status1, zpods::Status::OK);
    let backup_path = fmt::format("{}/{}", zpods::temp_path(), config.archive_path.value());
    let status2 = zpods::restore(backup_path.c_str(), zpods::temp_path());
    EXPECT_EQ(status2, zpods::Status::OK);
}

TEST(BackupTest, BackupRestoreMultiPath) {
    auto src_path1 = fmt::format("{}/recursive", zpods::test_data_path());
    auto src_path2 = fmt::format("{}/middle", zpods::test_data_path());
    auto target_path = zpods::temp_path();

    zpods::BackupConfig config;
    config.compress = false;
    config.filter.paths.emplace_back(src_path1);
    config.filter.paths.emplace_back(src_path2);
    let status1 = zpods::backup(target_path, config);
    EXPECT_EQ(status1, zpods::Status::OK);
    let backup_path = fmt::format("{}/{}", zpods::temp_path(), config.archive_path.value());
    let status2 = zpods::restore(backup_path.c_str(), zpods::temp_path());
    EXPECT_EQ(status2, zpods::Status::OK);
}

TEST(BackupTest, BackupRestoreSoftLink) {
    auto src_path = fmt::format("{}/filter", zpods::test_data_path());
    auto target_path = zpods::temp_path();

    zpods::BackupConfig config;
    config.compress = false;
    config.filter.types.emplace(zpods::fs::FileType::symlink);
    config.filter.paths.emplace_back(src_path);
    let status1 = zpods::backup(target_path, config);
    EXPECT_EQ(status1, zpods::Status::OK);
    let backup_path = fmt::format("{}/{}", zpods::temp_path(), config.archive_path.value());
    let status2 = zpods::restore(backup_path.c_str(), zpods::temp_path());
    EXPECT_EQ(status2, zpods::Status::OK);
}

TEST(BackupTest, Magic) {
    // read the magic number to check whether it is a zpods file
    let src_path = fmt::format("{}/single/man_pthreads.txt", zpods::test_data_path());
    EXPECT_EQ(zpods::restore(src_path.c_str(), zpods::temp_path()),
              zpods::Status::NOT_ZPODS_FILE);
}

TEST(BackupTest, BackupRestoreCompression) {
    auto src_path = fmt::format("{}/recursive", zpods::test_data_path());
    auto target_path = zpods::temp_path();

    zpods::BackupConfig config;
    config.compress = true;
    config.filter.paths.emplace_back(src_path);
    let status1 = zpods::backup(target_path, config);
    EXPECT_EQ(status1, zpods::Status::OK);
    let backup_path = fmt::format("{}/{}", zpods::temp_path(), config.archive_path.value());
    let status2 = zpods::restore(backup_path.c_str(), zpods::temp_path());
    EXPECT_EQ(status2, zpods::Status::OK);
}

TEST(BackupTest, BackupRestoreCompressionEncryption) {
    auto src_path = fmt::format("{}/recursive", zpods::test_data_path());
    auto target_path = zpods::temp_path();

    zpods::BackupConfig config;
    config.compress = true;
    config.crypto_config = zpods::CryptoConfig("123456");
    config.filter.paths.emplace_back(src_path);

    // backup with password
    {
        EXPECT_EQ(zpods::backup(target_path, config),
                  zpods::Status::OK);
    }

    let backup_path = fmt::format("{}/{}", zpods::temp_path(), config.archive_path.value());

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
