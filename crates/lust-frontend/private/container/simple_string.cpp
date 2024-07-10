#include "container/simple_string.hpp"

#include <cstring>
#include <type_traits>
#include <memory>

namespace lust
{
    simple_string::simple_string() = default;

    simple_string::simple_string(const char* s) {
        if (nullptr != s) {
            m_length = strlen(s);
            m_data = new char[m_length + 1];
            strcpy(m_data, s);
        }
    }

    simple_string::simple_string(std::string_view s)
    {
        if (!s.empty()) {
            m_length = s.size();
            m_data = new char[m_length + 1];
            strncpy(m_data, s.data(), m_length);
            m_data[m_length] = '\0';
        }
    }

    simple_string::simple_string(const simple_string &other)
    {
        if (!other.is_empty()) {
            m_length = other.m_length;
            m_data = new char[m_length + 1];
            strcpy(m_data, other.m_data);
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
        simple_string(std::forward<simple_string>(other)).swap(*this);
        return *this;
    }

    simple_string::~simple_string()
    {
        delete m_data;
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
}
