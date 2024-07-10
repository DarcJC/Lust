#include "parser.hpp"

#include <string_view>
#include <iostream>
#include <sstream>

namespace lust
{
namespace grammar
{
    /**
     * A LL(1) parser
     */
    class Parser : public IParser {
    public:
        Parser(lexer::TokenStream& token_stream);

        lust::UniquePtr<ASTNode_Program> parse() override;

    private:
        /**
         * @brief Notify a parser error and set parser error flag
         */
        void error(std::string_view msg);

        lexer::Token next_token();

        /**
         * @brief The token consumer
         */
        void expected(lexer::TerminalTokenType expected_type);

    private:
        lexer::TokenStream& m_token_stream;
        lexer::Token m_current_token{};

        bool m_error_occurred = false;

    private:
        UniquePtr<ASTNode_Program> parse_program();
    };

    lust::UniquePtr<IParser> IParser::create(lexer::TokenStream &token_stream)
    {
        return UniquePtr<IParser>(new Parser(token_stream));
    }

    Parser::Parser(lexer::TokenStream &token_stream)
        : m_token_stream(token_stream)
    {
    }

    lust::UniquePtr<ASTNode_Program> Parser::parse()
    {
        return parse_program();
    }

    void Parser::error(std::string_view msg)
    {
        m_error_occurred = true;
        lexer::SourceLoc loc = lexer::pos_to_line_and_row(m_token_stream->original_text(), m_current_token.pos);
        std::cerr << "Error occurred while parsing at L" << loc.line << ":" << loc.row << " :\n\t" << msg;
    }

    lexer::Token Parser::next_token()
    {
        return m_token_stream->next_token();
    }

    void Parser::expected(lexer::TerminalTokenType expected_type)
    {
        if (m_current_token.type == expected_type) {
            m_current_token = next_token();
        } else {
            std::stringstream s;
            s << "Unexpected token. Expected " << lexer::token_type_to_string(expected_type) << ", found " << lexer::token_type_to_string(m_current_token.type) << ".";
            error(s.str());
        }
    }

    UniquePtr<ASTNode_Program> Parser::parse_program()
    {
        return nullptr;
    }
}
}
