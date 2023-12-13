//
// Created by code4love on 23-9-21.
//

#include <cstring>
#include "ZpodsLib/src/base/bit_ops.h"
#include <gtest/gtest.h>

TEST(BitStreamTest, test_read_write1)
{
    zpods::BitStream bs;

    bs.append_bit(1);
    bs.append_bit(0);
    bs.append_bit(1);

    ASSERT_EQ(bs.read_bit(), 1);
    ASSERT_EQ(bs.read_bit(), 0);
    ASSERT_EQ(bs.read_bit(), 1);
}

TEST(BitStreamTest, test_read_write2)
{
    zpods::BitStream bs;

    int magic = 0b11101;

    bs.append_bits((char*)&magic, 3);

    ASSERT_EQ(bs.read_bit(), 1);
    ASSERT_EQ(bs.read_bit(), 0);
    ASSERT_EQ(bs.read_bit(), 1);
    ASSERT_EQ(bs.read_bit(), 0);
    ASSERT_EQ(bs.read_bit(), 0);

    bs.back_read_bit(2);

    bs.append_bits((char*)&magic, 4);
    ASSERT_EQ(bs.read_bits(4), 0b1101);
    ASSERT_EQ(bs.read_bit(), 0);
}

TEST(BitStreamTest, test_read_write_big)
{
    zpods::BitStream bs;

    const size_t N = 1000;

    for (int i = 0; i < N; ++i)
    {
        bs.append_bit(i % 2);
    }
    for (int i = 0; i < N; ++i)
    {
        ASSERT_EQ(bs.read_bit(), i % 2);
    }
}
