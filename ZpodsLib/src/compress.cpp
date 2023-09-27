//
// Created by code4love on 23-9-20.
//
#include "compress.h"
#include "bit_ops.h"

using namespace zpods;

namespace {

    inline size_t make_ol_code(size_t offset, size_t len) {
        return bits_part(offset, 0, OFFSET_BITS) |
               bits_part(len, OFFSET_BITS, LENGTH_BITS);
    }

    inline size_t make_oll_code(size_t offset, size_t len, size_t literal) {
        return bits_part(offset, 0, OFFSET_BITS) |
               bits_part(len, OFFSET_BITS, LENGTH_BITS) |
               bits_part(literal, OFFSET_BITS + LENGTH_BITS, LITERAL_BITS);
    }

    template<typename T>
    struct HuffmanDictEntry {
        Code code{};
        T val{};

        static constexpr auto ValBits = 16;

        HuffmanDictEntry(Code code, T val) : code(code), val(val) {}

        HuffmanDictEntry() = default;

        void fill(ref_mut <BitStream> bs) {
            bs.append_bits(&code.len, BYTE_BITS);
            bs.append_bits(&code.bits, code.len);
            bs.append_bits(&val, ValBits);
        }

        /*
         * @brief read a HuffmanDictEntry from the bit stream
         * @param bs is the bit stream
         * @return true if the entry is the end of the dictionary, false otherwise
         */
        bool read(ref_mut <BitStream> bs) {
            code.len = bs.read_bits(BYTE_BITS);
            if (code.len == 0) {
                return true;
            }
            code.bits = bs.read_bits(code.len);
            val = bs.read_bits(ValBits);
            return false;
        }
    };

    template<typename T>
    void fill_dict(ref_mut <BitStream> bs, const std::unordered_map<T, Code> &dict) {
        for (const auto &[val, code]: dict) {
            HuffmanDictEntry<T>(code, val).fill(bs);
        }
        bs.append_byte(0);
    }

    auto read_dict(ref_mut <BitStream> bs) {
        std::unordered_map<Code, size_t> dict;

        HuffmanDictEntry<size_t> entry;
        while (true) {
            if (entry.read(bs)) {
                break;
            }
            dict[entry.code] = entry.val;
        }

        return dict;
    }

    template<typename T>
    std::pair<T, Code> read_with_dict(ref_mut <BitStream> bs, const std::unordered_map<Code, T> &dict) {
        size_t bits = 0;
        size_t len = 0;
        while (true) {
            bits = bits | ((bs.read_bit() & 1) << len);
            len++;
            let iter = dict.find({bits, len});
            if (iter != dict.end()) {
                return {iter->second, iter->first};
            }
        }
    }

    class SearchBuffer {
    public:
        SearchBuffer() = default;

