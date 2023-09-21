//
// Created by code4love on 23-9-21.
//

#ifndef ZPODS_BITSTREAM_H
#define ZPODS_BITSTREAM_H

#include <cstdint>

namespace zpods {

    class BitStream {
    public:
        using unit_type = unsigned int;
        using unit_pointer = unit_type *;

        explicit BitStream(char *ptr);

        void append_bit(unit_type bit);

        void append_byte(char byte);

        void append_bits(auto *src, std::size_t bit_count) {
            append_bits(reinterpret_cast<const char *>(src), bit_count);
        }
        void append_bits(const char *src, std::size_t bit_count);

        unit_type read_bit();

        char read_byte();

        std::size_t read_bits(std::size_t bit_count);

        void back_write_bit(int bits = 1) { write_bit_pos_ -= bits; }

        void back_read_bit(int bits = 1) { read_bit_pos_ -= bits; }

        char *write_byte_ptr() { return reinterpret_cast<char *>(ptr_) + write_bit_pos_ / BYTE_BITS_LEN; }

    private:
        unit_pointer ptr_;
        std::size_t write_bit_pos_ = 0;
        std::size_t read_bit_pos_ = 0;

        static constexpr auto BYTE_BITS_LEN = 8;
        static constexpr auto BITS_PER_UNIT = sizeof(unit_type) * BYTE_BITS_LEN;
    };
}


#endif //ZPODS_BITSTREAM_H
