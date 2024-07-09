#pragma once

#include "lexer.hpp"

namespace lust
{
namespace grammar
{

    class IASTNode {
    protected:
        virtual ~IASTNode();

    };

    class ASTBaseNode : public IASTNode { };

    class ASTNode_Program : public ASTBaseNode {
    };

    class IParser {
    public:
        virtual ~IParser() = default;

        static IParser* create(lexer::TokenStream& token_stream);

        /**
         * Starting to parse token stream into AST
         */
        virtual ASTNode_Program parse() = 0;
    };

}
}
