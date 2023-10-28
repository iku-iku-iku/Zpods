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

auto filtered_paths_expect_eq(std::unordered_set<std::string> paths, fs::FilesFilter filter) {
    let_mut iter = fs::FileCollector(std::move(filter));
    let filtered_paths = std::unordered_set(iter.begin(), iter.end());
    for (const auto &item: filtered_paths) {
        spdlog::info("file: {}", item.filename().c_str());
        EXPECT_TRUE(paths.contains(item.filename()));
    }
    EXPECT_EQ(paths.size(), filtered_paths.size());
}

// default filter
TEST(FsTest, FileCollectorWithDefaultFilter) {
    filtered_paths_expect_eq({}, fs::FilesFilter{});
}

// filter with paths
TEST(FsTest, FileCollectorWithPaths) {
    let path1 = fs::path(test_data_path()) / fs::path("filter") / fs::path("dir2");
    let path2 = path1 / fs::path("sub");
    filtered_paths_expect_eq(
            {"empty_file", "main.c"},
            fs::FilesFilter{
                    .paths = {path1, path2},
            });
}

// filter with types
TEST(FsTest, FileCollectorWithTypes) {
    let filter_path = fs::path(test_data_path()) / fs::path("filter");
    filtered_paths_expect_eq(
            {"soft_lint_to_man_fork.txt"},
            fs::FilesFilter{
                    .paths = {filter_path},
                    .types = {fs::FileType::symlink}
            });
}

// filter with size
TEST(FsTest, FileCollectorWithSize) {
    let filter_path = fs::path(test_data_path()) / fs::path("filter");
    filtered_paths_expect_eq(
            {"main.c", "man_printf.txt"},
            fs::FilesFilter{
                    .paths = {filter_path},
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
                {"man_printf.txt", "man_fork.txt", "hard_link_to_man_fork.txt"},
                {
                        .paths = {filter_path},
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