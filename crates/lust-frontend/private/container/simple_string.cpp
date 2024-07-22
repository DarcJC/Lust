#include "container/simple_string.hpp"

#include <cstring>
#include <type_traits>

namespace lust
{
    simple_string::simple_string() = default;

    simple_string::simple_string(const char* s) {
        append(s);
    }

    simple_string::simple_string(std::string_view s)
    {
        append(s);
    }

    simple_string::simple_string(const simple_string &other)
    {
        if (!other.is_empty()) {
            append(other.m_data);
        }
    }

    simple_string::simple_string(simple_string &&other)
    {
        m_data = other.m_data;
        m_length = other.m_length;

        other.m_data = nullptr;
        other.m_length = 0;
    }

    simple_string &simple_string::operator=(const simple_string &other) noexcept
    {
        simple_string(other).swap(*this);
        return *this;
    }

    simple_string &simple_string::operator=(simple_string &&other) noexcept
    {
        if (this != &other) {
            simple_string(std::forward<simple_string>(other)).swap(*this);
        }
        return *this;
    }

    simple_string::~simple_string()
    {
        delete[] m_data;
    }

    bool simple_string::is_empty() const noexcept
    {
        return 0 == m_length;
    }

    void simple_string::swap(simple_string &other)
    {
        if (&other != this) {
            auto* other_data = other.m_data;
            auto other_len = other.m_length;

            other.m_data = m_data;
            other.m_length = m_length;

            m_data = other_data;
            m_length = other_len;
        }
    }

    simple_string::operator std::string_view() noexcept
    {
        return std::string_view(m_data, m_length);
    }

    simple_string::operator const char*() const noexcept
    {
        return m_data;
    }

    simple_string::operator char *() noexcept
    {
        return m_data;
    }

    char *simple_string::data()
    {
        return m_data;
    }

    size_t simple_string::length() {
        return m_length;
    }

    void simple_string::ensure_capacity(size_t new_length) {
        if (new_length > m_length) {
            char* new_data = new char[new_length + 1];
            if (m_data) {
                std::memcpy(new_data, m_data, m_length);
                delete[] m_data;
            }
            m_data = new_data;
        }
    }

    simple_string& simple_string::append(const char* s) {
        if (s) {
            size_t new_length = m_length + std::strlen(s);
            ensure_capacity(new_length);
            std::memcpy(m_data + m_length, s, std::strlen(s) + 1);
            m_length = new_length;
        }
        return *this;
    }

    simple_string& simple_string::append(std::string_view s) {
        if (!s.empty()) {
            size_t new_length = m_length + s.size();
            ensure_capacity(new_length);
            std::memcpy(m_data + m_length, s.data(), s.size());
            m_length = new_length;
            m_data[m_length] = '\0';
        }
        return *this;
    }

    simple_string& simple_string::operator+=(const char* s) {
        return append(s);
    }

    simple_string& simple_string::operator+=(std::string_view s) {
        return append(s);
    }

    simple_string& simple_string::operator+=(const simple_string& other) {
        return append(other.m_data);
    }
}
