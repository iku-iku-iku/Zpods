//
// Created by code4love on 23-9-21.
//

#include "bit_stream.h"

using namespace zpods;

zpods::BitStream::BitStream(char *ptr) : ptr_(reinterpret_cast<unit_pointer>(ptr)) {

}

void zpods::BitStream::append_bit(unit_type bit) {
    *(ptr_ + write_bit_pos_ / BITS_PER_UNIT) &= ~(1 << (write_bit_pos_ % BITS_PER_UNIT));
    *(ptr_ + write_bit_pos_ / BITS_PER_UNIT) |= ((bit & 1) << (write_bit_pos_ % BITS_PER_UNIT));
    write_bit_pos_++;
}

void zpods::BitStream::append_byte(char byte) {
    append_bits(&byte, BYTE_BITS_LEN);
}

void zpods::BitStream::append_bits(const char *src, std::size_t bit_count) {
    for (std::size_t i = 0; i < bit_count; ++i) {
        append_bit(*(src + i / BYTE_BITS_LEN) >> (i % BYTE_BITS_LEN));
    }
}

zpods::BitStream::unit_type zpods::BitStream::read_bit() {
    unit_type bit = *(ptr_ + read_bit_pos_ / BITS_PER_UNIT) >> (read_bit_pos_ % BITS_PER_UNIT) & 1;
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
