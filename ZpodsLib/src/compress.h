//
// Created by code4love on 23-9-20.
//

#ifndef ZPODS_COMPRESS_H
#define ZPODS_COMPRESS_H

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
     * @param dst is the pointer to the destination data
     * @return the size (in bytes) of compressed string
     */
    size_t compress(const char *src, size_t src_size, char *dst);

    /*
     * @brief decompress the src to dst
     * @param src is the pointer to the source data
     * @param dst is the pointer to the destination data
     * @return the size (in bytes) of decompressed string
     */
    size_t decompress(const char *src, char *dst);
}

#endif //ZPODS_COMPRESS_H
