//
// Created by code4love on 23-9-20.
//
#include "compress.h"
#include "bit_ops.h"
#include "fs.h"

using namespace zpods;

namespace {

    inline bool literal_mark(size_t offset, size_t len) {
        return offset == 0 && len == 0;
    }

    inline bool eof_mark(size_t offset, size_t len) {
        return offset == 0 && len == mask(LENGTH_BITS);
    }

    inline constexpr size_t make_ol_code(size_t offset, size_t len) {
        return bits_part(offset, 0, OFFSET_BITS) |
               bits_part(len, OFFSET_BITS, LENGTH_BITS);
    }

    inline size_t make_oll_code(size_t offset, size_t len, size_t literal) {
        return bits_part(offset, 0, OFFSET_BITS) |
               bits_part(len, OFFSET_BITS, LENGTH_BITS) |
               bits_part(literal, OFFSET_BITS + LENGTH_BITS, LITERAL_BITS);
    }
}


namespace {


    template<typename T>
    struct HuffmanDictEntry {
        Code code{};
        T val{};

        static constexpr auto CodeLenBits = 4;

        HuffmanDictEntry(Code code, T val) : code(code), val(val) {}

        HuffmanDictEntry() = default;

        template<size_t ValBits>
        auto fill(ref_mut <BitStream> bs) {
            bs.append_bits(&code.len, CodeLenBits);
            bs.append_bits(&code.bits, code.len);
            bs.append_bits(&val, ValBits);

            return CodeLenBits + code.len + ValBits;
        }

        /*
         * @brief read a HuffmanDictEntry from the bit stream
         * @param bs is the bit stream
         * @return true if the entry is the end of the dictionary, false otherwise
         */
        template<size_t ValBits>
        bool read(ref_mut <BitStream> bs) {
            code.len = bs.read_bits(CodeLenBits);
            if (code.len == 0) {
                return true;
            }
            code.bits = bs.read_bits(code.len);
            val = bs.read_bits(ValBits);
            return false;
        }
    };

    template<size_t ValBits, typename T>
    auto fill_dict(ref_mut <BitStream> bs, const std::unordered_map<T, Code> &dict) {
        size_t write_bits = 0;
        for (const auto &[val, code]: dict) {
            write_bits += HuffmanDictEntry<T>(code, val).template fill<ValBits>(bs);
        }

        byte eod = 0;
        bs.append_bits(&eod, HuffmanDictEntry<T>::CodeLenBits);

        write_bits += HuffmanDictEntry<T>::CodeLenBits;
        return write_bits;
    }

