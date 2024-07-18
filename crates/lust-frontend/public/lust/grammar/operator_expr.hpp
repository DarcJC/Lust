#pragma once
#include "grammar.hpp"
#include "lustfrontend_export.h"

namespace lust
{
namespace grammar
{
    enum class OperatorType : uint32_t {
        INVALID,

        ASSIGNMENT,
        LOGICAL_OR,
        LOGICAL_AND,
        LOGICAL_EQUALITY,
        LOGICAL_NONE_EQUALITY,
        LOGICAL_RELATION_LESS_THAN,
        LOGICAL_RELATION_LESS_THAN_EQUALITY,
        LOGICAL_RELATION_GREATER_THAN,
        LOGICAL_RELATION_GREATER_THAN_EQUALITY,
        ARITHMETIC_ADD,
        ARITHMETIC_SUBTRACT,
        ARITHMETIC_MULTIPLY,
        ARITHMETIC_DIVIDE,
        BITWISE_OR,
        BITWISE_XOR,
        BITWISE_AND,
        ARITHMETIC_EXPONENT,
        UNARY_ARITHMETIC_MINUS,
        UNARY_ARITHMETIC_SELF_INCREASE,
        UNARY_ARITHMETIC_SELF_DECREASE,
        UNARY_LOGICAL_NOT,
        UNARY_BITWISE_INVERSE,
        LITERAL_INTEGER,
        LITERAL_FLOAT,
        LITERAL_STRING,
        LITERAL_CHAR,

        MAX_NUM,
    };

    LUSTFRONTEND_API extern const char* operator_type_to_name(OperatorType type);

    struct ASTNode_Expr : public ASTBaseNode<GrammarRule::EXPRESSION, ASTNode_Statement> {
        vector<const IASTNode*> collect_self_nodes() const override;
    };

    struct ASTNode_IntegerExpr : public ASTBaseNode<GrammarRule::INTEGER_LITERAL, ASTNode_Expr> {
    };

    struct ASTNode_FloatExpr : public ASTBaseNode<GrammarRule::FLOAT_LITERAL, ASTNode_Expr> {
    };

    struct ASTNode_StringExpr : public ASTBaseNode<GrammarRule::STRING_LITERAL, ASTNode_Expr> {
    };

    struct ASTNode_Operator : public ASTBaseNode<GrammarRule::OPERATOR, ASTNode_Expr> {
        OperatorType operator_type;
        UniquePtr<ASTNode_Expr> left_oprand;
        UniquePtr<ASTNode_Expr> right_oprand;

        vector<const IASTNode*> collect_self_nodes() const override;
        simple_string get_name() const override;
    };
}
}
