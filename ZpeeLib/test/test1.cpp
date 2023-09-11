// 测试代码
#include <gtest/gtest.h>
#include "library.h"

TEST(addTest, HandlesPositiveNumbers) {
  EXPECT_EQ(3, add(1, 2));
  EXPECT_EQ(5, add(2, 3));
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}