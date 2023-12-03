//
// Created by code4love on 23-10-9.
//

#include "gtest/gtest.h"
#include "ZpodsLib/src/network/network.h"

using namespace zpods;

//TEST(NetworkTest, UserTest) {
//    User user;
//    user.username = "wyl";
//    user.password = "123";
//    EXPECT_NE(user.unregister(), Status::ERROR);
//    EXPECT_EQ(user.login(), Status::USER_NOT_EXISTS);
//    EXPECT_EQ(user.register_(), Status::OK);
//    EXPECT_EQ(user.login(), Status::OK);
//    user.password = "1234";
//    EXPECT_EQ(user.login(), Status::WRONG_PASSWORD);
//}
//
//TEST(NetworTest, FileUploadTest) {
//    User user;
//    user.username = "wyl";
//    user.password = "123";
//    EXPECT_EQ(user.login(), Status::OK);
//
//    let test_path = zpods::test_data_path();
//    let file_path = fmt::format("{}/{}", test_path, "single/man_pthreads.txt");
//    spdlog::info("UPLOAD {}", file_path);
//
//    EXPECT_EQ(user.upload_file(file_path.c_str()), Status::OK);
//}
//
//TEST(NetworTest, FileDownloadTest) {
//    User user;
//    user.username = "wyl";
//    user.password = "123";
//    EXPECT_EQ(user.login(), Status::OK);
//
//    let temp_path = zpods::temp_path();
//    let local_dir = fmt::format("{}/{}", temp_path, "single");
//    EXPECT_EQ(user.download_file("man_pthreads.txt", local_dir.c_str()), Status::OK);
//}
