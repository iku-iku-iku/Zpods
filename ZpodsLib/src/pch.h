//
// Created by code4love on 23-9-23.
//

#ifndef ZPODS_PCH_H
#define ZPODS_PCH_H

#include <type_traits>

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

    #define let const auto
    #define let_mut auto

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

    inline constexpr const char *project_path() {
        return PROJECT_PATH;
    }

    inline constexpr const char *test_data_path() {
        return TEST_DATA_PATH;
    }

    inline constexpr const char *temp_path() {
        return TEMP_PATH;
    }
}

#include "spdlog/spdlog.h"
#include <cstdint>
#include <string>
#include <vector>
#include <ranges>
#include <numeric>
#include <fstream>

#include "enum.h"



#endif //ZPODS_PCH_H
