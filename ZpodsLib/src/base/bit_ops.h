//
// Created by code4love on 23-9-21.
//

#ifndef ZPODS_BITSTREAM_H
#define ZPODS_BITSTREAM_H

#include "pch.h"

namespace zpods
{

class BitStream
{
  public:
    using unit_type = byte;
    using unit_pointer = unit_type*;

    explicit BitStream(char* ptr = nullptr);

    void append_bit(unit_type bit);

    void append_byte(byte byte);

    void append_bits(auto* src, std::size_t bit_count)
    {
        append_bits(reinterpret_cast<p_cbyte>(src), bit_count);
    }

    void append_bits(p_cbyte src, std::size_t bit_count);

    unit_type read_bit();

    char read_byte();

    std::size_t read_bits(std::size_t bit_count);

    void back_write_bit(int bits = 1) { write_bit_pos_ -= bits; }

    void back_read_bit(int bits = 1) { read_bit_pos_ -= bits; }

    p_byte write_byte_ptr()
    {
        return reinterpret_cast<p_byte>(base_ptr_) +
               write_bit_pos_ / BYTE_BITS_LEN;
    }

    [[nodiscard]] auto floor_write_bytes() const
    {
        return write_bit_pos_ / BYTE_BITS_LEN;
    }

    [[nodiscard]] auto write_len_in_bytes() const
    {
        return ceil_div(write_bit_pos_, BYTE_BITS_LEN);
    }

    unit_pointer get_buf_ptr_() { return bytes_buf_.get(); }

    auto take_buf() -> std::unique_ptr<byte[]>&&
    {
        return std::move(bytes_buf_);
    }

  private:
    std::unique_ptr<byte[]> bytes_buf_;
    size_t bytes_buf_len;
    unit_pointer base_ptr_ = nullptr;
    std::size_t write_bit_pos_ = 0;
    std::size_t read_bit_pos_ = 0;

    static constexpr auto BYTE_BITS_LEN = 8;
    static constexpr auto BITS_PER_UNIT = sizeof(unit_type) * BYTE_BITS_LEN;
    static constexpr auto INIT_BYTES_BUF_LEN = 16;
};
} // namespace zpods

#endif // ZPODS_BITSTREAM_H
