//
// Created by code4love on 23-9-24.
//

#include "gtest/gtest.h"
#include "fs.h"

using namespace zpods;

TEST(FsTest, test_basic) {
    std::string path = test_data_path();
    EXPECT_TRUE(fs::is_directory(path));
    EXPECT_FALSE(fs::is_directory(path + "/single/man_pthreads.txt"));
    EXPECT_STREQ(fs::relative("/home/a.txt", "/home"), "a.txt");
    EXPECT_STREQ(fs::relative("/home/a.txt", "/home/"), "a.txt");
    EXPECT_STREQ(fs::relative("/home/a.txt", "/home/a.txt"), "a.txt");
    EXPECT_STREQ(fs::relative("a.txt", "a.txt"), "a.txt");
    EXPECT_STREQ(fs::relative("/home/a.txt", "/opt"), nullptr);
}

TEST(FsTest, test_iterate)
{
    spdlog::info("{}", project_path());
    for (const auto& path: fs::FileCollector(project_path()) ){
        spdlog::info("{}", path.string());
    }
}