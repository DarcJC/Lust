#include "lexer.hpp"

#include <unordered_map>

namespace lust
{
namespace lexer 
{
    Tokenizer::Tokenizer(std::string_view in_text)
        : m_text_to_parse(in_text)
        , m_text_cursor(0)
    { }

    const std::string_view Tokenizer::original_text() const
    {
        return m_text_to_parse;
    }

    Token Tokenizer::next_token()
    {
        // Consuming whitespace at the start
        consume_whitespace();

        if (!is_cursor_valid()) {
            return make_token(TerminalTokenType::END, "" );
        }

        char current = current_char();

        // Check for EOF
        if (current == EOF) {
            return make_token(TerminalTokenType::END, "" );
        }

        // Check for keywords and identifiers
        if (lust_is_alpha(current) || current == '_') {
            return identifier_or_keyword();
        }

        // Check for digits (integer and float literals)
        if (lust_is_digit(current)) {
            return number_literal();
        }

        // Check for newline
        if (current_char() == '\r' || current_char() == '\n') {
            return newline();
        }

        // Check for single character tokens and operators
        switch (current) {
            case '=':
                return match_next('=') ? make_token(TerminalTokenType::EQEQ, "==") : make_token(TerminalTokenType::EQ, "=");
            case '!':
                return match_next('=') ? make_token(TerminalTokenType::NEQ, "!=") : make_token(TerminalTokenType::NOT, "!");
            case '<':
                return match_next('=') ? make_token(TerminalTokenType::LTE, "<=") : make_token(TerminalTokenType::LT, "<");
            case '>':
                return match_next('=') ? make_token(TerminalTokenType::GTE, ">=") : make_token(TerminalTokenType::GT, ">");
            case '+':
                return match_next('+') ? make_token(TerminalTokenType::PLUSPLUS, "++") : make_token(TerminalTokenType::PLUS, "+");
            case '-':
                return match_next('-') ? make_token(TerminalTokenType::MINUSMINUS, "--") : make_token(TerminalTokenType::MINUS, "-");
            case '*':
                return match_next('*') ? make_token(TerminalTokenType::STARSTAR, "**") : make_token(TerminalTokenType::STAR, "*");
            case '/':
                return match_next('/') ? comment() : make_token(TerminalTokenType::SLASH, "/");
            case '(':
                return make_token(TerminalTokenType::LPAREN, "(");
            case ')':
                return make_token(TerminalTokenType::RPAREN, ")");
            case '{':
                return make_token(TerminalTokenType::LBRACE, "{");
            case '}':
                return make_token(TerminalTokenType::RBRACE, "}");
            case ';':
                return make_token(TerminalTokenType::SEMICOLON, ";");
            case ':':
                return make_token(TerminalTokenType::COLON, ":");
            case '.':
                return match_next('.') ? 
                        match_next('=') ? make_token(TerminalTokenType::RANGEEQ, "..=") : make_token(TerminalTokenType::RANGE, "..")
                    : make_token(TerminalTokenType::DOT, ".");
            case ',':
                return make_token(TerminalTokenType::COMMA, ",");
            case '\'':
                return make_token(TerminalTokenType::SQ, "'");
            case '"':
                return string_literal();
            case '[':
                return make_token(TerminalTokenType::LBRACKET, "[");
            case ']':
                return make_token(TerminalTokenType::RBRACKET, "]");
            case '|':
                return match_next('|') ? make_token(TerminalTokenType::OR, "||") : make_token(TerminalTokenType::BITOR, "|");
            case '&':
                return match_next('&') ? make_token(TerminalTokenType::AND, "&&") : make_token(TerminalTokenType::BITAND, "&");
            case '^':
                return make_token(TerminalTokenType::BITXOR, "^");
            case '~':
                return make_token(TerminalTokenType::BITINV, "~");
            case '\r':
            case '\n':
                return newline();
            default:
                return error_token("Unexpected character");
        }

        return { TerminalTokenType::ERROR, "Unexpected character" };
    }

    bool Tokenizer::is_cursor_valid() const
    {
        return m_text_cursor < m_text_to_parse.size() && m_text_cursor >= 0;
    }

    char Tokenizer::current_char() const
    {
        return is_cursor_valid() ? m_text_to_parse[m_text_cursor] : EOF;
    }

    char Tokenizer::next_char()
    {
        ++m_text_cursor;

        if (is_cursor_valid()) {
            return m_text_to_parse[m_text_cursor];
        }

        return EOF;
    }

