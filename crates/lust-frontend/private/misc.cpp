#include "public_pch.hpp"

#include <cctype>
#include <string>

extern "C" {

    bool lust_is_space(char c) {
        return std::isspace(static_cast<unsigned char>(c));
    }

    bool lust_is_alpha(char c)
    {
        auto cc = static_cast<unsigned char>(c);
        return !(std::isspace(cc) || std::ispunct(cc) || std::iscntrl(cc));
    }

    bool lust_is_digit(char c)
    {
        return std::isdigit(static_cast<unsigned char>(c));
    }

    bool lust_is_alnum(char c)
    {
        return std::isalnum(static_cast<unsigned char>(c));
    }

    uint32_t lust_decode_utf8(const char *s, size_t &i, bool& success)
    {
        std::string str(s);

        uint32_t codepoint = 0;
        size_t extraBytes = 0;

        unsigned char ch = str[i];
        if (ch <= 0x7F) {
            codepoint = ch;
        } else if (ch <= 0xBF) {
            success = false;
            return 0;
        } else if (ch <= 0xDF) {
            codepoint = ch & 0x1F;
            extraBytes = 1;
        } else if (ch <= 0xEF) {
            codepoint = ch & 0x0F;
            extraBytes = 2;
        } else if (ch <= 0xF7) {
            codepoint = ch & 0x07;
            extraBytes = 3;
        } else {
            success = false;
            return 0;
        }

        while (extraBytes > 0) {
            ++i;
            if (i >= str.size()) {
                success = false;
                return 0;
            }
            ch = str[i];
            if ((ch & 0xC0) != 0x80) {
                success = false;
                return 0;
            }
            codepoint = (codepoint << 6) | (ch & 0x3F);
            --extraBytes;
        }
        success = true;
        return codepoint;
    }
}
