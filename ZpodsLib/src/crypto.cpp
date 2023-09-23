//
// Created by code4love on 23-9-23.
//

#include "crypto.h"

int zpods::raw_encrypt(p_cbyte plaintext, int plaintext_len, p_cbyte key, p_cbyte iv, p_byte ciphertext) {
    EVP_CIPHER_CTX *ctx;
    int len;
    int ciphertext_len;

    if (!(ctx = EVP_CIPHER_CTX_new()))
        return -1;

    if (1 != EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), nullptr, key, iv))
        return -1;

    if (1 != EVP_EncryptUpdate(ctx, ciphertext, &len, plaintext, plaintext_len))
        return -1;
    ciphertext_len = len;

    if (1 != EVP_EncryptFinal_ex(ctx, ciphertext + len, &len))
        return -1;
    ciphertext_len += len;

    EVP_CIPHER_CTX_free(ctx);

    return ciphertext_len;
}

int zpods::raw_decrypt(p_cbyte ciphertext, int ciphertext_len, p_cbyte key, p_cbyte iv, p_byte plaintext) {
    EVP_CIPHER_CTX *ctx;
    int len;
    int plaintext_len;

    if (!(ctx = EVP_CIPHER_CTX_new()))
        return -1;

    if (1 != EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv))
        return -1;

    if (1 != EVP_DecryptUpdate(ctx, plaintext, &len, ciphertext, ciphertext_len))
        return -1;
    plaintext_len = len;

    if (1 != EVP_DecryptFinal_ex(ctx, plaintext + len, &len))
        return -1;
    plaintext_len += len;

    EVP_CIPHER_CTX_free(ctx);

    return plaintext_len;
}

bool
zpods::encrypt(const std::string &plaintext, const std::string &key, const std::string &iv, std::string &ciphertext) {
    int plaintext_len = (int) plaintext.size();
    let block_size = 16; // 假设块大小为16字节
    let padding = block_size - (plaintext_len % block_size);
    int ciphertext_len = (int) (plaintext_len + padding);

    std::vector<unsigned char> cipher_buf(ciphertext_len);

    if (!(ciphertext_len = raw_encrypt(reinterpret_cast<p_cbyte>(plaintext.c_str()),
                                       plaintext_len,
                                       reinterpret_cast<p_cbyte>(key.c_str()),
                                       reinterpret_cast<p_cbyte>(iv.c_str()),
                                       &cipher_buf[0]))) {
        return false;
    }

    ciphertext.assign(cipher_buf.begin(), cipher_buf.begin() + ciphertext_len);
    return true;
}

bool
zpods::decrypt(ref<std::string> ciphertext, ref<std::string> key, ref<std::string> iv, ref_mut<std::string> plaintext) {
    int ciphertext_len = (int) ciphertext.size();

    std::vector<unsigned char> plain_buf(ciphertext_len);
    int plaintext_len;

    if (!(plaintext_len = raw_decrypt(reinterpret_cast<p_cbyte>(ciphertext.c_str()),
                                      ciphertext_len,
                                      reinterpret_cast<p_cbyte>(key.c_str()),
                                      reinterpret_cast<p_cbyte>(iv.c_str()),
                                      &plain_buf[0]))) {
        return false;
    }

    plaintext.assign(plain_buf.begin(), plain_buf.begin() + plaintext_len);
    return true;
}

