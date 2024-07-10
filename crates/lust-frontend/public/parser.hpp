#pragma once

#include "grammar.hpp"


namespace lust
{
namespace grammar
{

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
