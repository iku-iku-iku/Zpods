// 测试代码
#include <gtest/gtest.h>
#include <filesystem>
#include "zpods_lib.h"

TEST(BackupTest, backup_restore) {
    auto src_path = fmt::format("{}/recursive", zpods::test_data_path());
    auto target_path = zpods::temp_path();

//    for (int i = 0; i < 10; i++) {
    EXPECT_EQ(zpods::backup(src_path.c_str(), target_path), zpods::Status::OK);
//    }
    EXPECT_EQ(zpods::restore(fmt::format("{}/archive", zpods::temp_path()).c_str(), zpods::temp_path()), zpods::Status::OK);
}
