#include "grammar.hpp"
#include "container/unique_ptr.hpp"
#include "container/vector.hpp"
#include "grammar/type_expr.hpp"
#include "grammar/operator_expr.hpp"

namespace lust
{
namespace grammar
{
    const char* grammar_rule_to_name(GrammarRule rule) {
        switch (rule) {
            case GrammarRule::PROGRAM: return "PROGRAM";
            case GrammarRule::STATEMENT: return "STATEMENT";
            case GrammarRule::EXPR_STATEMENT: return "EXPR_STATEMENT";
            case GrammarRule::VAR_DECL: return "VAR_DECL";
            case GrammarRule::FUNCTION_DECL: return "FUNCTION_DECL";
            case GrammarRule::ATTRIBUTE: return "ATTRIBUTE";
            case GrammarRule::GENERIC_PARAM: return "GENERIC_PARAM";
            case GrammarRule::GENERIC: return "GENERIC";
            case GrammarRule::PARAMETER: return "INVOKE_PARAM";
            case GrammarRule::TYPE_EXPR: return "TYPE_EXPR";
            case GrammarRule::PARAMETERS_LIST: return "INVOKE_PARAM_LIST";
            case GrammarRule::BLOCK: return "BLOCK";
            case GrammarRule::OPERATOR: return "OPERATOR";
            case GrammarRule::INTEGER_LITERAL: return "INTEGER_LITERAL";
            case GrammarRule::FLOAT_LITERAL: return "FLOAT_LITERAL";
            case GrammarRule::STRING_LITERAL: return "STRING_LITERAL";
            case GrammarRule::QUALIFIED_NAME_USAGE: return "QUALIFIED_NAME_USAGE";
            case GrammarRule::INVOKE_PARAMETERS: return "INVOKE_PARAMETERS";

            default:
                break;
        }
        return "Unknown";
    }

    IASTNode::~IASTNode() = default;

    vector<const IASTNode*> IASTNode::collect_self_nodes() const {
        return {};
    }

    simple_string IASTNode::get_name() const {
        return grammar_rule_to_name(get_type());
    }

    vector<const IASTNode*> ASTNode_ParamDecl::collect_self_nodes() const {
            vector<const IASTNode*> res = IASTNode::collect_self_nodes();
            res.push_back(type.get());
            return res;
    }

    vector<const IASTNode*> ASTNode_ParamList::collect_self_nodes() const {
            vector<const IASTNode*> res = IASTNode::collect_self_nodes();
        for (const UniquePtr<ASTNode_ParamDecl>& n : params) {
            res.push_back(n.get());
        }
        return res;
    }

    vector<const IASTNode*> ASTNode_InvokeParameters::collect_self_nodes() const {
            vector<const IASTNode*> res = IASTNode::collect_self_nodes();
        for (const auto& n : parameter_expressions) {
            res.push_back(n.get());
        }
        return res;
    }
    vector<const IASTNode*> ASTNode_Attribute::collect_self_nodes() const {
        return IASTNode::collect_self_nodes();
    }
    
    vector<const IASTNode*> ASTNode_GenericParam::collect_self_nodes() const {
        return IASTNode::collect_self_nodes();
    }

    vector<const IASTNode*> ASTNode_Statement::collect_self_nodes() const {
        vector<const IASTNode*> res = IASTNode::collect_self_nodes();
        for (const UniquePtr<ASTNode_Attribute>& attr : attributes) {
            res.push_back(attr.get());
        }
        return res;
    }

    vector<const IASTNode*> ASTNode_ExprStatement::collect_self_nodes() const {
        vector<const IASTNode*> res = ASTNode_Statement::collect_self_nodes();
        res.push_back(expression.get());
        return res;
    }

    vector<const IASTNode*> ASTNode_Program::collect_self_nodes() const {
        vector<const IASTNode*> res = IASTNode::collect_self_nodes();
        for (const UniquePtr<ASTNode_Attribute>& attr : attributes) {
            res.push_back(attr.get());
        }
        for (const UniquePtr<ASTNode_Statement>& statement : statements) {
            res.push_back(statement.get());
        }
        return res;
    }

    vector<const IASTNode*> ASTNode_VarDecl::collect_self_nodes() const {
        vector<const IASTNode*> res = ASTNode_Statement::collect_self_nodes();
        res.push_back(specified_type.get());
        res.push_back(evaluate_expression.get());
        return res;
    }

    vector<const IASTNode*> ASTNode_FunctionDecl::collect_self_nodes() const {
        vector<const IASTNode*> res = ASTNode_Statement::collect_self_nodes();

        for (auto& generic_param : generic_params) {
            res.push_back(generic_param.get());
        }
        res.push_back(params.get());
        res.push_back(ret_type.get());
        res.push_back(body.get());

        return res;
    }

    vector<const IASTNode*> ASTNode_Block::collect_self_nodes() const {
        vector<const IASTNode*> res = ASTNode_Statement::collect_self_nodes();
        for (const UniquePtr<ASTNode_Statement>& statement : statements) {
            res.push_back(statement.get());
        }
        return res;
    }
}
}
