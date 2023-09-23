//
// Created by code4love on 23-9-23.
//

#ifndef ZPODS_PCH_H
#define ZPODS_PCH_H

namespace zpods {
    #define let const auto
    #define let_mut auto

    template<typename T>
    using ref = const T &;

    template<typename T>
    using ref_mut = T &;

    using byte = unsigned char;
    using p_cbyte = const byte *;
    using p_byte = byte *;
}

#include "spdlog/spdlog.h"

#include <cstdint>
#include <string>
#include <vector>

#include "enum.h"


#endif //ZPODS_PCH_H
