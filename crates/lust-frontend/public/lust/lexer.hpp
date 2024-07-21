#pragma once

#include <cstdint>
#include <string_view>
#include "container/simple_string.hpp"
#include "lustfrontend_export.h"

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

        // self
        SELF,
        // #[
        ATTRIBUTE_START,
        // #!
        GLOBAL_ATTRIBUTE_START,
        // async
        ASYNC,
        // await
        AWAIT,
        // pub
        PUB,
        // crate
        CRATE,
        // super
        SUPER,
        // mod
        MOD,
        // as
        AS,
        // static
        STATIC,
        // ref
        REF,
        // true
        TRUE,
        // false
        FALSE,

        // +=
        PLUS_EQUAL,
        // -=
        MINUS_EQUAL,
        // *=
        STAR_EQUAL,
        // /=
        SLASH_EQUAL,
        // %=
        PRECENTAGE_EQUAL,
        // &=
        AND_EQUAL,
        // |=
        OR_EQUAL,
        // ^=
        XOR_EQUAL,
        // %
        PRECENTAGE,

        // return
        RETURN,

        // error
        ERROR,
        MAX_NUM,
    };

    LUSTFRONTEND_API extern const char* token_type_to_string(TerminalTokenType type);

    LUSTFRONTEND_API extern const bool is_assignment_token(TerminalTokenType token_type);

    LUSTFRONTEND_API extern const bool is_unary_token(TerminalTokenType token_type);

    struct SourceLoc {
        lust::simple_string filename = "<eval>";
        lust::simple_string function_name = "<anonymous>";
        int64_t line = -1;
        int64_t row = -1;
    };

    extern SourceLoc pos_to_line_and_row(std::string_view full_text, int64_t pos);

    /**
     * @warning Don't try to destruct this struct crossing binary boundary because using STL here.
     */
    struct Token {
        TerminalTokenType type;
        lust::simple_string value;
        int64_t pos;

        /**
         * @note Use this interface to ensure ABI compatibility
         */
        LUSTFRONTEND_API const char* get_value() const;
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