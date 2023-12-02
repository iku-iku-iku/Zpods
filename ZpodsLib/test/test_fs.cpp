//
// Created by code4love on 23-9-24.
//

#include "gtest/gtest.h"
#include "fs.h"
#include "zpods_lib.h"

using namespace zpods;
using namespace zpods::fs;

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

TEST(FsTest, DeltaBackupTest) {
    let file_path = path(test_data_path()) / path("tiny/a.txt");
    EXPECT_EQ(2, get_file_family(file_path.c_str()).size());
}

auto filtered_paths_expect_eq(std::unordered_set<std::string> expected_paths, fs::zpath path, fs::FilesFilter filter) {
    let_mut iter = fs::FileCollector(path, std::move(filter)).paths();
    let filtered_paths = std::unordered_set(iter.begin(), iter.end());
    for (const auto &item: filtered_paths) {
        spdlog::info("file: {}", item.filename().c_str());
        EXPECT_TRUE(expected_paths.contains(item.filename()));
    }
    EXPECT_EQ(expected_paths.size(), filtered_paths.size());
}

// default filter
TEST(FsTest, FileCollectorWithDefaultFilter) {
    filtered_paths_expect_eq({}, zpods::fs::zpath(), fs::FilesFilter{});
}
//
//// filter with paths
//TEST(FsTest, FileCollectorWithPaths) {
//    let path1 = fs::path(test_data_path()) / fs::path("filter") / fs::path("dir2");
//    let path2 = path1 / fs::path("sub");
//    filtered_paths_expect_eq(
//            {"empty_file", "main.c"}, zpods::fs::zpath(),
//            fs::FilesFilter{
//                    .paths = {path1, path2},
//            });
//}

// filter with types
TEST(FsTest, FileCollectorWithTypes) {
    let filter_path = fs::path(test_data_path()) / fs::path("filter");
    filtered_paths_expect_eq(
            {"soft_lint_to_man_fork.txt"}, filter_path,
            fs::FilesFilter{
                    .types = {fs::FileType::symlink}
            });
}

// filter with size
TEST(FsTest, FileCollectorWithSize) {
    let filter_path = fs::path(test_data_path()) / fs::path("filter");
    filtered_paths_expect_eq(
            {"main.c", "man_printf.txt"}, filter_path,
            fs::FilesFilter{
                    .types = {fs::FileType::regular},
                    .min_size = 20,
                    .max_size = 2.8_KB,
            });
}

TEST(FsTest, FileCollectorWithNames) {
    {
        let regex = std::regex("abc");
        EXPECT_TRUE(std::regex_search("abcd", regex));
        EXPECT_FALSE(std::regex_search("abdc", regex));
    }
    {
        let regex = std::regex("a.*c");
        EXPECT_TRUE(std::regex_search("abcd", regex));
        EXPECT_TRUE(std::regex_search("abdc", regex));
    }

    {
        let filter_path = fs::path(test_data_path()) / fs::path("filter");
        filtered_paths_expect_eq(
                {"man_printf.txt", "man_fork.txt", "hard_link_to_man_fork.txt"}, filter_path,
                {
                        .types = {fs::FileType::regular},
                        .re_list = {"man"}
                }
        );
    }
}


TEST(FsTest, Inotify) {
//    let test_path = test_data_path();
//    let tmp_path = temp_path();
//    let callback = [&](const char *path) {
//        zpods::BackupConfig config;
//        config.compress = true;
//        config.crypto_config = zpods::CryptoConfig("123456");
//        zpods::backup(tmp_path, config);
//    };
//    FsWatcher watcher(test_path, {
//            .on_file_create = callback,
//            .on_file_delete = callback,
//            .on_file_modify = callback,
//            .on_dir_create = callback,
//            .on_dir_delete = callback,
//    });
}