        // TODO: optimize with trie
        std::pair<size_t, size_t> search(p_cbyte buf, size_t cur_offset, size_t buf_len) {
            size_t search_window_start = cur_offset > SEARCH_WINDOW_SIZE ? cur_offset - SEARCH_WINDOW_SIZE : 0;
            size_t max_match_offset = 0;
            size_t max_match_len = 0;
            auto remain_len = buf_len - cur_offset;

            {
                size_t match_len;

                for (size_t match_start = search_window_start; match_start < cur_offset; match_start++) {

                    for (match_len = 0;
                         match_len + match_start < cur_offset &&
                         match_len < LOOK_AHEAD_WINDOW_SIZE &&
                         match_len < remain_len;
                         match_len++) {
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
}

template<size_t Policy>
std::pair<size_t, std::unique_ptr<byte[]>> zpods::compress(p_cbyte src, size_t src_size) {
    constexpr auto HuffmanEnabled = enabled(Policy, Huffman);
    size_t dst_size = 0;

    SearchBuffer sb;

    size_t code;

    BitStream bs;

    std::vector<size_t> offsets, lens, literals;

    for (size_t i = 0; i < src_size;) {
        auto [offset, len] = sb.search(src, i, src_size);

        if (offset == 0 && len == 0) {
            if constexpr (!HuffmanEnabled) {
                code = make_oll_code(offset, len, *(src + i));
                bs.append_bits(&code, OLL_CODE);
                dst_size += OLL_CODE;

//            spdlog::info("(ENCODE) OFFSET: {}, LEN: {}, LITERAL: {:c}, CODE: {:0{}b}", offset, len, *(src + i), code,
//                         OLL_CODE);
            } else {
                offsets.push_back(offset);
                lens.push_back(len);
                literals.push_back(*(src + i));
            }
        } else {
            if constexpr (!HuffmanEnabled) {
                code = make_ol_code(offset, len);
                bs.append_bits(&code, OL_CODE);
                dst_size += OL_CODE;

//            spdlog::info("(ENCODE) OFFSET: {}, LEN: {}, WORDS: {}, CODE: {:0{}b}", offset, len,
//                         std::string_view((const char*)src + i - offset, len), code, OL_CODE);
            } else {
                offsets.push_back(offset);
                lens.push_back(len);
            }
        }

        i += std::max(len, (size_t) 1);
    }

    ZPODS_ASSERT(offsets.size() == lens.size());

    if (!HuffmanEnabled) {
        // mark the end of compressed data
        code = make_oll_code(0, 0, eof_literal);
        bs.append_bits(&code, OLL_CODE);
        dst_size += OLL_CODE;

        dst_size = ceil_div(dst_size, 8);

//    spdlog::info("(ENCODE) FROM {} bytes TO {} bytes, compress ratio = {:f}%", src_size, dst_size,
//                 (double) dst_size / (double) src_size * 100);
    } else {
        offsets.push_back(0);
        lens.push_back(0);
        literals.push_back(eof_literal);

        let offset_dict = make_huffman_dictionary(std::span(offsets));
        print_map(offset_dict);
        let len_dict = make_huffman_dictionary(std::span(lens));
        print_map(len_dict);
        let literal_dict = make_huffman_dictionary(std::span(literals));
        print_map(literal_dict);

        fill_dict(bs, offset_dict);
        fill_dict(bs, len_dict);
        fill_dict(bs, literal_dict);

        let_mut literal_iter = literals.begin();
        for (size_t i = 0; i < offsets.size(); i++) {
            let offset = offsets[i];
            let len = lens[i];

            let_ref offset_code = offset_dict.find(offset)->second;
            let_ref len_code = len_dict.find(len)->second;

            bs.append_bits(&offset_code.bits, offset_code.len);
            dst_size += offset_code.len;
            bs.append_bits(&len_code.bits, len_code.len);
            dst_size += len_code.len;

            if (offset == 0 && len == 0) {
                let_ref literal = *literal_iter++;
                let_ref literal_code = literal_dict.find(literal)->second;
                bs.append_bits(&literal_code.bits, literal_code.len);
                dst_size += literal_code.len;
                spdlog::info("(ENCODE) OFFSET: {}, LEN: {}, LITERAL: {}", offset_code, len_code, literal_code);
            } else {
                spdlog::info("(ENCODE) OFFSET: {}, LEN: {}", offset_code, len_code);
            }
        }
    }

    return {dst_size, std::move(bs.take_buf())};
}

template<size_t Policy>
std::pair<size_t, std::unique_ptr<byte[]>> zpods::decompress(p_cbyte src) {
    constexpr auto HuffmanEnabled = enabled(Policy, Huffman);

    size_t dst_size = 0;
    BitStream dst_bs;

    size_t offset, len, literal, code;

    std::unordered_map<Code, size_t> offset_dict, len_dict, literal_dict;
    std::vector<size_t> offsets, lens, literals;

    if constexpr (HuffmanEnabled) {
        BitStream src_bs((char *) src);

        {
            offset_dict = read_dict(src_bs);
            print_map(offset_dict);
            len_dict = read_dict(src_bs);
            print_map(len_dict);
            literal_dict = read_dict(src_bs);
            print_map(literal_dict);
        }

        while (true) {

            let [offset, offset_code] = read_with_dict(src_bs, offset_dict);
            let [len, len_code] = read_with_dict(src_bs, len_dict);

            offsets.push_back(offset);
            lens.push_back(len);

            if (offset == 0 && len == 0) {
                let [literal, literal_code] = read_with_dict(src_bs, literal_dict);
                literals.push_back(literal);
                if (literal == eof_literal) {
                    break;
                }
                spdlog::info("(DECODE) OFFSET: {}, LEN: {}, LITERAL: {}", offset_code, len_code, literal_code);
            } else {
                spdlog::info("(DECODE) OFFSET: {}, LEN: {}", offset_code, len_code);
            }
        }

    }

    if (!HuffmanEnabled) {
        BitStream src_bs((char *) src);
        while (true) {

            offset = src_bs.read_bits(OFFSET_BITS);
            len = src_bs.read_bits(LENGTH_BITS);

            offsets.push_back(offset);
            lens.push_back(len);

            if (offset == 0 && len == 0) {
                literal = src_bs.read_bits(LITERAL_BITS);
                literals.push_back(literal);

                if (literal == eof_literal) {
                    break;
                } else {
//                    dst_bs.append_byte((char) literal);
//                    dst_size++;
//
//                    code = make_oll_code(offset, len, literal);
//                spdlog::info("(DECODE) OFFSET: {}, LEN: {}, LITERAL: {:c}, CODE: {:0{}b}", offset, len, literal, code,
//                             OLL_CODE);
                }
            } else {
//                code = make_ol_code(offset, len);
//            spdlog::info("(DECODE) OFFSET: {}, LEN: {}, WORDS: {}, CODE: {:0{}b}", offset, len,
//                         std::string_view(reinterpret_cast<const char *>(dst_bs.write_byte_ptr() - offset), len), code, OL_CODE);
//                for (size_t i = 0; i < len; ++i) {
//                    dst_bs.append_byte(*(dst_bs.write_byte_ptr() - offset));
//                    dst_size++;
//                }
            };
        }
    }

    let_mut iter = literals.begin();
    for (size_t  i = 0; i < offsets.size(); i++)  {
        offset = offsets[i];
        len = lens[i];

        if (offset ==0 && len ==0) {
            literal = *iter++;
            if (literal == eof_literal) {
                break;
            }
            dst_bs.append_byte((char) literal);
            spdlog::info("CAHR: {:c}", literal);
            dst_size++;
        } else {
            spdlog::info("WORDS: {}", std::string_view(reinterpret_cast<const char *>(dst_bs.write_byte_ptr() - offset), len));
            for (size_t j = 0; j < len; ++j) {
                dst_bs.append_byte(*(dst_bs.write_byte_ptr() - offset));
                dst_size++;
            }
        }
    }

    return {dst_size, std::move(dst_bs.take_buf())};
}

template std::pair<size_t, std::unique_ptr<byte[]>> zpods::compress<LZ77>(zpods::p_cbyte src, size_t src_size);

template std::pair<size_t, std::unique_ptr<byte[]>>
zpods::compress<LZ77 | Huffman>(zpods::p_cbyte src, size_t src_size);

template std::pair<size_t, std::unique_ptr<byte[]>> zpods::decompress<LZ77>(p_cbyte src);

template std::pair<size_t, std::unique_ptr<byte[]>> zpods::decompress<LZ77 | Huffman>(p_cbyte src);
