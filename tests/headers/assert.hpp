#pragma once

#include <format>
#include <iostream>
#include <stdexcept>
#include <string_view>
#include <string>

struct TestError : std::runtime_error {
    TestError() : std::runtime_error("Test error occurred") {}
    TestError(std::string_view w) : std::runtime_error(std::string(w)) {}
};


#define TEST_CHECK_OK_MSG(expr, ...) do {\
        if (!(expr)) { \
            std::cerr << "Error message at " << __FILE__ << ":" << __LINE__ << "\n\t" << std::format(__VA_ARGS__) << std::endl; \
            throw TestError(); \
        } \
    } while (false);


#define TEST_MUST_BE_FALSE_MSG(expr, ...) do {\
        if ((expr)) { \
            std::cerr << "Error message at " << __FILE__ << ":" << __LINE__ << "\n\t" << std::format(__VA_ARGS__) << std::endl; \
            throw TestError(); \
        } \
    } while (false);
