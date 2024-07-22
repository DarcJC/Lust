#pragma once

#include <string_view>
#include <ostream>

namespace lust
{
    // Small String Optimization on stack(also can be data segment and so on) memory size
    // Notice it memory usage will be SSO_BUFFER_SIZE + 1 because we need space to store '\0'
    constexpr size_t SSO_BUFFER_SIZE = 15;

    class LUSTFRONTEND_API simple_string {
    public:
        simple_string();
        simple_string(const char* s);
        simple_string(std::string_view s);

        simple_string(const simple_string& other);
        simple_string(simple_string&& other);

        simple_string& operator=(const simple_string& other) noexcept;
        simple_string& operator=(simple_string&& other) noexcept;

        bool operator==(const simple_string& other) const noexcept;
        bool operator!=(const simple_string& other) const noexcept;
        
        bool operator==(const char* other) const noexcept;
        bool operator!=(const char* other) const noexcept;

        virtual ~simple_string();

        bool is_empty() const noexcept;
        void swap(simple_string& other);

        operator std::string_view() noexcept;
        operator const char*() const noexcept;
        operator char*() noexcept;

        char* data();
        const char* data() const;
        size_t length();
        void ensure_capacity(size_t new_length);

        simple_string& append(const char* s);
        simple_string& append(std::string_view s);

        simple_string& operator+=(const char* s);
        simple_string& operator+=(std::string_view s);
        simple_string& operator+=(const simple_string& other);

    private:
        union {
            char m_ss_buffer[SSO_BUFFER_SIZE + 1]; // internal buffer
            char* m_heap_data; // allocated by memory allocator
        };
        size_t m_length = 0;
        size_t m_capacity = SSO_BUFFER_SIZE;
        bool m_is_heap = false;

        void set_data(const char* s, size_t len);

    };

    inline std::ostream& operator<<(std::ostream& os, const lust::simple_string& str) {
        if (!str.is_empty()) {
            os << str.operator const char *();
        }

        return os;
    }
}

