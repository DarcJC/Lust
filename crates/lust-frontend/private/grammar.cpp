#include "grammar.hpp"


namespace lust
{
namespace grammar
{
    IASTNode::~IASTNode() = default;

    class Parser : public IParser {
    public:
        Parser(lexer::TokenStream& token_stream);

        ASTNode_Program parse() override;

    private:
        lexer::TokenStream& m_token_stream;
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
}
}
