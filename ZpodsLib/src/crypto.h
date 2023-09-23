//
// Created by code4love on 23-9-23.
//

#ifndef ZPODS_CRYPTO_H
#define ZPODS_CRYPTO_H

#include "pch.h"
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
    bool encrypt(ref<std::string> plaintext, ref<std::string> key, ref<std::string> iv,
                 ref_mut<std::string> ciphertext);

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
    bool decrypt(ref<std::string> ciphertext, ref<std::string> key, ref<std::string> iv,
                 ref_mut<std::string> plaintext);
}

#endif //ZPODS_CRYPTO_H
