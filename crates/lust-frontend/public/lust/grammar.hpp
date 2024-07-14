#pragma once

#include "lexer.hpp"
#include "container/unique_ptr.hpp"
#include "container/vector.hpp"

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

    enum class GrammarRule : uint32_t {
        NONE,

        PROGRAM,
        STATEMENT,
        EXPR_STATEMENT,
        VAR_DECL,
        FUNCTION_DECL,
        ATTRIBUTE,
        GENERIC_PARAM,
        GENERIC,
        INVOKE_PARAM,
        TYPE_EXPR,
        INVOKE_PARAM_LIST,

        MAX_NUM,
    };

    enum class Visibility : uint8_t {
        DEFAULT = 0,
        SELF = 0,
        SUPER = 1,
        CRATE = 2,
        PUBLIC = 3,
    };

    struct IASTNode {
    public:
        virtual ~IASTNode();

        virtual GrammarRule get_type() const = 0;
    };

    template <GrammarRule Rule, class Parent = IASTNode>
    struct ASTBaseNode : public Parent { 
        constexpr GrammarRule get_type() const override {
            return Rule;
        }
    };

    struct QualifiedName { 
        simple_string name;
        vector<simple_string> name_spaces;
    };

    struct ASTNode_InvokeParam : public ASTBaseNode<GrammarRule::INVOKE_PARAM> {
        UniquePtr<ASTNode_TypeExpr> type;
        simple_string identifier;
    };

    struct ASTNode_ParamList : public ASTBaseNode<GrammarRule::INVOKE_PARAM_LIST> {
        vector<UniquePtr<ASTNode_InvokeParam>> params;
    };

    struct ASTNode_Attribute : public ASTBaseNode<GrammarRule::ATTRIBUTE> {
        QualifiedName name;
        vector<simple_string> args;
    };

    struct ASTNode_GenericParam : public ASTBaseNode<GrammarRule::GENERIC_PARAM> {
        simple_string identifier;
        vector<QualifiedName> constraints;
    };

    struct ASTNode_Statement : public ASTBaseNode<GrammarRule::STATEMENT> {
        vector<UniquePtr<ASTNode_Attribute>> attributes{};
        Visibility visibility;
    };

    struct ASTNode_ExprStatement : public ASTBaseNode<GrammarRule::EXPR_STATEMENT, ASTNode_Statement> {
    };

    struct ASTNode_Program : public ASTBaseNode<GrammarRule::PROGRAM> {
        vector<UniquePtr<ASTNode_Attribute>> attributes{};
        vector<UniquePtr<ASTNode_Statement>> statements{};
    };

    struct ASTNode_VarDecl : public ASTBaseNode<GrammarRule::VAR_DECL, ASTNode_Statement> {
    };

    struct ASTNode_FunctionDecl : public ASTBaseNode<GrammarRule::FUNCTION_DECL, ASTNode_Statement> {
        bool is_async = false;
        simple_string identifier;
        vector<UniquePtr<ASTNode_GenericParam>> generic_params;
        UniquePtr<ASTNode_ParamList> params;
        UniquePtr<ASTNode_TypeExpr> ret_type;
    };
}
}
