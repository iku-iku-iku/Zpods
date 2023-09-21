//
// Created by code4love on 23-9-21.
//

#include <gtest/gtest.h>
#include <cstring>
#include "bit_stream.h"

TEST(bit_stream_test, test_read_write1) {
    char buf[1024];
    zpods::BitStream bs(buf);

    bs.append_bit(1);
    bs.append_bit(0);
    bs.append_bit(1);

    ASSERT_EQ(bs.read_bit(), 1);
    ASSERT_EQ(bs.read_bit(), 0);
    ASSERT_EQ(bs.read_bit(), 1);
}

TEST(bit_stream_test, test_read_write2) {
    char buf[1024];
    zpods::BitStream bs(buf);

    int magic = 0b11101;

    bs.append_bits((char *) &magic, 3);

    ASSERT_EQ(bs.read_bit(), 1);
    ASSERT_EQ(bs.read_bit(), 0);
    ASSERT_EQ(bs.read_bit(), 1);
    ASSERT_EQ(bs.read_bit(), 0);
    ASSERT_EQ(bs.read_bit(), 0);

    bs.back_read_bit(2);

    bs.append_bits((char *) &magic, 4);
    ASSERT_EQ(bs.read_bits(4), 0b1101);
    ASSERT_EQ(bs.read_bit(), 0);
}

TEST(bit_stream_test, test_read_write_big) {
    char buf[1024];
    zpods::BitStream bs(buf);

    const size_t N = 1000;

    for (int i = 0; i < N; ++i) {
        bs.append_bit(i % 2);
    }
    for (int i = 0; i < N; ++i) {
        ASSERT_EQ(bs.read_bit(), i % 2);
    }
}
