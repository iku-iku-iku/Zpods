//
// Created by code4love on 23-9-23.
//

#ifndef ZPODS_PCH_H
#define ZPODS_PCH_H

#include "spdlog/spdlog.h"

#include <type_traits>
#include <cstdint>
#include <string>
#include <vector>
#include <ranges>
#include <span>
#include <numeric>
#include <fstream>
#include <cassert>
#include <queue>
#include <stack>

#include "enum.h"



#ifndef PROJECT_PATH
// do not use this macro directly!
#define PROJECT_PATH "macro PROJECT_PATH needed"
#endif

#ifndef TEST_DATA_PATH
// do not use this macro directly!
#define TEST_DATA_PATH "macro TEST_DATA_PATH needed"
#endif

#ifndef TEMP_PATH
// do not use this macro directly!
#define TEMP_PATH "macro TEMP_PATH needed"
#endif

namespace zpods {

    #define ZPODS_ASSERT(x) assert(x)

    #define let const auto
    #define let_ref const auto&
    #define let_mut auto
    #define let_mut_ref auto&

    template<typename T>
    using ref = const std::remove_cvref_t<T> &;

    template<typename T>
    using ref_mut = std::remove_cvref_t<T> &;

    static_assert(std::is_same_v<ref<int>, const int &>);
    static_assert(std::is_same_v<ref<int&>, const int &>);
    static_assert(std::is_same_v<ref<int&&>, const int &>);
    static_assert(std::is_same_v<ref<const int>, const int &>);
    static_assert(std::is_same_v<ref<const int&>, const int &>);
    static_assert(std::is_same_v<ref<const int&&>, const int &>);

    static_assert(std::is_same_v<ref_mut<int>, int &>);
    static_assert(std::is_same_v<ref_mut<int&>, int &>);
    static_assert(std::is_same_v<ref_mut<int&&>, int &>);
    static_assert(std::is_same_v<ref_mut<const int>, int &>);
    static_assert(std::is_same_v<ref_mut<const int&>, int &>);
    static_assert(std::is_same_v<ref_mut<const int&&>, int &>);

    using byte = unsigned char;
    using p_cbyte = const byte *;
    using p_byte = byte *;

    constexpr auto BYTE_BITS = 8;

    inline constexpr const char *project_path() {
        return PROJECT_PATH;
    }

    inline constexpr const char *test_data_path() {
        return TEST_DATA_PATH;
    }

    inline constexpr const char *temp_path() {
        return TEMP_PATH;
    }

    constexpr auto ceil_div(auto a, auto b) {
        return (a + b - 1) / b;
    }

    constexpr size_t mask(size_t bits) {
        return (1 << bits) - 1;
    }

    constexpr size_t bits_part(size_t bits, size_t offset, size_t cnt) {
        return (mask(cnt) & bits) << offset;
    }

    template<typename T>
    constexpr auto enabled(T bits, auto opt) {
        return (bits & static_cast<T>(opt)) != 0;
    }

    template<typename T>
    void print_map(const T& map) {
        spdlog::debug("[MAP] map size: {}", map.size());
        for (let_ref [key, val] : map) {
            spdlog::debug("key: {}, value: {}", key, val);
        }
    }
}

#endif //ZPODS_PCH_H
