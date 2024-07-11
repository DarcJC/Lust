#include "lexer.hpp"

#include <unordered_map>

namespace lust
{
namespace lexer 
{
    class Tokenizer : public ITokenizer {
    public:
        Tokenizer(std::string_view in_text);

        const std::string_view original_text() const override;

        Token next_token() override;

        bool is_cursor_valid() const override;

        /**
         * @brief Lookahead 1 token type state
         */
        TerminalTokenType get_pervious_token_type() const override;

    protected:
        char current_char() const;

        /**
         * @brief eat next char
         */
        char next_char();

        /**
         * @brief eat space, \\n, \v, \f, \r, \t
         */
        void consume_whitespace();

        /**
         * @brief Lookup the keyword table. If not found, it will be IDENT.
         */
        TerminalTokenType lookup_keyword(std::string_view text);

        /**
         * Inv lookahead
         * @brief if matched then eat, else do nothing
         */
        bool match_next(char expected);

        /**
         * Inv lookahead
         * @brief if matched then eat, else do nothing
         */
        bool match_next(std::string_view expected);

        Token error_token(std::string_view message);

        Token make_token(TerminalTokenType type, std::string_view val);

    private:
        // UTF-8 encoded string, be careful when using a single char
        std::string m_text_to_parse;

        // Cursor pointing to current char
        int64_t m_text_cursor = 0;

        // Previous token type
        TerminalTokenType m_previous_token_type = TerminalTokenType::NONE;

    protected:
        // IDENT / Keyword
        Token identifier_or_keyword();

        // Number
        Token number_literal();

        // String
        Token string_literal();

        // Newline
        Token newline();

        // Comment
        Token comment();

    };

    Tokenizer::Tokenizer(std::string_view in_text)
        : m_text_to_parse(in_text)
        , m_text_cursor(0)
        , m_previous_token_type(TerminalTokenType::NONE)
    { }

    const std::string_view Tokenizer::original_text() const
    {
        return m_text_to_parse;
    }

    Token Tokenizer::next_token()
    {
        // Consuming whitespace at the start
        consume_whitespace();

        Token token;

        char current = current_char();

        // Check for EOF
        if (current == EOF || !is_cursor_valid()) {
            token = make_token(TerminalTokenType::END, "" );
            goto token_exit;
        }

        // Check for digits (integer and float literals)
        if (lust_is_digit(current)) {
            token = number_literal();
            goto token_exit;
        }

        // Check for keywords and identifiers
        if (lust_is_alpha(current) || current == '_') {
            token = identifier_or_keyword();
            goto token_exit;
        }

        // Check for newline
        // Useless at the meanwhile because consume_whitespace ate them.
        if (current_char() == '\r' || current_char() == '\n') {
            token = newline();
            goto token_exit;
        }

        // Check for single character tokens and operators
        switch (current) {
            case '=':
                token = match_next('=') ? make_token(TerminalTokenType::EQEQ, "==") : make_token(TerminalTokenType::EQ, "=");
                break;
            case '!':
                token = match_next('=') ? make_token(TerminalTokenType::NEQ, "!=") : make_token(TerminalTokenType::NOT, "!");
                break;
            case '<':
                token = match_next('=') ? make_token(TerminalTokenType::LTE, "<=") : make_token(TerminalTokenType::LT, "<");
                break;
            case '>':
                token = match_next('=') ? make_token(TerminalTokenType::GTE, ">=") : make_token(TerminalTokenType::GT, ">");
                break;
            case '+':
                token = match_next('+') ? make_token(TerminalTokenType::PLUSPLUS, "++") : make_token(TerminalTokenType::PLUS, "+");
                break;
            case '-':
                token = match_next('-') ? make_token(TerminalTokenType::MINUSMINUS, "--") 
                    : match_next('>') ? make_token(TerminalTokenType::ARROW, "->") : make_token(TerminalTokenType::MINUS, "-");
                break;
            case '*':
                token = match_next('*') ? make_token(TerminalTokenType::STARSTAR, "**") : make_token(TerminalTokenType::STAR, "*");
                break;
            case '/':
                token = match_next('/') ? comment() : make_token(TerminalTokenType::SLASH, "/");
                break;
            case '(':
                token = make_token(TerminalTokenType::LPAREN, "(");
                break;
            case ')':
                token = make_token(TerminalTokenType::RPAREN, ")");
                break;
            case '{':
                token = make_token(TerminalTokenType::LBRACE, "{");
                break;
            case '}':
                token = make_token(TerminalTokenType::RBRACE, "}");
                break;
            case ';':
                token = make_token(TerminalTokenType::SEMICOLON, ";");
                break;
            case ':':
                token = match_next(':') ? make_token(TerminalTokenType::COLONCOLON, "::") : make_token(TerminalTokenType::COLON, ":");
                break;
            case '.':
                token = match_next('.') ? 
                        match_next('=') ? make_token(TerminalTokenType::RANGEEQ, "..=") : make_token(TerminalTokenType::RANGE, "..")
                    : make_token(TerminalTokenType::DOT, ".");
                break;
            case ',':
                token = make_token(TerminalTokenType::COMMA, ",");
                break;
            case '\'':
                token = make_token(TerminalTokenType::SQ, "'");
                break;
            case '"':
                token = string_literal();
                break;
            case '[':
                token = make_token(TerminalTokenType::LBRACKET, "[");
                break;
            case ']':
                token = make_token(TerminalTokenType::RBRACKET, "]");
                break;
            case '|':
                token = match_next('|') ? make_token(TerminalTokenType::OR, "||") : make_token(TerminalTokenType::BITOR, "|");
                break;
            case '&':
                token = match_next('&') ? make_token(TerminalTokenType::AND, "&&") : make_token(TerminalTokenType::BITAND, "&");
                break;
            case '^':
                token = make_token(TerminalTokenType::BITXOR, "^");
                break;
            case '~':
                token = make_token(TerminalTokenType::BITINV, "~");
                break;
            case '#':
                token = match_next('[') ? make_token(TerminalTokenType::ATTRIBUTE_START, "#[") :
                    match_next('!') ? make_token(TerminalTokenType::GLOBAL_ATTRIBUTE_START, "#!") : make_token(TerminalTokenType::HASH, "#");
                break;
            case '\r':
            case '\n':
                token = newline();
                break;
            default:
                token = error_token("Unexpected character");
        }

        if (token.type != TerminalTokenType::ERROR) {
            // match_next() will eat token if matched
            // so we don't need to add `token.value.size()`
            m_text_cursor++;
        }

token_exit:
        m_previous_token_type = token.type;
        return token;
    }

