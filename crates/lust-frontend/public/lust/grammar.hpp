#pragma once

#include "container/simple_string.hpp"
#include "container/unique_ptr.hpp"
#include "container/vector.hpp"
#include "lustfrontend_export.h"
#include "grammar/qualified_name.hpp"

namespace lust
{
namespace grammar
{
    struct ASTNode_TypeExpr;
    struct ASTNode_TypeExpr_Trival;
    struct ASTNode_TypeExpr_Reference;
    struct ASTNode_TypeExpr_Tuple;
    struct ASTNode_TypeExpr_Generic;
    struct ASTNode_TypeExpr_Function;
    struct ASTNode_Block;
    struct ASTNode_Expr;
    struct ASTNode_Operator;

    enum class GrammarRule : uint32_t {
        NONE,

        PROGRAM,
        STATEMENT,
        NAMED_STATEMENT,
        EXPR_STATEMENT,
        VAR_DECL,
        FUNCTION_DECL,
        ATTRIBUTE,
        GENERIC_PARAM,
        GENERIC,
        PARAMETER,
        TYPE_EXPR,
        PARAMETERS_LIST,
        BLOCK,
        OPERATOR,
        EXPRESSION,
        INTEGER_LITERAL,
        FLOAT_LITERAL,
        STRING_LITERAL,
        QUALIFIED_NAME_USAGE,
        INVOKE_PARAMETERS,
        BLOCK_EXPR,
        IF_BLOCK_EXPR,
        STRUCT,
        STRUCT_FIELD,
        TRAIT,
        MORPHISMS_TYPE,
        MORPHISMS_CONSTANT,

        MAX_NUM,
    };

    LUSTFRONTEND_API extern const char* grammar_rule_to_name(GrammarRule rule);

    enum class Visibility : uint8_t {
        DEFAULT = 0,
        SELF = 0,
        SUPER = 1,
        CRATE = 2,
        PUBLIC = 3,
    };

    struct LUSTFRONTEND_API IASTNode {
    public:
        virtual ~IASTNode();

        virtual GrammarRule get_type() const = 0;

        virtual vector<const IASTNode*> collect_self_nodes() const;
        virtual simple_string get_name() const;
    };

    template <GrammarRule Rule, class Parent = IASTNode>
    struct ASTBaseNode : public Parent { 
        constexpr GrammarRule get_type() const override {
            return Rule;
        }

        constexpr static GrammarRule static_type() {
            return Rule;
        }

        using Super = Parent;
    };

    struct ASTNode_ParamDecl : public ASTBaseNode<GrammarRule::PARAMETER> {
        UniquePtr<ASTNode_TypeExpr> type;
        simple_string identifier;

        bool is_instance_function = false;

        vector<const IASTNode*> collect_self_nodes() const override;
    };

    struct ASTNode_ParamList : public ASTBaseNode<GrammarRule::PARAMETERS_LIST> {
        vector<UniquePtr<ASTNode_ParamDecl>> params;

        vector<const IASTNode*> collect_self_nodes() const override;
    };

    struct ASTNode_InvokeParameters : public ASTBaseNode<GrammarRule::INVOKE_PARAMETERS> {
        vector<UniquePtr<ASTNode_Expr>> parameter_expressions;

        vector<const IASTNode*> collect_self_nodes() const override;
    };

    struct ASTNode_Attribute : public ASTBaseNode<GrammarRule::ATTRIBUTE> {
        QualifiedName name;
        vector<simple_string> args;

        vector<const IASTNode*> collect_self_nodes() const override;
    };

    struct ASTNode_GenericParam : public ASTBaseNode<GrammarRule::GENERIC_PARAM> {
        vector<UniquePtr<ASTNode_TypeExpr>> types;
        vector<QualifiedName> constraints;

        vector<const IASTNode*> collect_self_nodes() const override;
    };

    struct ASTNode_Statement : public ASTBaseNode<GrammarRule::STATEMENT> {
        vector<UniquePtr<ASTNode_Attribute>> attributes{};
        Visibility visibility;
        bool is_end_with_semicolon = true;

