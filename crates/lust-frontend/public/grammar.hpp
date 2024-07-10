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

}
}
