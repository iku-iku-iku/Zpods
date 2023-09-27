//
// Created by code4love on 23-9-20.
//

#ifndef ZPODS_COMPRESS_H
#define ZPODS_COMPRESS_H

#include <utility>

#include "pch.h"
#include "bit_ops.h"

namespace zpods {
    constexpr static size_t eof_literal = 255;

    constexpr static size_t OFFSET_BITS = 15;
    constexpr static size_t LENGTH_BITS = 5;
    constexpr static size_t LITERAL_BITS = 8;

    constexpr static size_t OL_CODE = OFFSET_BITS + LENGTH_BITS;
    constexpr static size_t OLL_CODE = OL_CODE + LITERAL_BITS;

    constexpr static size_t SEARCH_WINDOW_SIZE = 1 << OFFSET_BITS;
    constexpr static size_t LOOK_AHEAD_WINDOW_SIZE = 1 << LENGTH_BITS;

    /*
     * @brief compress the src to dst
     * @param src is the pointer to the source data
     * @param src_size is the size (in bytes) of source data
     * @return the size (in bytes) of compressed string and the pointer to the compressed data
     */
    constexpr auto LZ77 = 1 << 0;
    constexpr auto Huffman = 1 << 1;


    template<size_t Policy = LZ77>
    std::pair<size_t, std::unique_ptr<byte[]>> compress(p_cbyte src, size_t src_size);

    /*
     * @brief decompress the src to dst
     * @param src is the pointer to the source data
     * @return the size (in bytes) of decompressed string and the pointer to the decompressed data
     */
    template<size_t Policy = LZ77>
    std::pair<size_t, std::unique_ptr<byte[]>> decompress(p_cbyte src);

    template<typename Value>
    struct Node {
        size_t freq = 0;
        Value val = 0;
        bool is_leaf = false;
        std::shared_ptr<Node> left = nullptr;
        std::shared_ptr<Node> right = nullptr;

        Node(size_t freq, Value val, bool is_leaf, std::shared_ptr<Node> &&left, std::shared_ptr<Node> &&right)
                : freq(freq), val(val), is_leaf(is_leaf), left(std::move(left)), right(std::move(right)) {}

        auto operator<=>(const Node &rhs) const {
            return freq <=> rhs.freq;
        }
    };

    struct Code {
        size_t bits;
        size_t len;

        friend bool operator==(const zpods::Code &lhs, const zpods::Code &rhs) {
            return lhs.bits == rhs.bits && lhs.len == rhs.len;
        }
    };


    template<typename T>
    std::unordered_map<T, size_t> get_freq_table(std::span<T> rng) {
        std::unordered_map<T, size_t> freq_table;
        for (let_ref item: rng) {
            freq_table[item]++;
        }
        return freq_table;
    }

    template<typename Value>
    void
    dfs_huffman_tree(ref<Node<Value>> root, ref_mut<std::unordered_map<Value, Code>> huffman_dict, Code code = {0, 0}) {
        if (root.is_leaf) {
            huffman_dict[root.val] = code;
            return;
        }
        dfs_huffman_tree<Value>(*root.left, huffman_dict, {code.bits | (0 << code.len), code.len + 1});
        dfs_huffman_tree<Value>(*root.right, huffman_dict, {code.bits | (1 << code.len), code.len + 1});
    }


    template<typename Value>
    std::unordered_map<Value, Code> make_huffman_dictionary(std::span<Value> rng) {
        ZPODS_ASSERT(!rng.empty());

        let freq_table = get_freq_table(rng);
        std::unordered_map<Value, Code> huffman_dict;

        if (freq_table.size() == 1) {
            huffman_dict[freq_table.begin()->first] = {0, 1};
            return huffman_dict;
        }

        using Term = Node<Value>;
        std::priority_queue<Term, std::vector<Term>, std::greater<>> pq;
        for (auto &i: freq_table) {
            pq.emplace(i.second, i.first, true, nullptr, nullptr);
        }
        while (pq.size() > 1) {
            let node1 = pq.top();
            pq.pop();
            let node2 = pq.top();
            pq.pop();

            pq.emplace(node1.freq + node2.freq, 0, false, std::make_shared<Term>(node1), std::make_shared<Term>(node2));
        }

        dfs_huffman_tree<Value>(pq.top(), huffman_dict);
        return huffman_dict;
    }
}

template<>
struct fmt::formatter<zpods::Code> {
    constexpr auto parse(format_parse_context &ctx) {
        return ctx.end();
    }

    template<typename FormatContext>
    auto format(const zpods::Code &p, FormatContext &ctx) const {
        return format_to(ctx.out(), "0b{:0{}b}", p.bits, p.len, p.len);
    }
};

namespace std {
    template<>
    struct hash<zpods::Code> {
        std::size_t operator()(const zpods::Code &p) const {
            return std::hash<int>()(p.bits) ^ std::hash<int>()(p.len);
        }
    };
}
#endif //ZPODS_COMPRESS_H
