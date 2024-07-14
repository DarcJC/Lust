#pragma once

#include <string_view>
#include <charconv> // for std::from_chars
#include <concepts> // for concepts
#include <type_traits> // for std::is_arithmetic_v

namespace lust
{
namespace grammar
{

    template <typename T>
    concept Numeric = std::is_arithmetic_v<T>;

    template<Numeric T>
    struct Number {
        bool is_null = false;
        T value = {};
    };

    template <Numeric T>
    Number<T> convert_string_to_number(std::string_view val_to_conv) {
        Number<T> result;
        auto [ptr, ec] = std::from_chars(val_to_conv.data(), val_to_conv.data() + val_to_conv.size(), result.value);

        if (ec == std::errc::invalid_argument || ec == std::errc::result_out_of_range) {
            result.is_null = true;
        }

        return result;
    }
}
}
