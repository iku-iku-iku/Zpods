//
// Created by code4love on 23-9-23.
//

#include <gtest/gtest.h>
#include "crypto.h"

TEST(CryptoTest, EncryptDecrypt1) {
    unsigned char key[] = "01234567890123456789012345678901";
    unsigned char iv[] = "0123456789012345";
    unsigned char plaintext[] = "Hello, world!";
    unsigned char ciphertext[128];
    unsigned char decryptedtext[128];
    int decryptedtext_len, ciphertext_len;

    // 加密
    ciphertext_len = zpods::raw_encrypt(plaintext, (int) strlen((char *) plaintext), key, iv, ciphertext);
    ASSERT_GT(ciphertext_len, 0);

    // 解密
    decryptedtext_len = zpods::raw_decrypt(ciphertext, ciphertext_len, key, iv, decryptedtext);
    ASSERT_GT(decryptedtext_len, 0);

    decryptedtext[decryptedtext_len] = '\0';

    // 验证解密后的文本是否与原始明文相同
    ASSERT_STREQ((char *) decryptedtext, (char *) plaintext);
}

TEST(CryptoTest, EncryptDecrypt2) {
    std::string plaintext = "012345678912345";
    std::string key = std::to_string(rand());
    key.resize(32);
    std::string iv = std::to_string(rand());
    iv.resize(16);

    // 测试加密
    let cipher = zpods::encrypt(plaintext, key, iv);
    EXPECT_TRUE(cipher.has_value());
    EXPECT_NE(plaintext, cipher);

    // 测试解密
    let decrypt = zpods::decrypt(*cipher, key, iv);
    EXPECT_TRUE(decrypt.has_value());
    EXPECT_EQ(plaintext, decrypt);
}

TEST(CryptoTest, EncrpytDecrptFile) {
    std::string src_path = fmt::format("{}/{}", zpods::test_data_path(), "single/man_pthreads.txt");
    std::string dst_path = fmt::format("{}/{}", zpods::temp_path(), "single/man_pthreads.txt");
    std::string key = std::to_string(rand());

    let config = zpods::CryptoConfig(key);
    // 测试加密
    {
        let status = zpods::encrypt_file(src_path.c_str(), dst_path.c_str(), config);
        EXPECT_EQ(status, zpods::Status::OK);
    }

    // 验证加密后的文件是否与原始文件不同
    EXPECT_FALSE(zpods::fs::is_same_content(src_path.c_str(), dst_path.c_str()));
    // 测试解密
    {
        let status = zpods::decrypt_file(dst_path.c_str(), dst_path.c_str(), config);
        EXPECT_EQ(status, zpods::Status::OK);
    }
    // 验证解密后的文件是否与原始文件相同
    EXPECT_TRUE(zpods::fs::is_same_content(src_path.c_str(), dst_path.c_str()));
}