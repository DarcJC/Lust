#pragma once

#include "grammar.hpp"
#include "container/number.hpp"

namespace lust
{
namespace grammar
{
    struct ASTNode_TypeExpr : public ASTBaseNode<GrammarRule::TYPE_EXPR> {
        /**
         * @brief Type like bool
         */
        virtual bool is_trivial_type();
        /**
         * @brief Type like (i32, bool)
         */
        virtual bool is_tuple_type();
        /**
         * @brief Type like &mut i32 or &i32
         */
        virtual bool is_reference_type();
        /**
         * @brief Generic type which needs to be inference
         */
        virtual bool is_generic_type();
        /**
         * @brief Function type, like fn(i32) -> float
         */
        virtual bool is_function_type();
        /**
         * @brief Array type [i32; n]
         */
        virtual bool is_array_type();
        /**
         * @brief Unit type
         */
        virtual bool is_unit_type();
    };

    struct ASTNode_TypeExpr_Trivial : public ASTNode_TypeExpr {
        bool is_trivial_type() override;

        QualifiedName type_name;
    };

    struct ASTNode_TypeExpr_Tuple : public ASTNode_TypeExpr {
        bool is_tuple_type() override;

        bool is_unit_type() override;

        vector<UniquePtr<ASTNode_TypeExpr>> composite_types;
    };

    struct ASTNode_TypeExpr_Reference : public ASTNode_TypeExpr {
        bool is_reference_type() override;

        UniquePtr<ASTNode_TypeExpr> referenced_type;
    };

    struct ASTNode_TypeExpr_Generic : public ASTNode_TypeExpr {
        bool is_generic_type() override;

        QualifiedName base_type;
        vector<UniquePtr<ASTNode_GenericParam>> params;
    };

    struct ASTNode_TypeExpr_Function : public ASTNode_TypeExpr {
        bool is_function_type() override;

        vector<UniquePtr<ASTNode_TypeExpr>> param_types;
        UniquePtr<ASTNode_TypeExpr> return_type;
    };

    struct ASTNode_TypeExpr_Array : public ASTNode_TypeExpr {
        bool is_array_type() override;

        UniquePtr<ASTNode_TypeExpr> array_type;
        size_t array_size;
    };

}
}
