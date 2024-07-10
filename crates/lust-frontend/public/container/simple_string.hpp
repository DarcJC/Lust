#pragma once

#include <string_view>

namespace lust
{
    class simple_string {
    public:
        simple_string();
        simple_string(const char* s);
        simple_string(std::string_view s);

        simple_string(const simple_string& other);
        simple_string(simple_string&& other);

        simple_string& operator=(const simple_string& other) noexcept;
        simple_string& operator=(simple_string&& other) noexcept;

        virtual ~simple_string();

        bool is_empty() const noexcept;

        void swap(simple_string& other);

        operator std::string_view() noexcept;
        operator const char*() const noexcept;
        operator char*() noexcept;

        char* data();

    private:
        char* m_data = nullptr;
        size_t m_length = 0;

    };
}
