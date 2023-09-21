//
// Created by code4love on 23-9-20.
//

#include "compress.h"
#include "spdlog/spdlog.h"
#include "bit_stream.h"

using namespace zpods;

constexpr auto ceil_div(auto a, auto b) {
    return (a + b - 1) / b;
}

constexpr size_t mask(size_t bits) {
    return (1 << bits) - 1;
}

constexpr size_t bits_part(size_t bits, size_t offset, size_t cnt) {
    return (mask(cnt) & bits) << offset;
}

inline size_t make_ol_code(size_t offset, size_t len) {
    return bits_part(offset, 0, OFFSET_BITS) |
           bits_part(len, OFFSET_BITS, LENGTH_BITS);
}

inline size_t make_oll_code(size_t offset, size_t len, size_t literal) {
    return bits_part(offset, 0, OFFSET_BITS) |
           bits_part(len, OFFSET_BITS, LENGTH_BITS) |
           bits_part(literal, OFFSET_BITS + LENGTH_BITS, LITERAL_BITS);
}


class SearchBuffer {
public:
    SearchBuffer() = default;

    std::pair<size_t, size_t> search(const char *buf, size_t cur_offset, size_t buf_len) {
        size_t search_window_start = cur_offset > SEARCH_WINDOW_SIZE ? cur_offset - SEARCH_WINDOW_SIZE : 0;
        size_t max_match_offset = 0;
        size_t max_match_len = 0;

        {
            size_t match_len;

            for (size_t match_start = search_window_start; match_start < cur_offset; match_start++) {

                for (match_len = 0;
                     match_len + match_start < cur_offset && match_len < LOOK_AHEAD_WINDOW_SIZE; match_len++) {
                    if (buf[match_start + match_len] != buf[cur_offset + match_len]) {
                        break;
                    }
                }

                if (match_len && match_len >= max_match_len) {
                    max_match_len = match_len;
                    max_match_offset = cur_offset - match_start;
                }
            }
        }

        return {max_match_offset, max_match_len};
    }

private:

};

size_t zpods::compress(const char *src, size_t src_size, char *dst) {
    size_t dst_size = 0;

    SearchBuffer sb;

    size_t code;

    BitStream bs(dst);

    for (size_t i = 0; i < src_size;) {
        auto [offset, len] = sb.search(src, i, src_size);

        if (offset == 0 && len == 0) {
            code = make_oll_code(offset, len, *(src + i));

            bs.append_bits(&code, OLL_CODE);
            dst_size += OLL_CODE;

            spdlog::info("(ENCODE) OFFSET: {}, LEN: {}, LITERAL: {}, CODE: {:0{}b}", offset, len, *(src + i), code,
                         OLL_CODE);
        } else {
            code = make_ol_code(offset, len);

            bs.append_bits(&code, OL_CODE);
            dst_size += OL_CODE;

            spdlog::info("(ENCODE) OFFSET: {}, LEN: {}, WORDS: {}, CODE: {:0{}b}", offset, len,
                         std::string_view(src + i - offset, len), code, OL_CODE);
        }

        i += std::max(len, (size_t) 1);
    }

    // mark the end of compressed data
    code = make_oll_code(0, 0, eof_literal);
    bs.append_bits(&code, OLL_CODE);
    dst_size += OLL_CODE;

    return ceil_div(dst_size, 8);
}

size_t zpods::decompress(const char *src, size_t src_size, char *dst) {
    size_t dst_size = 0;
    BitStream src_bs((char *) src);
    BitStream dst_bs(dst);

    size_t offset, len, literal, code;

    while (true) {

        offset = src_bs.read_bits(OFFSET_BITS);
        len = src_bs.read_bits(LENGTH_BITS);

        if (offset == 0 && len == 0) {
            literal = src_bs.read_bits(LITERAL_BITS);

            if ((char) literal == eof_literal) {
                break;
            } else {
                dst_bs.append_byte((char) literal);
                dst_size++;

                code = make_oll_code(offset, len, literal);
                spdlog::info("(DECODE) OFFSET: {}, LEN: {}, LITERAL: {:c}, CODE: {:0{}b}", offset, len, literal, code,
                             OLL_CODE);
            }
        } else {
            code = make_ol_code(offset, len);
            spdlog::info("(DECODE) OFFSET: {}, LEN: {}, WORDS: {}, CODE: {:0{}b}", offset, len,
                         std::string_view(dst_bs.write_byte_ptr() - offset, len), code, OL_CODE);
            for (size_t i = 0; i < len; ++i) {
                dst_bs.append_byte(*(dst_bs.write_byte_ptr() - offset));
                dst_size++;

            }
        };

    }
    return dst_size;
}