    void Tokenizer::consume_whitespace()
    {
        while (is_cursor_valid()) {
            if (lust_is_space(current_char())) {
                next_char();
            } else if (current_char() == '/' && m_text_to_parse[m_text_cursor + 1] == '/') {
                break;
            } else {
                break;
            }
        }
    }

    TerminalTokenType Tokenizer::lookup_keyword(std::string_view text)
    {
        static const std::unordered_map<std::string, TerminalTokenType> keywords = {
            { "let", TerminalTokenType::LET },
            { "const", TerminalTokenType::CONST },
            { "mut", TerminalTokenType::MUT },
            { "fn", TerminalTokenType::FN },
            { "struct", TerminalTokenType::STRUCT },
            { "trait", TerminalTokenType::TRAIT },
            { "impl", TerminalTokenType::IMPL },
            { "for", TerminalTokenType::FOR },
            { "type", TerminalTokenType::TYPE },
            { "if", TerminalTokenType::IF },
            { "else", TerminalTokenType::ELSE },
            { "loop", TerminalTokenType::LOOP },
            { "while", TerminalTokenType::WHILE },
            { "break", TerminalTokenType::BREAK },
            { "continue", TerminalTokenType::CONTINUE },
            { "in", TerminalTokenType::IN },
            { "enum", TerminalTokenType::ENUM },
            { "or", TerminalTokenType::OR },
            { "and", TerminalTokenType::AND },
            { "not", TerminalTokenType::NOT }
        };
        if (auto it = keywords.find(std::string(text)); it != keywords.end()) {
            return it->second;
        }

        return TerminalTokenType::IDENT;
    }

    bool Tokenizer::match_next(char expected)
    {
        if (is_cursor_valid() && m_text_to_parse[m_text_cursor + 1] == expected) {
            m_text_cursor++;
            return true;
        }
        return false;
    }

    bool Tokenizer::match_next(std::string_view expected)
    {
        if (m_text_cursor + expected.size() >= m_text_to_parse.size()) {
            return false;
        }

        size_t start = m_text_cursor;
        for (int i = 0; i < expected.size(); ++i) {
            if (expected[i] != m_text_to_parse[start + i]) {
                return false;
            }
        }

        m_text_cursor += expected.size();
        return true;
    }

    Token Tokenizer::error_token(const std::string &message)
    {
        return make_token(TerminalTokenType::ERROR, message);
    }

    Token Tokenizer::make_token(TerminalTokenType type, std::string_view val)
    {
        return { type, std::string(val) };
    }

    Token Tokenizer::identifier_or_keyword()
    {
        size_t start = m_text_cursor;

        while (is_cursor_valid() && (lust_is_alnum(current_char()) || current_char() == '_')) {
            next_char();
        }

        std::string_view text = m_text_to_parse.substr(start, m_text_cursor - start);
        TerminalTokenType type = lookup_keyword(text);

        return make_token(type, text);
    }

    Token Tokenizer::number_literal()
    {
        size_t start = m_text_cursor;
        bool is_float = false;

        while (is_cursor_valid() && lust_is_digit(current_char())) {
            next_char();
        }

        // Check for a floating-point number
        if (current_char() == '.' && lust_is_digit(next_char())) {
            while (is_cursor_valid() && lust_is_digit(current_char())) {
                next_char();
            }
            is_float = true;
        }

        std::string_view text = m_text_to_parse.substr(start, m_text_cursor - start);

        if (is_float) {
            return make_token(TerminalTokenType::FLOAT, text);
        } else {
            return make_token(TerminalTokenType::INT, text);
        }
    }

    Token Tokenizer::string_literal()
    {
        // Skip the opening quote
        size_t start = ++m_text_cursor;

        while (is_cursor_valid() && current_char() != '"') {
            next_char();
        }

        if (!is_cursor_valid()) {
            return error_token("Unterminated string literal");
        }

        std::string_view text = m_text_to_parse.substr(start, m_text_cursor - start);
        // Skip the closing quote
        next_char();

        return make_token(TerminalTokenType::STRING, text);
    }

    Token Tokenizer::newline()
    {
        int substr_len = 1;
        if (match_next('\r') || match_next('\n')) {
            substr_len = 2;
        }
        return make_token(TerminalTokenType::NEWLINE, m_text_to_parse.substr(m_text_cursor, substr_len));
    }

    Token Tokenizer::comment()
    {
        size_t start = m_text_cursor;

        while (is_cursor_valid() && current_char() != '\n') {
            next_char();
        }

        std::string_view text = m_text_to_parse.substr(start, m_text_cursor - start);

        return make_token(TerminalTokenType::COMMENTVAL, text);
    }
}
}

