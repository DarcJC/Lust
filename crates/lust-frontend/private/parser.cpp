#include "parser.hpp"

#include <string_view>

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

        ASTNode_Program parse() override;

    protected:
        void error(std::string_view msg);

    private:
        lexer::TokenStream& m_token_stream;
        lexer::Token m_current_token{};

        bool m_error_occurred = false;
    };

    IParser *IParser::create(lexer::TokenStream &token_stream)
    {
        return new Parser(token_stream);
    }

    Parser::Parser(lexer::TokenStream &token_stream)
        : m_token_stream(token_stream)
    {
    }

    ASTNode_Program Parser::parse()
    {
        return ASTNode_Program();
    }

    void Parser::error(std::string_view msg)
    {
        m_error_occurred = true;
    }
}
}
