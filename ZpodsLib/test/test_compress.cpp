//
// Created by code4love on 23-9-20.
//

#include <gtest/gtest.h>
#include <cstring>
#include "compress.h"

TEST(CompressTest, SimpleCompressDecompress1) {
    zpods::byte src[] = "TOBEORNOTTOBE" \
                 "TOBEORNOTTOBE" \
                 "TOBEORNOTTOBE" \
                 "TOBEORNOTTOBE";


    size_t src_size = std::strlen((const char*)src);

    std::unique_ptr<zpods::byte[]> dst;

    auto dst_size = zpods::compress(src, src_size, dst);
    ASSERT_LT(dst_size, src_size);

    auto back_size = zpods::decompress(dst.get(), dst);
    ASSERT_EQ(back_size, src_size);
}
