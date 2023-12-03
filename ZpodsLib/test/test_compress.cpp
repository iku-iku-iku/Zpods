//
// Created by code4love on 23-9-20.
//

#include <gtest/gtest.h>
#include <cstring>
#include "ZpodsLib/src/base/compress.h"
#include "ZpodsLib/src/base/fs.h"

using namespace zpods;

void compress_decompress_test_helper(std::span<byte> src) {
    let [dst_size1, compressed1] = compress<LZ77>(src);
    let [dst_size2, compressed2] = compress(src);
    spdlog::info("[raw] {}, [lz77] {} [lz77&huffman] {}", src.size(), dst_size1, dst_size2);

    ASSERT_LT(dst_size1, src.size());
    ASSERT_LT(dst_size2, src.size());

    let [back_size1, decompressed1] = decompress<LZ77>(compressed1.get());
    let [back_size2, decompressed2] = decompress(compressed2.get());

    ASSERT_EQ(back_size1, src.size());
    ASSERT_EQ(back_size2, src.size());
}

TEST(CompressTest, SimpleCompressDecompress) {
    byte src[] =
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
    compress_decompress_test_helper(std::span(src, src_size));
}

TEST(CompressTest, TestHuffman) {
    byte src[] = "AABC";
    let_mut dict = make_huffman_dictionary(std::span(src, 4));
    ASSERT_TRUE(dict['A'].len == 1);
    ASSERT_TRUE(dict['B'].len == 2);
    ASSERT_TRUE(dict['C'].len == 2);
}

TEST(CompressTest, HugeCompressDecompress) {
    let test = [](int N) {
        byte src[N];
        for (int i = 0; i < N; i++) { src[i] = i & 0xff; }

        compress_decompress_test_helper(std::span(src, N));
    };

    for (int i = 10; i <= 18; i++) {
        test(1 << i);
    }
}

TEST(CompressTest, FileCompressDecompress) {
    let test_file = std::string("/single/man_pthreads.txt");
    let src_path = test_data_path() + test_file;
    let dst_compressed = temp_path() + test_file + ".zpods";
    let dst_decompressed = temp_path() + test_file;

    let status = compress_file(src_path.c_str(), dst_compressed.c_str());
    ASSERT_EQ(status, Status::OK);

    let status2 = decompress_file(dst_compressed.c_str(), dst_decompressed.c_str());
    ASSERT_EQ(status2, Status::OK);

    // check file size
    let src_file_size = fs::get_file_size(src_path.c_str());
    let compressed_file_size = fs::get_file_size(dst_compressed.c_str());
    let decompressed_file_size = fs::get_file_size(dst_decompressed.c_str());

    spdlog::info("src_file_size: {}, compressed_file_size: {}, decompressed_file_size: {}",
                 src_file_size, compressed_file_size, decompressed_file_size);
    spdlog::info("compress ratio: {}", (double) compressed_file_size / (double) src_file_size);
    ASSERT_LT(compressed_file_size, src_file_size);
    ASSERT_EQ(src_file_size, decompressed_file_size);
}
