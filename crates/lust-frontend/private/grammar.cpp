#include "grammar.hpp"
#include "container/simple_string.hpp"
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
            case GrammarRule::BLOCK_EXPR: return "BLOCK_EXPR";
            case GrammarRule::IF_BLOCK_EXPR: return "IF_BLOCK_EXPR";
            case GrammarRule::STRUCT: return "STRUCT";
            case GrammarRule::STRUCT_FIELD: return "STRUCT_FIELD";
            case GrammarRule::TRAIT: return "TRAIT";
            case GrammarRule::MORPHISMS_TYPE: return "MORPHISMS_TYPE";
            case GrammarRule::MORPHISMS_CONSTANT: return "MORPHISMS_CONSTANT";

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
            vector<const IASTNode*> res = Super::collect_self_nodes();
            res.push_back(type.get());
            return res;
    }

    vector<const IASTNode*> ASTNode_ParamList::collect_self_nodes() const {
            vector<const IASTNode*> res = Super::collect_self_nodes();
        for (const UniquePtr<ASTNode_ParamDecl>& n : params) {
            res.push_back(n.get());
        }
        return res;
    }

    vector<const IASTNode*> ASTNode_InvokeParameters::collect_self_nodes() const {
            vector<const IASTNode*> res = Super::collect_self_nodes();
        for (const auto& n : parameter_expressions) {
            res.push_back(n.get());
        }
        return res;
    }
    vector<const IASTNode*> ASTNode_Attribute::collect_self_nodes() const {
        return Super::collect_self_nodes();
    }
    
    vector<const IASTNode*> ASTNode_GenericParam::collect_self_nodes() const {
        auto res = Super::collect_self_nodes();

        for (auto& n : types) {
            res.push_back(n.get());
        }

        return res;
    }

    vector<const IASTNode*> ASTNode_Statement::collect_self_nodes() const {
        vector<const IASTNode*> res = Super::collect_self_nodes();
        for (const UniquePtr<ASTNode_Attribute>& attr : attributes) {
            res.push_back(attr.get());
        }
        return res;
    }

    simple_string ASTNode_Statement::get_name() const {
        simple_string name = Super::get_name();
        if (!is_end_with_semicolon) {
            name += "(RET)";
        }
        return name;
    }

    vector<const IASTNode*> ASTNode_ExprStatement::collect_self_nodes() const {
        vector<const IASTNode*> res = Super::collect_self_nodes();
        res.push_back(expression.get());
        return res;
    }

    vector<const IASTNode*> ASTNode_Program::collect_self_nodes() const {
        vector<const IASTNode*> res = Super::collect_self_nodes();
        for (const UniquePtr<ASTNode_Attribute>& attr : attributes) {
            res.push_back(attr.get());
        }
        for (const UniquePtr<ASTNode_Statement>& statement : statements) {
            res.push_back(statement.get());
        }
        return res;
    }

    vector<const IASTNode*> ASTNode_VarDecl::collect_self_nodes() const {
        vector<const IASTNode*> res = Super::collect_self_nodes();
        res.push_back(specified_type.get());
        res.push_back(evaluate_expression.get());
        return res;
    }

    vector<const IASTNode*> ASTNode_FunctionDecl::collect_self_nodes() const {
        vector<const IASTNode*> res = Super::collect_self_nodes();

        for (auto& generic_param : generic_params) {
            res.push_back(generic_param.get());
        }
        res.push_back(params.get());
        res.push_back(ret_type.get());
        res.push_back(body.get());

        return res;
    }

    vector<const IASTNode*> ASTNode_Block::collect_self_nodes() const {
        vector<const IASTNode*> res = Super::collect_self_nodes();
        for (const UniquePtr<ASTNode_Statement>& statement : statements) {
            res.push_back(statement.get());
        }
        return res;
    }

    vector<const IASTNode*> ASTNode_StructField::collect_self_nodes() const {
        vector<const IASTNode*> res = Super::collect_self_nodes();
        res.push_back(field_type.get());
        return res;
    }

    vector<const IASTNode*> ASTNode_StructDecl::collect_self_nodes() const {
        vector<const IASTNode*> res = Super::collect_self_nodes();
        for (const UniquePtr<ASTNode_StructField>& field : fields) {
            res.push_back(field.get());
        }
        return res;
    }

    vector<const IASTNode*> ASTNode_MorphismsType::collect_self_nodes() const {
        vector<const IASTNode*> res = Super::collect_self_nodes();
        res.push_back(value.get());
        return res;
    }

    vector<const IASTNode*> ASTNode_MorphismsConstant::collect_self_nodes() const {
        vector<const IASTNode*> res = Super::collect_self_nodes();
        res.push_back(value.get());
        return res;
    }

    vector<const IASTNode*> ASTNode_TraitDecl::collect_self_nodes() const {
        vector<const IASTNode*> res = Super::collect_self_nodes();

        for (const auto& n : generic_params) {
            res.push_back(n.get());
        }

        for (const auto& n : morphisms_types) {
            res.push_back(n.get());
        }

        for (const auto& n : morphisms_constants) {
            res.push_back(n.get());
        }

        for (const auto& n : functions) {
            res.push_back(n.get());
        }

        return res;
    }
}
}
