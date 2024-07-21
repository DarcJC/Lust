#pragma once
#include "container/simple_string.hpp"
#include "container/unique_ptr.hpp"
#include "grammar.hpp"
#include "lustfrontend_export.h"

namespace lust
{
namespace grammar
{
    enum class OperatorType : uint32_t {
        INVALID,

        ASSIGNMENT,
        ASSIGNMENT_ADD,
        ASSIGNMENT_SUBTRACT,
        ASSIGNMENT_MULTIPLY,
        ASSIGNMENT_DIVIDE,
        ASSIGNMENT_BITWISE_OR,
        ASSIGNMENT_BITWISE_XOR,
        ASSIGNMENT_BITWISE_AND,
        ASSIGNMENT_MOD,
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
        ARITHMETIC_MOD,
        BITWISE_OR,
        BITWISE_XOR,
        BITWISE_AND,
        ARITHMETIC_EXPONENT,
        UNARY_ARITHMETIC_MINUS,
        UNARY_ARITHMETIC_SELF_INCREASE,
        UNARY_ARITHMETIC_SELF_DECREASE,
        UNARY_ARITHMETIC_SELF_CHANGE_SIGN,
        UNARY_LOGICAL_NOT,
        UNARY_BITWISE_INVERSE,
        LITERAL_INTEGER,
        LITERAL_FLOAT,
        LITERAL_STRING,
        LITERAL_CHAR,
        FUNCTION_CALL,
        VARIABLE,

        MAX_NUM,
    };

    LUSTFRONTEND_API extern const char* operator_type_to_name(OperatorType type);

    struct ASTNode_Expr : public ASTBaseNode<GrammarRule::EXPRESSION, ASTNode_Statement> {
        vector<const IASTNode*> collect_self_nodes() const override;
    };

    struct ASTNode_Operator : public ASTBaseNode<GrammarRule::OPERATOR, ASTNode_Expr> {
        OperatorType operator_type;
        UniquePtr<ASTNode_Expr> left_oprand;
        UniquePtr<ASTNode_Expr> right_oprand;

        vector<const IASTNode*> collect_self_nodes() const override;
        simple_string get_name() const override;
    };

    struct ASTNode_IntegerExpr : public ASTBaseNode<GrammarRule::INTEGER_LITERAL, ASTNode_Operator> {
        simple_string value;
    };

    struct ASTNode_FloatExpr : public ASTBaseNode<GrammarRule::FLOAT_LITERAL, ASTNode_Operator> {
        simple_string value;
    };

    struct ASTNode_StringExpr : public ASTBaseNode<GrammarRule::STRING_LITERAL, ASTNode_Operator> {
        simple_string value;
    };

    struct ASTNode_QualifiedName : public ASTBaseNode<GrammarRule::QUALIFIED_NAME_USAGE, ASTNode_Operator> {
        QualifiedName qualified_name{};
        UniquePtr<ASTNode_InvokeParameters> passing_parameters;

        vector<const IASTNode*> collect_self_nodes() const override;
        simple_string get_name() const override;
    };

}
}
