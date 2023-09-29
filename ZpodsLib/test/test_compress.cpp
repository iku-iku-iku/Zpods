//
// Created by code4love on 23-9-20.
//

#include <gtest/gtest.h>
#include <cstring>
#include "compress.h"

TEST(CompressTest, SimpleCompressDecompress1) {
//    spdlog::set_level(spdlog::level::debug);
    zpods::byte src[] =
            "TOBEORNOTTOBE" \
                 "TOBEORNOTTOBE" \
                 "TOBEORNOTTOBE" \
                 "TOBEORNOTTOBE" \
                 "TOBEORNOTTOBE" \
                 "TOBEORNOTTOBE" \
                 "TOBEORNOTTOBE" \
                 "TOBEORNOTTOBE" \
                 "TOBEORNOTTOBE" \
                 "TOBEORNOTTOBE" \
                 "TOBEORNOTTOBE" \
                 "TOBEORNOTTOBE";

    size_t src_size = std::strlen((const char *) src);

    auto [dst_size, compressed] = zpods::compress(std::span(src, src_size));
    ASSERT_LT(dst_size, src_size);

    spdlog::info("src_size: {}, dst_size: {}", src_size, dst_size);

    auto [back_size, decompressed] = zpods::decompress(compressed.get());
    ASSERT_EQ(back_size, src_size);
}

TEST(CompressTest, SimpleCompressDecompress2) {
    zpods::byte src[] =
            "TOBEORNOTTOBE" \
                 "TOBEORNOTTOBE" \
                 "TOBEORNOTTOBE" \
                 "TOBEORNOTTOBE" \
                 "TOBEORNOTTOBE" \
                 "TOBEORNOTTOBE" \
                 "TOBEORNOTTOBE" \
                 "TOBEORNOTTOBE" \
                 "TOBEORNOTTOBE" \
                 "TOBEORNOTTOBE" \
                 "TOBEORNOTTOBE" \
                 "TOBEORNOTTOBE";

    size_t src_size = std::strlen((const char *) src);

    auto [dst_size, compressed] = zpods::compress<zpods::LZ77 | zpods::Huffman>(std::span(src, src_size));
    ASSERT_LT(dst_size, src_size);
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

void compress_decompress_test_helper(std::span<zpods::byte> src) {
    auto [dst_size1, compressed1] = zpods::compress<zpods::LZ77>(src);
    auto [dst_size2, compressed2] = zpods::compress<zpods::LZ77 | zpods::Huffman>(src);
    spdlog::info("[raw] {}, [lz77] {} [lz77&huffman] {}", src.size(), dst_size1, dst_size2);

    ASSERT_LT(dst_size1, src.size());
    ASSERT_LT(dst_size2, src.size());

    auto [back_size1, decompressed1] = zpods::decompress<zpods::LZ77>(compressed1.get());
    auto [back_size2, decompressed2] = zpods::decompress<zpods::LZ77 | zpods::Huffman>(compressed2.get());

    ASSERT_EQ(back_size1, src.size());
    ASSERT_EQ(back_size2, src.size());
}

TEST(CompressTest, HugeCompressDecompress1) {
    auto test = [](int N) {
        zpods::byte src[N];
        for (int i = 0; i < N; i++) { src[i] = i & 0xff; }

        compress_decompress_test_helper(std::span(src, N));
    };

//    spdlog::set_level(spdlog::level::debug);

    for (int i = 10; i <= 18; i++) {
        test(1 << i);
    }

//    test(1 << 19);
}
