#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <string_view>

namespace lust {

namespace lexer {

    enum class TerminalTokenType : uint32_t {
        NONE,
        // let
        LET,
        // const
        CONST,
        // mut
        MUT,
        // fn
        FN,
        // struct
        STRUCT,
        // trait
        TRAIT,
        // impl
        IMPL,
        // for
        FOR,
        // type
        TYPE,
        // if
        IF,
        // else
        ELSE,
        // loop
        LOOP,
        // while
        WHILE,
        // break
        BREAK,
        // continue
        CONTINUE,
        // in
        IN,
        // enum
        ENUM,
        // ..
        RANGE,
        // ..=
        RANGEEQ,
        // Identify Name
        IDENT,
        // =
        EQ,
        // (
        LPAREN,
        // )
        RPAREN,
        // {
        LBRACE,
        // }
        RBRACE,
        // ;
        SEMICOLON,
        // :
        COLON,
        // ->
        ARROW,
        // ,
        COMMA,
        // #
        HASH,
        // .
        DOT,
        // '
        SQ,
        // "
        DQ,
        // [
        LBRACKET,
        // ]
        RBRACKET,
        // EOF
        END,
        // '||' or 'or'
        OR,
        // && or and
        AND,
        // ! or not
        NOT,
        // ==
        EQEQ,
        // !=
        NEQ,
        // <
        LT,
        // <=
        LTE,
        // >
        GT,
        // >=
        GTE,
        // +
        PLUS,
        // ++
        PLUSPLUS,
        // -
        MINUS,
        // --
        MINUSMINUS,
        // *
        STAR,
        // **
        STARSTAR,
        // /
        SLASH,
        // Comment
        COMMENT,
        // |
        BITOR,
        // ^
        BITXOR,
        // &
        BITAND,
        // ~
        BITINV,
        // int literal
        INT,
        // float literal
        FLOAT,
        // string literal
        STRING,

        // new line character sequence \n \r\n \r
        NEWLINE,

        // comment value
        COMMENTVAL,

        // error
        ERROR,
        MAX_NUM,
    };

    extern const char* token_type_to_string(TerminalTokenType type);

    struct Token {
        TerminalTokenType type;
        std::string value;
    };

    class LUSTFRONTEND_API Tokenizer {
    public:
        Tokenizer(std::string_view in_text);

        const std::string_view original_text() const;

        Token next_token();

        bool is_cursor_valid() const;

        /**
         * @brief Lookahead 1 token type state
         */
        TerminalTokenType get_pervious_token_type() const;

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

        static Token error_token(const std::string& message);

        static Token make_token(TerminalTokenType type, std::string_view val);

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

}
}