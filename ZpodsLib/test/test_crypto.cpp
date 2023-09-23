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
    std::string plaintext = "Hello, world!";
    std::string key = "01234567890123456789012345678901"; // 请确保密钥长度正确
    std::string iv = "0123456789012345"; // 请确保IV长度正确
    std::string ciphertext;
    std::string decryptedtext;

    // 测试加密
    EXPECT_TRUE(zpods::encrypt(plaintext, key, iv, ciphertext));
    EXPECT_NE(plaintext, ciphertext);

    // 测试解密
    EXPECT_TRUE(zpods::decrypt(ciphertext, key, iv, decryptedtext));
    EXPECT_EQ(plaintext, decryptedtext);
}