    template<size_t ValBits>
    auto read_dict(ref_mut <BitStream> bs) {
        std::unordered_map<Code, size_t> dict;

        HuffmanDictEntry<size_t> entry;
        while (true) {
            if (entry.read<ValBits>(bs)) {
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
                         match_len < LOOK_AHEAD_WINDOW_SIZE - 1 &&
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
std::pair<size_t, std::unique_ptr<byte[]>> zpods::compress(std::span<byte> src_rng) {
    constexpr auto HuffmanEnabled = enabled(Policy, Huffman);
    size_t dst_size = 0;

    SearchBuffer sb;

    BitStream bs;

    std::vector<size_t> offsets, lens, literals;

    p_cbyte src = src_rng.data();
    let src_size = src_rng.size();

    for (size_t i = 0; i < src_size;) {
        auto [offset, len] = sb.search(src, i, src_size);

        if (offset == 0 && len == 0) {
            offsets.push_back(offset);
            lens.push_back(len);
            literals.push_back(*(src + i));
        } else {
            offsets.push_back(offset);
            lens.push_back(len);
        }

        i += std::max(len, (size_t) 1);
    }

    ZPODS_ASSERT(offsets.size() == lens.size());

    if (!HuffmanEnabled) {
        let_mut literal_iter = literals.begin();
        for (size_t i = 0; i < offsets.size(); i++) {
            let offset = offsets[i];
            let len = lens[i];
            if (offset == 0 && len == 0) {
                let_ref literal = *literal_iter++;
                let code = make_oll_code(offset, len, literal);
                bs.append_bits(&code, OLL_CODE);
                dst_size += OLL_CODE;

                spdlog::debug("(ENCODE) OFFSET: {}, LEN: {}, LITERAL: {:c}", offset, len, literal);
            } else {
                let code = make_ol_code(offset, len);
                bs.append_bits(&code, OL_CODE);
                dst_size += OL_CODE;

                spdlog::debug("(ENCODE) OFFSET: {}, LEN: {}", offset, len);
            }
        }
        // mark the end of compressed data
        let code = make_ol_code(0, mask(LENGTH_BITS));
        bs.append_bits(&code, OL_CODE);
        dst_size += OLL_CODE;
    } else {
        offsets.push_back(0);
        lens.push_back(EOF_LEN);

        let offset_dict = make_huffman_dictionary(std::span(offsets));
        print_map(offset_dict);
        let len_dict = make_huffman_dictionary(std::span(lens));
        print_map(len_dict);
        let literal_dict = make_huffman_dictionary(std::span(literals));
        print_map(literal_dict);

        dst_size += fill_dict<OFFSET_BITS>(bs, offset_dict);
        dst_size += fill_dict<LENGTH_BITS>(bs, len_dict);
        dst_size += fill_dict<LITERAL_BITS>(bs, literal_dict);

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
                spdlog::debug("(ENCODE) OFFSET: {}, LEN: {}, LITERAL: {}", offset_code, len_code, literal_code);
            } else {
                spdlog::debug("(ENCODE) OFFSET: {}, LEN: {}", offset_code, len_code);
            }
        }
    }

    dst_size = ceil_div(dst_size, 8);

    return {dst_size, std::move(bs.take_buf())};
}

template<size_t Policy>
std::pair<size_t, std::unique_ptr<byte[]>> zpods::decompress(p_cbyte src) {
    constexpr auto HuffmanEnabled = enabled(Policy, Huffman);

    size_t dst_size = 0;
    BitStream dst_bs;

    std::unordered_map<Code, size_t> offset_dict, len_dict, literal_dict;
    std::vector<size_t> offsets, lens, literals;

    if constexpr (HuffmanEnabled) {
        BitStream src_bs((char *) src);

        {
            offset_dict = read_dict<OFFSET_BITS>(src_bs);
            print_map(offset_dict);
            len_dict = read_dict<LENGTH_BITS>(src_bs);
            print_map(len_dict);
            literal_dict = read_dict<LITERAL_BITS>(src_bs);
            print_map(literal_dict);
        }

        while (true) {

            let [offset, offset_code] = read_with_dict(src_bs, offset_dict);
            let [len, len_code] = read_with_dict(src_bs, len_dict);

            offsets.push_back(offset);
            lens.push_back(len);

            if (eof_mark(offset, len)) {
                break;
            }

            if (literal_mark(offset, len)) {
                let [literal, literal_code] = read_with_dict(src_bs, literal_dict);
                literals.push_back(literal);
                spdlog::debug("(DECODE) OFFSET: {}, LEN: {}, LITERAL: {}", offset_code, len_code, literal_code);
            } else {
                spdlog::debug("(DECODE) OFFSET: {}, LEN: {}", offset_code, len_code);
            }
        }

    }

    if (!HuffmanEnabled) {
        BitStream src_bs((char *) src);
        while (true) {

            let offset = src_bs.read_bits(OFFSET_BITS);
            let len = src_bs.read_bits(LENGTH_BITS);

            offsets.push_back(offset);
            lens.push_back(len);

            if (eof_mark(offset, len)) {
                break;
            }

            if (literal_mark(offset, len)) {
                let literal = src_bs.read_bits(LITERAL_BITS);
                literals.push_back(literal);
            }
        }
    }

    let_mut iter = literals.begin();
    for (size_t i = 0; i < offsets.size(); i++) {
        let offset = offsets[i];
        let len = lens[i];

        if (eof_mark(offset, len)) {
            break;
        }

        if (literal_mark(offset, len)) {
            let literal = *iter++;
            dst_bs.append_byte((char) literal);
            dst_size++;

            spdlog::debug("(DECODE) OFFSET: {}, LEN: {}, LITERAL: {:c}", offset, len, literal);
        } else {
            spdlog::debug("(DECODE) OFFSET: {}, LEN: {}, WORDS: {}", offset, len,
                          std::string_view((const char *) (dst_bs.write_byte_ptr() - offset), len));
            for (size_t j = 0; j < len; ++j) {
                dst_bs.append_byte(*(dst_bs.write_byte_ptr() - offset));
                dst_size++;
            }
        }
    }

    return {dst_size, std::move(dst_bs.take_buf())};
}

Status zpods::compress_file(const char *src_path, const char *dst_path) {
    let_mut ifs = fs::open_or_create_file_as_ifs(src_path, fs::ios::binary);
    if (!ifs.is_open()) {
        spdlog::error("cannot open file: {}", src_path);
        return Status::ERROR;
    }

    let src_size = fs::get_file_size(src_path);
    std::vector<byte> src(src_size);
    ifs.read((char *) src.data(), src_size);

    let [dst_size, compressed] = compress(src);

    let_mut ofs = fs::open_or_create_file_as_ofs(dst_path, fs::ios::binary);
    if (!ofs.is_open()) {
        spdlog::error("cannot open file: {}", dst_path);
        return Status::ERROR;
    }

    ofs.write((const char *) compressed.get(), dst_size);

    return Status::OK;
}

Status zpods::decompress_file(const char *src_path, const char *dst_path) {
    let_mut ifs = fs::open_or_create_file_as_ifs(src_path, std::ios::binary);
    if (!ifs.is_open()) {
        spdlog::error("cannot open file: {}", src_path);
        return Status::ERROR;
    }

    let src_size = fs::get_file_size(src_path);
    std::vector<byte> src(src_size);
    ifs.read((char *) src.data(), src_size);

    let [dst_size, decompressed] = decompress(src.data());

    let_mut ofs = fs::open_or_create_file_as_ofs(dst_path, std::ios::binary);
    if (!ofs.is_open()) {
        spdlog::error("cannot open file: {}", dst_path);
        return Status::ERROR;
    }

    ofs.write((const char *) decompressed.get(), dst_size);

    return Status::OK;
}

template std::pair<size_t, std::unique_ptr<byte[]>> zpods::compress<LZ77>(std::span<byte>);

template std::pair<size_t, std::unique_ptr<byte[]>> zpods::compress<LZ77 | Huffman>(std::span<byte>);

template std::pair<size_t, std::unique_ptr<byte[]>> zpods::decompress<LZ77>(p_cbyte src);

template std::pair<size_t, std::unique_ptr<byte[]>> zpods::decompress<LZ77 | Huffman>(p_cbyte src);
