// 测试代码
#include <gtest/gtest.h>
#include <filesystem>
#include "zpods_lib.h"
#include "zpods_core.h"

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

TEST(BackupTest, DeltaBackup) {
    let dir = "recursive";
    let src_path = fs::path(zpods::test_data_path()) / dir;
    let dest_path = zpods::temp_path();
    let pods_path = fs::path(dest_path) / fmt::format("{}-PODS", dir);
    let some_file_path = src_path / "single/man_pthreads.txt";
    let new_file = src_path / "single/new_file.txt";
    fs::remove_file(new_file.c_str());
    fs::remove_all(pods_path.c_str());

    BackupConfig config;
    config.compress = true;
    config.crypto_config = CryptoConfig("123456");
    config.filter.paths.emplace_back(src_path);

    // backup with password
    EXPECT_EQ(backup(dest_path, config), Status::OK);

    // restore with right password
    EXPECT_EQ(zpods::restore(pods_path.c_str(), temp_path(), config), zpods::Status::OK);

    // sleep one second the make the timestamp different
    sleep(1);
    // === start test of delta backup ===
    // ***create a new file***
    fs::copy(some_file_path.c_str(), new_file.c_str(), fs::copy_options::skip_existing);
    ASSERT_TRUE(fs::exists(new_file.c_str()));
    // do a delta backup
    EXPECT_EQ(backup(dest_path, config), Status::OK);
    let_mut paths = fs::FileCollector(pods_path, {}).paths();
    std::sort(paths.begin(), paths.end());
    EXPECT_EQ(paths.size(), 2);
    // since we add a file which is in original files, if delta backup works, the size of the new file should be smaller
    // if we simply back up all files, the size of the new backup file should be larger
    EXPECT_GT(fs::get_file_size(paths[0]), fs::get_file_size(paths[1]));
    // restore with right password
    EXPECT_EQ(zpods::restore(pods_path.c_str(), temp_path(), config), zpods::Status::OK);
    let restored_new_file_path = fs::path(temp_path()) / "single/new_file.txt";
    EXPECT_TRUE(fs::exists(restored_new_file_path.c_str()));

    // ***update the file***
    // we must sleep one second to make the last_modified_ts different
    sleep(1);
    std::ofstream ofs(new_file);
    ofs << "hello world";
    ofs.close();

    // do a delta backup
    EXPECT_EQ(backup(dest_path, config), Status::OK);
    paths = fs::FileCollector(pods_path, {}).paths();
    std::sort(paths.begin(), paths.end());
    EXPECT_EQ(paths.size(), 3);
    // since we only update one file, if delta backup works, the size of the new backup file should be smaller
    // if we simply back up all files, the size of the new backup file should be larger
    EXPECT_GT(fs::get_file_size(paths[1]), fs::get_file_size(paths[2]));
    // restore with right password
    EXPECT_EQ(zpods::restore(pods_path.c_str(), temp_path(), config), zpods::Status::OK);
    EXPECT_TRUE(fs::exists(restored_new_file_path.c_str()));
    EXPECT_EQ("hello world", fs::read_from_file(restored_new_file_path.c_str()));

    // ***and the last, delete the new file***
    let ts_before_remove_file = get_current_timestamp();
    // sleep one second to make the ts different
    sleep(1);
    ASSERT_TRUE(fs::remove_file(new_file.c_str()));
    // do a delta backup
    EXPECT_EQ(backup(dest_path, config), Status::OK);
    paths = fs::FileCollector(pods_path, {}).paths();
    std::sort(paths.begin(), paths.end());
    EXPECT_EQ(paths.size(), 4);
    // since we only delete one file, if delta backup works (only mark the deletion in header),
    // the size of the new backup file should be smaller
    // if we simply back up all files, the size of the new backup file should be larger
    EXPECT_GT(fs::get_file_size(paths[2]), fs::get_file_size(paths[3]));
    // for simplicity, we do not automatically delete the file in the restore process, so we must manually delete it
    fs::remove_file(restored_new_file_path.c_str());
    EXPECT_EQ(restore(pods_path.c_str(), temp_path(), config), zpods::Status::OK);
    // if the delta backup works, we will not get it back
    EXPECT_FALSE(fs::exists(restored_new_file_path.c_str()));

    // ***because we have implemented delta backup, we can restore from a given timestamp! ***
    config.timestamp = ts_before_remove_file;
    EXPECT_EQ(restore(pods_path.c_str(), temp_path(), config), Status::OK);
    EXPECT_TRUE(fs::exists(restored_new_file_path.c_str()));
    // Amazing! We had done a time travel to get it back!
}