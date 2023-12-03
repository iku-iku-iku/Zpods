//
// Created by code4love on 23-9-23.
//

#include "ZpodsLib/src/base/crypto.h"
#include "fs.h"
#include "ZpodsLib/src/core/config.h"
#include <openssl/err.h>

using namespace zpods;

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
    {
        let hash = std::hash<std::string_view>()({(char *) ciphertext, (size_t) ciphertext_len});
        spdlog::info("CIPHER SIZE: {} HASH: {}", ciphertext_len, hash);
    }

    EVP_CIPHER_CTX *ctx;
    int len;
    int plaintext_len;

    if (!(ctx = EVP_CIPHER_CTX_new()))
        return -1;

    if (1 != EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), nullptr, key, iv))
        return -1;

    if (1 != EVP_DecryptUpdate(ctx, plaintext, &len, ciphertext, ciphertext_len))
        return -1;
    plaintext_len = len;

    if (1 != EVP_DecryptFinal_ex(ctx, plaintext + len, &len)) {
        ERR_print_errors_fp(stderr);
        return -1;
    }
    plaintext_len += len;

    EVP_CIPHER_CTX_free(ctx);

    return plaintext_len;
}

auto
zpods::encrypt(std::string_view plaintext, std::string_view key, std::string_view iv) -> std::optional<std::string> {
    let plaintext_len = (int) plaintext.size();
    let block_size = 16; // 假设块大小为16字节
    let padding = block_size - (plaintext_len % block_size);
    let_mut ciphertext_len = (int) (plaintext_len + padding);

    std::vector<byte> cipher_buf(ciphertext_len);

    if (!(ciphertext_len = raw_encrypt(reinterpret_cast<p_cbyte>(plaintext.data()),
                                       plaintext_len,
                                       reinterpret_cast<p_cbyte>(key.data()),
                                       reinterpret_cast<p_cbyte>(iv.data()),
                                       &cipher_buf[0]))) {
        return {};
    }

    return std::string(cipher_buf.begin(), cipher_buf.begin() + ciphertext_len);
}

auto zpods::decrypt(std::string_view ciphertext, std::string_view key,
                    std::string_view iv) -> std::optional<std::string> {
    int ciphertext_len = (int) ciphertext.size();

    std::vector<unsigned char> plain_buf(ciphertext_len);
    int plaintext_len;

    if (!(plaintext_len = raw_decrypt(reinterpret_cast<p_cbyte>(ciphertext.data()),
                                      ciphertext_len,
                                      reinterpret_cast<p_cbyte>(key.data()),
                                      reinterpret_cast<p_cbyte>(iv.data()),
                                      &plain_buf[0]))) {
        return {};
    }

    return std::string(plain_buf.begin(), plain_buf.begin() + plaintext_len);
}

zpods::Status zpods::encrypt_file(const char *src_path, const char *dst_path, const CryptoConfig& config) {
    let_mut ifs = fs::open_or_create_file_as_ifs(src_path, fs::ios::binary);
    if (!ifs.is_open()) {
        spdlog::error("cannot open file: {}", src_path);
        return Status::ERROR;
    }

    let src_size = fs::get_file_size(src_path);
    std::vector<byte> src(src_size);
    ifs.read((char *) src.data(), src_size);

    std::string_view src_view((const char *) src.data(), src_size);
    let cipher = encrypt(src_view, config.key_, config.iv_);
    let hash = std::hash<std::string_view>()({cipher->data(), cipher->size()});
    spdlog::info("CIPHER SIZE: {} HASH: {}", cipher->size(), hash);
    ZPODS_ASSERT(cipher.has_value());

    let_mut ofs = fs::open_or_create_file_as_ofs(dst_path, fs::ios::binary);
    if (!ofs.is_open()) {
        spdlog::error("cannot open file: {}", dst_path);
        return Status::ERROR;
    }

    ofs.write((const char *) cipher->c_str(), (long) cipher->size());

    return Status::OK;
}

Status zpods::decrypt_file(const char *src_path, const char *dst_path, const CryptoConfig& config) {
    let_mut ifs = fs::open_or_create_file_as_ifs(src_path, std::ios::binary);
    if (!ifs.is_open()) {
        spdlog::error("cannot open file: {}", src_path);
        return Status::ERROR;
    }

    let src_size = fs::get_file_size(src_path);
    std::vector<byte> src(src_size);
    ifs.read((char *) src.data(), src_size);

    std::string_view src_view((const char *) src.data(), src_size);

    let plain_text = decrypt(src_view, config.key_, config.iv_);

    let_mut ofs = fs::open_or_create_file_as_ofs(dst_path, std::ios::binary);
    if (!ofs.is_open()) {
        spdlog::error("cannot open file: {}", dst_path);
        return Status::ERROR;
    }

    ofs.write((const char *) plain_text->c_str(), (long) plain_text->size());

    return Status::OK;
}

