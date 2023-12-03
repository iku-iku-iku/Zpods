//
// Created by code4love on 23-9-23.
//

#ifndef ZPODS_CRYPTO_H
#define ZPODS_CRYPTO_H

#include "pch.h"
#include "config.h"
#include <openssl/types.h>
#include <openssl/evp.h>

namespace zpods {
    /*
     * @brief raw_encrypt the plaintext to ciphertext
     * @param plaintext is the pointer to the source data
     * @param plaintext_len is the size (in bytes) of source data
     * @param key is the pointer to the key
     * @param iv is the pointer to the iv
     * @param ciphertext is the pointer to the destination data
     * @return the size (in bytes) of encrypted string
     * @note the ciphertext must be allocated by caller
     * @note the key and iv must be 32 bytes
     */
    int raw_encrypt(p_cbyte plaintext, int plaintext_len, p_cbyte key, p_cbyte iv, p_byte ciphertext);

    /*
     * @brief C++ style API encapsulation of raw_encrypt for easier use
     */
    auto encrypt(std::string_view plaintext, std::string_view key, std::string_view iv) -> std::optional<std::string>;

    /*
     * @brief encrypt the file
     * @param src_path is the path to the source file
     * @param dst_path is the path to the destination file
     * @return OK if success, otherwise return the error code
     */
    Status encrypt_file(const char *src_path, const char *dst_path, const CryptoConfig& config);

    /*
     * @brief raw_decrypt the ciphertext to plaintext
     * @param ciphertext is the pointer to the source data
     * @param ciphertext_len is the size (in bytes) of source data
     * @param key is the pointer to the key
     * @param iv is the pointer to the iv
     * @param plaintext is the pointer to the destination data
     * @return the size (in bytes) of decrypted string
     * @note the plaintext must be allocated by caller
     */
    int raw_decrypt(p_cbyte ciphertext, int ciphertext_len, p_cbyte key, p_cbyte iv, p_byte plaintext);

    /*
     * @brief C++ style API encapsulation of raw_decrypt for easier use
     */
    auto decrypt(std::string_view ciphertext, std::string_view key, std::string_view iv) -> std::optional<std::string>;

    /*
     * @brief decrypt the file
     * @param src_path is the path to the source file
     * @param dst_path is the path to the destination file
     * @return OK if success, otherwise return the error code
     */
    Status decrypt_file(const char *src_path, const char *dst_path, const CryptoConfig& config);
}

#endif //ZPODS_CRYPTO_H
