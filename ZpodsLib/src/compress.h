//
// Created by code4love on 23-9-20.
//

#ifndef ZPODS_COMPRESS_H
#define ZPODS_COMPRESS_H

#include "pch.h"
#include "bit_ops.h"

namespace zpods {
    constexpr static char eof_literal = -1;

    constexpr static size_t OFFSET_BITS = 15;
    constexpr static size_t LENGTH_BITS = 5;
    constexpr static size_t LITERAL_BITS = 8;

    constexpr static size_t OL_CODE = OFFSET_BITS + LENGTH_BITS;
    constexpr static size_t OLL_CODE = OL_CODE + LITERAL_BITS;

    constexpr static size_t SEARCH_WINDOW_SIZE = 1 << OFFSET_BITS;
    constexpr static size_t LOOK_AHEAD_WINDOW_SIZE = 1 << LENGTH_BITS;

    /*
     * @brief compress the src to dst
     * @param src is the pointer to the source data
     * @param src_size is the size (in bytes) of source data
     * @return the size (in bytes) of compressed string and the pointer to the compressed data
     */
    std::pair<size_t, std::unique_ptr<byte[]>> compress(p_cbyte src, size_t src_size);

    /*
     * @brief decompress the src to dst
     * @param src is the pointer to the source data
     * @return the size (in bytes) of decompressed string and the pointer to the decompressed data
     */
    std::pair<size_t, std::unique_ptr<byte[]>> decompress(p_cbyte src);
}

#endif //ZPODS_COMPRESS_H