        vector<const IASTNode*> collect_self_nodes() const override;
        simple_string get_name() const override;
    };

    struct ASTNode_NamedStatement : public ASTBaseNode<GrammarRule::NAMED_STATEMENT, ASTNode_Statement> {
        simple_string identifier;
    };

    struct ASTNode_ExprStatement : public ASTBaseNode<GrammarRule::EXPR_STATEMENT, ASTNode_Statement> {
        UniquePtr<ASTNode_Expr> expression;

        vector<const IASTNode*> collect_self_nodes() const override;
    };

    struct ASTNode_Program : public ASTBaseNode<GrammarRule::PROGRAM> {
        vector<UniquePtr<ASTNode_Attribute>> attributes{};
        vector<UniquePtr<ASTNode_Statement>> statements{};

        vector<const IASTNode*> collect_self_nodes() const override;
    };

    struct ASTNode_VarDecl : public ASTBaseNode<GrammarRule::VAR_DECL, ASTNode_Statement> {
        UniquePtr<ASTNode_TypeExpr> specified_type;
        UniquePtr<ASTNode_Operator> evaluate_expression;
        bool is_forward_decl_only = false;
        bool is_mutable = false;
        bool is_const = false;
        simple_string identifier;

        vector<const IASTNode*> collect_self_nodes() const override;
    };

    struct ASTNode_FunctionDecl : public ASTBaseNode<GrammarRule::FUNCTION_DECL, ASTNode_NamedStatement> {
        bool is_async = false;
        vector<UniquePtr<ASTNode_GenericParam>> generic_params;
        UniquePtr<ASTNode_ParamList> params;
        UniquePtr<ASTNode_TypeExpr> ret_type;
        UniquePtr<ASTNode_Block> body;

        vector<const IASTNode*> collect_self_nodes() const override;
    };

    struct ASTNode_Block : public ASTBaseNode<GrammarRule::BLOCK, ASTNode_Statement> {
        vector<UniquePtr<ASTNode_Statement>> statements{};

        vector<const IASTNode*> collect_self_nodes() const override;
    };

    struct ASTNode_StructField : public ASTBaseNode<GrammarRule::STRUCT_FIELD, ASTNode_NamedStatement> {
        bool is_field_mutable = false;
        UniquePtr<ASTNode_TypeExpr> field_type;

        vector<const IASTNode*> collect_self_nodes() const override;
    };

    struct ASTNode_StructDecl : public ASTBaseNode<GrammarRule::STRUCT, ASTNode_NamedStatement> {
        vector<UniquePtr<ASTNode_StructField>> fields{};
        vector<UniquePtr<ASTNode_GenericParam>> generic_params;

        vector<const IASTNode*> collect_self_nodes() const override;
    };

    struct ASTNode_MorphismsType : public ASTBaseNode<GrammarRule::MORPHISMS_TYPE, ASTNode_NamedStatement> {
        UniquePtr<ASTNode_TypeExpr> value;

        vector<const IASTNode*> collect_self_nodes() const override;
    };

    struct ASTNode_MorphismsConstant : public ASTBaseNode<GrammarRule::MORPHISMS_CONSTANT, ASTNode_NamedStatement> {
        UniquePtr<ASTNode_TypeExpr> type;
        UniquePtr<ASTNode_Expr> value;

        vector<const IASTNode*> collect_self_nodes() const override;
    };

    struct ASTNode_TraitDecl : public ASTBaseNode<GrammarRule::TRAIT, ASTNode_NamedStatement> {
        vector<UniquePtr<ASTNode_GenericParam>> generic_params;
        vector<UniquePtr<ASTNode_MorphismsType>> morphisms_types;
        vector<UniquePtr<ASTNode_MorphismsConstant>> morphisms_constants;
        vector<UniquePtr<ASTNode_FunctionDecl>> functions;

        vector<const IASTNode*> collect_self_nodes() const override;
    };

    template <typename T>
    T* dyn_cast(IASTNode* node) {
        if (node) {
            if (node->get_type() == T::static_type()) {
                return static_cast<T*>(node);
            }
        }
        return nullptr;
    }
}
}
