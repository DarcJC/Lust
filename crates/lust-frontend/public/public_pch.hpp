#pragma once

#include "lustfrontend_export.h"

#include <cstddef>
#include <cstdint>
#include <cctype>

extern "C" {

    bool lust_is_space(char c);
    bool lust_is_alpha(char c);
    bool lust_is_digit(char c);
    bool lust_is_alnum(char c);

    uint32_t lust_decode_utf8(const char* str, size_t& i, bool& success);

}
