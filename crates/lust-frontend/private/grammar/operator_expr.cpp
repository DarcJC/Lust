#include "grammar/operator_expr.hpp"
#include "grammar.hpp"

namespace lust
{
namespace grammar
{
    const char* operator_type_to_name(OperatorType type) {
        switch (type) {
            case OperatorType::INVALID: return "INVALID";
            case OperatorType::ASSIGNMENT: return "ASSIGNMENT";
            case OperatorType::LOGICAL_OR: return "LOGICAL_OR";
            case OperatorType::LOGICAL_AND: return "LOGICAL_AND";
            case OperatorType::LOGICAL_EQUALITY: return "LOGICAL_EQUALITY";
            case OperatorType::LOGICAL_NONE_EQUALITY: return "LOGICAL_NONE_EQUALITY";
            case OperatorType::LOGICAL_RELATION_LESS_THAN: return "LOGICAL_RELATION_LESS_THAN";
            case OperatorType::LOGICAL_RELATION_LESS_THAN_EQUALITY: return "LOGICAL_RELATION_LESS_THAN_EQUALITY";
            case OperatorType::LOGICAL_RELATION_GREATER_THAN: return "LOGICAL_RELATION_GREATER_THAN";
            case OperatorType::LOGICAL_RELATION_GREATER_THAN_EQUALITY: return "LOGICAL_RELATION_GREATER_THAN_EQUALITY";
            case OperatorType::ARITHMETIC_ADD: return "ARITHMETIC_ADD";
            case OperatorType::ARITHMETIC_SUBTRACT: return "ARITHMETIC_SUBTRACT";
            case OperatorType::ARITHMETIC_MULTIPLY: return "ARITHMETIC_MULTIPLY";
            case OperatorType::ARITHMETIC_DIVIDE: return "ARITHMETIC_DIVIDE";
            case OperatorType::BITWISE_OR: return "BITWISE_OR";
            case OperatorType::BITWISE_XOR: return "BITWISE_XOR";
            case OperatorType::BITWISE_AND: return "BITWISE_AND";
            case OperatorType::ARITHMETIC_EXPONENT: return "ARITHMETIC_EXPONENT";
            case OperatorType::UNARY_ARITHMETIC_MINUS: return "UNARY_ARITHMETIC_MINUS";
            case OperatorType::UNARY_ARITHMETIC_SELF_INCREASE: return "UNARY_ARITHMETIC_SELF_INCREASE";
            case OperatorType::UNARY_ARITHMETIC_SELF_DECREASE: return "UNARY_ARITHMETIC_SELF_DECREASE";
            case OperatorType::UNARY_LOGICAL_NOT: return "UNARY_LOGICAL_NOT";
            case OperatorType::LITERAL_INTEGER: return "LITERAL_INTEGER";
            case OperatorType::LITERAL_FLOAT: return "LITERAL_FLOAT";
            case OperatorType::LITERAL_STRING: return "LITERAL_STRING";
            case OperatorType::LITERAL_CHAR: return "LITERAL_CHAR";
            case OperatorType::FUNCTION_CALL: return "FUNCTION_CALL";
            case OperatorType::VARIABLE: return "VARIABLE";
            case OperatorType::BLOCK: return "BLOCK";
            case OperatorType::IF: return "IF";
            case OperatorType::MEMBER_VISIT: return "MEMBER_VISIT";

            default:
                break;
        }
        
        return "UNKNOWN_OPERATOR";
    }

    vector<const IASTNode*> ASTNode_Expr::collect_self_nodes() const {
        return ASTNode_Statement::collect_self_nodes();
    }

    vector<const IASTNode*> ASTNode_Operator::collect_self_nodes() const {
        vector<const IASTNode*> res = ASTNode_Expr::collect_self_nodes();
        res.push_back(left_oprand.get());
        res.push_back(right_oprand.get());
        return res;
    }

    simple_string ASTNode_Operator::get_name() const {
        return operator_type_to_name(operator_type);
    }

    vector<const IASTNode*> ASTNode_QualifiedName::collect_self_nodes() const {
        vector<const IASTNode*> res = ASTNode_Expr::collect_self_nodes();
        res.push_back(left_oprand.get());
        res.push_back(right_oprand.get());
        res.push_back(passing_parameters.get());
        return res;
    }

    simple_string ASTNode_QualifiedName::get_name() const {
        return operator_type_to_name(operator_type);
    }

    vector<const IASTNode*> ASTNode_BlockExpr::collect_self_nodes() const {
        auto res = ASTNode_Operator::collect_self_nodes();
        res.push_back(left_code_block.get());
        res.push_back(right_code_block.get());
        return res;
    }

    vector<const IASTNode*> ASTNode_ConditionalBlockExpr::collect_self_nodes() const {
        auto res = ASTNode_BlockExpr::collect_self_nodes();
        res.push_back(condition.get());
        return res;
    }
}
}
