// 测试代码
#include <gtest/gtest.h>
#include <filesystem>
#include "zpods_lib.h"

TEST(backup_test, repeat_backup_test) {
    auto src_path = zpods::project_path();
    auto target_path = "/home/code4love/tmp/";

    for (int i = 0; i < 10; i++) {
        ASSERT_EQ(zpods::backup(src_path, target_path), zpods::Status::OK);
    }
}