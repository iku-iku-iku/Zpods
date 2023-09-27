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

    spdlog::info("src_size: {}, dst_size: {}", src_size, dst_size);

    auto [back_size, decompressed] = zpods::decompress(compressed.get());
    ASSERT_EQ(back_size, src_size);
}

TEST(CompressTest, SimpleCompressDecompress2) {
    zpods::byte src[] = "TOBEORNOTTOBE" \
                 "TOBEORNOTTOBE" \
                 "TOBEORNOTTOBE" \
                 "TOBEORNOTTOBE";

    size_t src_size = std::strlen((const char *) src);

    auto [dst_size, compressed] = zpods::compress<zpods::LZ77 | zpods::Huffman>(src, src_size);
//    ASSERT_LT(dst_size, src_size);
//
    spdlog::info("src_size: {}, dst_size: {}", src_size, dst_size);
//
    auto [back_size, decompressed] = zpods::decompress<zpods::LZ77 | zpods::Huffman>(compressed.get());
    ASSERT_EQ(back_size, src_size);
}

TEST(CompressTest, TestHuffman) {
    zpods::byte src[] = "AABC";
    let_mut dict = zpods::make_huffman_dictionary(std::span(src, 4));
//    for (const auto &[val, code]: dict) {
//        spdlog::info("{:c} -> {:{}b}", val, code.bits, code.len);
//    }
    ASSERT_TRUE(dict['A'].bits == 0b0 && dict['A'].len == 1);
    ASSERT_TRUE(dict['B'].bits == 0b11 && dict['B'].len == 2);
    ASSERT_TRUE(dict['C'].bits == 0b10 && dict['C'].len == 2);
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
