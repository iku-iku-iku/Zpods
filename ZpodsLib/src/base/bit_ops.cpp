//
// Created by code4love on 23-9-21.
//

#include "ZpodsLib/src/base/bit_ops.h"

using namespace zpods;

zpods::BitStream::BitStream(char *ptr) : base_ptr_(reinterpret_cast<unit_pointer>(ptr)) {
    if (base_ptr_ == nullptr) {
        bytes_buf_len = INIT_BYTES_BUF_LEN;
        let new_ptr = new byte[bytes_buf_len]{0};
        bytes_buf_ = std::unique_ptr<byte[]>(new_ptr);
        base_ptr_ = reinterpret_cast<unit_pointer>(new_ptr);
    }
}

void zpods::BitStream::append_bit(unit_type bit) {
    if (floor_write_bytes() == bytes_buf_len) {
        let old_bytes_buf_len = bytes_buf_len;
        bytes_buf_len = (size_t) ((float)bytes_buf_len * 1.5f);
        let new_base_ptr = new byte[bytes_buf_len]{0};

        memcpy(new_base_ptr, bytes_buf_.get(), old_bytes_buf_len);
        base_ptr_ = new_base_ptr;
        bytes_buf_.reset(new_base_ptr);
    }
    // clear bit
    *(base_ptr_ + write_bit_pos_ / BITS_PER_UNIT) &= ~((unsigned)1 << (unsigned)(write_bit_pos_ % BITS_PER_UNIT));
    // set bit
    *(base_ptr_ + write_bit_pos_ / BITS_PER_UNIT) |= ((unsigned)(bit & 1) << (unsigned)(write_bit_pos_ % BITS_PER_UNIT));
    write_bit_pos_++;
}

void zpods::BitStream::append_byte(byte byte) {
    append_bits(&byte, BYTE_BITS_LEN);
}

void zpods::BitStream::append_bits(p_cbyte src, std::size_t bit_count) {
    for (std::size_t i = 0; i < bit_count; ++i) {
        append_bit(*(src + i / BYTE_BITS_LEN) >> (i % BYTE_BITS_LEN));
    }
}

zpods::BitStream::unit_type zpods::BitStream::read_bit() {
    unit_type bit = *(base_ptr_ + read_bit_pos_ / BITS_PER_UNIT) >> (read_bit_pos_ % BITS_PER_UNIT) & 1;
    read_bit_pos_++;
    return bit;
}

char zpods::BitStream::read_byte() {
    return (char) read_bits(BYTE_BITS_LEN);
}

std::size_t zpods::BitStream::read_bits(std::size_t bit_count) {
    std::size_t bits = 0;
    for (std::size_t i = 0; i < bit_count; ++i) {
        bits |= (read_bit() << i);
    }
    return bits;
}
