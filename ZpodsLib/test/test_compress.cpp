//
// Created by code4love on 23-9-20.
//

#include <gtest/gtest.h>
#include <cstring>
#include "compress.h"

TEST(CompressTest, SimpleCompressDecompress1) {
    char src[] = "TOBEORNOTTOBE" \
                 "TOBEORNOTTOBE" \
                 "TOBEORNOTTOBE" \
                 "TOBEORNOTTOBE";
    char dst[100] = {0};

    size_t src_size = std::strlen(src);

    auto dst_size = zpods::compress(src, src_size, dst);
    ASSERT_LT(dst_size, src_size);

    auto back_size = zpods::decompress(dst, src);
    ASSERT_EQ(back_size, src_size);
}
