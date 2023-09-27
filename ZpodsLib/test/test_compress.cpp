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

    size_t src_size = std::strlen((const char *) src);

    auto [dst_size, compressed] = zpods::compress(src, src_size);
    ASSERT_LT(dst_size, src_size);

    auto [back_size, decompressed] = zpods::decompress(compressed.get());
    ASSERT_EQ(back_size, src_size);
}

TEST(CompressTest, HugeRandomCompressDecompress1) {
    let N = 1 << 10;
    zpods::byte src[N];
    for (auto &i: src) {
        i = rand() % 256;
    }

    auto [dst_size, compressed] = zpods::compress(src, N);
    ASSERT_LT(dst_size, N);

    auto [back_size, decompressed] = zpods::decompress(compressed.get());
    ASSERT_EQ(back_size, N);
}
