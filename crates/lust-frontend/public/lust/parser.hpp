#pragma once

#include "lexer.hpp"
#include "grammar.hpp"
#include "container/unique_ptr.hpp"

namespace lust
{
namespace grammar
{

    class LUSTFRONTEND_API IParser {
    public:
        virtual ~IParser() = default;

        static lust::UniquePtr<IParser> create(lexer::TokenStream& token_stream);

        /**
         * Starting to parse token stream into AST
         */
        virtual lust::UniquePtr<ASTNode_Program> parse() = 0;

        /**
         * Check does error occurred during parsing
         */
        virtual bool is_error_occurred() const = 0;
    };

}
}
