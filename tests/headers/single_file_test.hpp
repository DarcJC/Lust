#pragma once

#include <stdexcept>
#include <iostream>
#include <cstdlib>
#include <cstdint>
#include <cstddef>
#include <string_view>
#include <string>
#include <algorithm>
#include <vector>
#include <map>
#include <unordered_map>

void entry();

struct TestError : std::runtime_error {
    using std::runtime_error::runtime_error;
};

int main() {
    try {
        entry();
    } catch (TestError& err) {
        std::cerr << "An error occurred during testing '" << LUST_TEST_NAME << "' :\n\t";
        std::cerr << err.what() << std::endl;
        return 1;
    }

    std::cout << "Success with test '" << LUST_TEST_NAME << "'" << std::endl;
    return 0;
}
