//
// Created by code4love on 23-9-24.
//

#include "gtest/gtest.h"
#include "fs.h"
#include "zpods_lib.h"

using namespace zpods;

TEST(FsTest, BasicTest) {
    std::string path = test_data_path();
    EXPECT_TRUE(fs::is_directory(path));
    EXPECT_FALSE(fs::is_directory(path + "/single/man_pthreads.txt"));
    EXPECT_STREQ(fs::relative("/home/a.txt", "/home"), "a.txt");
    EXPECT_STREQ(fs::relative("/home/a.txt", "/home/"), "a.txt");
    EXPECT_DEATH(fs::relative("a.txt", "a.txt"), "Assertion failed: `is_directory\\(base\\)`");
    EXPECT_STREQ(fs::relative("/home/a.txt", "/opt"), nullptr);
    EXPECT_STREQ(fs::get_base_name("/home/a.txt").c_str(), "/home");
    EXPECT_TRUE(fs::exists("/home"));
    EXPECT_FALSE(fs::exists("/me"));
}

TEST(FsTest, FileCollector) {
    spdlog::info("{}", project_path());
    for (const auto &path: fs::FileCollector(project_path())) {
        spdlog::info("{}", path.string());
    }
}

TEST(FsTest, Inotify) {
    let test_path = test_data_path();
    let tmp_path = temp_path();
    let callback = [&](const char *path) {
        zpods::BackupConfig config;
        config.compress = true;
        config.crypto_config = zpods::CryptoConfig("123456");
        zpods::backup(test_path, tmp_path, config);
    };
    FsWatcher watcher(test_path, {
            .on_file_create = callback,
            .on_file_delete = callback,
            .on_file_modify = callback,
            .on_dir_create = callback,
            .on_dir_delete = callback,
    });
}