    bool Tokenizer::is_cursor_valid() const
    {
        return m_text_cursor < m_text_to_parse.size() && m_text_cursor >= 0;
    }

    TerminalTokenType Tokenizer::get_pervious_token_type() const
    {
        return m_previous_token_type;
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
            { "not", TerminalTokenType::NOT },
            { "async", TerminalTokenType::ASYNC },
            { "await", TerminalTokenType::AWAIT },
            { "pub", TerminalTokenType::PUB },
            { "crate", TerminalTokenType::CRATE },
            { "super", TerminalTokenType::SUPER },
            { "mod", TerminalTokenType::MOD },
        };
        if (auto it = keywords.find(std::string(text)); it != keywords.end()) {
            return it->second;
        }

        return m_previous_token_type == TerminalTokenType::COLON ? TerminalTokenType::TYPE : TerminalTokenType::IDENT;
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

    Token Tokenizer::error_token(std::string_view message)
    {
        return { TerminalTokenType::ERROR, message, m_text_cursor };
    }

    Token Tokenizer::make_token(TerminalTokenType type, std::string_view val)
    {
        return { type, val, m_text_cursor - static_cast<int64_t>(val.size()) };
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

        bool is_escaped = false;

        while (is_cursor_valid()) {
            char c = current_char();

            if (!is_escaped && c == '"') {
                break;
            } else if (is_escaped) {
                is_escaped = false;
            } else if (c == '\\') {
                is_escaped = true;
            } else {
                is_escaped = false;
            }

            next_char();
        }

        if (!is_cursor_valid() || current_char() != '"') {
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

        std::string val;
        val.reserve(64);

        while (is_cursor_valid() && current_char() != '\n' && current_char() != '\r') {
            char c = next_char();
            if (c != '\r' && c != '\n') {
                val += c;
            }
        }

        // std::string_view text = m_text_to_parse.substr(start, m_text_cursor - start);

        return make_token(TerminalTokenType::COMMENTVAL, val);
    }

    const char *token_type_to_string(TerminalTokenType type)
    {
        switch (type) {
            case TerminalTokenType::NONE: return "NONE";
            case TerminalTokenType::LET: return "LET";
            case TerminalTokenType::CONST: return "CONST";
            case TerminalTokenType::MUT: return "MUT";
            case TerminalTokenType::FN: return "FN";
            case TerminalTokenType::STRUCT: return "STRUCT";
            case TerminalTokenType::TRAIT: return "TRAIT";
            case TerminalTokenType::IMPL: return "IMPL";
            case TerminalTokenType::FOR: return "FOR";
            case TerminalTokenType::TYPE: return "TYPE";
            case TerminalTokenType::IF: return "IF";
            case TerminalTokenType::ELSE: return "ELSE";
            case TerminalTokenType::LOOP: return "LOOP";
            case TerminalTokenType::WHILE: return "WHILE";
            case TerminalTokenType::BREAK: return "BREAK";
            case TerminalTokenType::CONTINUE: return "CONTINUE";
            case TerminalTokenType::IN: return "IN";
            case TerminalTokenType::ENUM: return "ENUM";
            case TerminalTokenType::RANGE: return "RANGE";
            case TerminalTokenType::RANGEEQ: return "RANGEEQ";
            case TerminalTokenType::IDENT: return "IDENT";
            case TerminalTokenType::EQ: return "EQ";
            case TerminalTokenType::LPAREN: return "LPAREN";
            case TerminalTokenType::RPAREN: return "RPAREN";
            case TerminalTokenType::LBRACE: return "LBRACE";
            case TerminalTokenType::RBRACE: return "RBRACE";
            case TerminalTokenType::SEMICOLON: return "SEMICOLON";
            case TerminalTokenType::COLON: return "COLON";
            case TerminalTokenType::COLONCOLON: return "COLONCOLON";
            case TerminalTokenType::ARROW: return "ARROW";
            case TerminalTokenType::COMMA: return "COMMA";
            case TerminalTokenType::HASH: return "HASH";
            case TerminalTokenType::DOT: return "DOT";
            case TerminalTokenType::SQ: return "SQ";
            case TerminalTokenType::DQ: return "DQ";
            case TerminalTokenType::LBRACKET: return "LBRACKET";
            case TerminalTokenType::RBRACKET: return "RBRACKET";
            case TerminalTokenType::END: return "END";
            case TerminalTokenType::OR: return "OR";
            case TerminalTokenType::AND: return "AND";
            case TerminalTokenType::NOT: return "NOT";
            case TerminalTokenType::EQEQ: return "EQEQ";
            case TerminalTokenType::NEQ: return "NEQ";
            case TerminalTokenType::LT: return "LT";
            case TerminalTokenType::LTE: return "LTE";
            case TerminalTokenType::GT: return "GT";
            case TerminalTokenType::GTE: return "GTE";
            case TerminalTokenType::PLUS: return "PLUS";
            case TerminalTokenType::PLUSPLUS: return "PLUSPLUS";
            case TerminalTokenType::MINUS: return "MINUS";
            case TerminalTokenType::MINUSMINUS: return "MINUSMINUS";
            case TerminalTokenType::STAR: return "STAR";
            case TerminalTokenType::STARSTAR: return "STARSTAR";
            case TerminalTokenType::SLASH: return "SLASH";
            case TerminalTokenType::COMMENT: return "COMMENT";
            case TerminalTokenType::BITOR: return "BITOR";
            case TerminalTokenType::BITXOR: return "BITXOR";
            case TerminalTokenType::BITAND: return "BITAND";
            case TerminalTokenType::BITINV: return "BITINV";
            case TerminalTokenType::INT: return "INT";
            case TerminalTokenType::FLOAT: return "FLOAT";
            case TerminalTokenType::STRING: return "STRING";
            case TerminalTokenType::NEWLINE: return "NEWLINE";
            case TerminalTokenType::COMMENTVAL: return "COMMENTVAL";
            case TerminalTokenType::SELF: return "SELF";
            case TerminalTokenType::ATTRIBUTE_START: return "ATTRIBUTE_START";
            case TerminalTokenType::GLOBAL_ATTRIBUTE_START: return "GLOBAL_ATTRIBUTE_START";
            case TerminalTokenType::ASYNC: return "ASYNC";
            case TerminalTokenType::AWAIT: return "AWAIT";
            case TerminalTokenType::PUB: return "PUB";
            case TerminalTokenType::CRATE: return "CRATE";
            case TerminalTokenType::SUPER: return "SUPER";
            case TerminalTokenType::ERROR: return "ERROR";
            case TerminalTokenType::MAX_NUM: return "MAX_NUM";
            default: return "UNKNOWN";
        }
    }

    TokenStream ITokenizer::create(std::string_view in_text)
    {
        return new Tokenizer(in_text);
    }

    TokenStream::TokenStream(ITokenizer *data_src)
        : m_data_src(data_src)
    { }

    TokenStream::~TokenStream()
    {
        delete m_data_src;
    }

    TokenStream &TokenStream::operator=(ITokenizer *data_src) noexcept
    {
        m_data_src = data_src;
        return *this;
    }

    TokenStream::TokenStream(TokenStream &&other)
    {
        m_data_src = other.m_data_src;
        other.m_data_src = nullptr;
    }

    TokenStream &TokenStream::operator=(TokenStream &&other) noexcept
    {
        if (this != &other) {
            m_data_src = other.m_data_src;
            other.m_data_src = nullptr;
        }

        return *this;
    }

    ITokenizer *TokenStream::operator->()
    {
        return m_data_src;
    }

    const char* Token::get_value() const
    {
        return value;
    }

    SourceLoc pos_to_line_and_row(std::string_view full_text, int64_t pos) {
        SourceLoc loc;
        if (pos >= 0 && pos <= static_cast<int64_t>(full_text.size())) {
            int64_t current_line = 0;
            int64_t current_row = 0;

            for (int64_t i = 0; i <= pos; ++i) {
                if (full_text[i] == '\n') {
                    ++current_line;
                    current_row = 0;
                } else {
                    ++current_row;
                }
            }

            loc.line = current_line + 1;
            loc.row = current_row + 1;
        }
        return loc;
    }
}
}

