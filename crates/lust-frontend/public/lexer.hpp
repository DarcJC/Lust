#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <string_view>

namespace lust {

namespace lexer {

    class ITokenizer;

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
        // ::
        COLONCOLON,
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

    class LUSTFRONTEND_API TokenStream final {
    public:
        TokenStream(ITokenizer* data_src);
        ~TokenStream();
        TokenStream& operator=(ITokenizer* data_src) noexcept;

        TokenStream(const TokenStream&) = delete;
        TokenStream& operator=(const TokenStream&) = delete;

        TokenStream(TokenStream&& other);
        TokenStream& operator=(TokenStream&& other) noexcept;

        ITokenizer* operator->();

    private:
        ITokenizer* m_data_src = nullptr;
    };


    class LUSTFRONTEND_API ITokenizer {
    public:
        virtual ~ITokenizer() = default;

        static TokenStream create(std::string_view in_text);

        virtual const std::string_view original_text() const = 0;

        virtual Token next_token() = 0;

        virtual bool is_cursor_valid() const = 0;

        /**
         * @brief Lookahead 1 token type state
         */
        virtual TerminalTokenType get_pervious_token_type() const = 0;
    };

}
}