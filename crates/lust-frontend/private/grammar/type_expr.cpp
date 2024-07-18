#include "grammar/type_expr.hpp"

namespace lust
{
namespace grammar
{

    bool ASTNode_TypeExpr::is_trivial_type()
    {
        return false;
    }

    bool ASTNode_TypeExpr::is_tuple_type()
    {
        return false;
    }

    bool ASTNode_TypeExpr::is_generic_type()
    {
        return false;
    }

    bool ASTNode_TypeExpr::is_function_type()
    {
        return false;
    }

    bool ASTNode_TypeExpr::is_array_type()
    {
        return false;
    }

    bool ASTNode_TypeExpr::is_unit_type()
    {
        return false;
    }

    bool ASTNode_TypeExpr::is_reference_type()
    {
        return false;
    }

    bool ASTNode_TypeExpr_Trivial::is_trivial_type()
    {
        return true;
    }

    bool ASTNode_TypeExpr_Tuple::is_tuple_type()
    {
        return true;
    }

    bool ASTNode_TypeExpr_Tuple::is_unit_type()
    {
        return composite_types.empty();
    }

    bool ASTNode_TypeExpr_Reference::is_reference_type()
    {
        return true;
    }

    bool ASTNode_TypeExpr_Generic::is_generic_type()
    {
        return true;
    }

    bool ASTNode_TypeExpr_Function::is_function_type()
    {
        return true;
    }

    bool ASTNode_TypeExpr_Array::is_array_type()
    {
        return false;
    }

    vector<const IASTNode*> ASTNode_TypeExpr_Tuple::collect_self_nodes() const {
        vector<const IASTNode*> res = ASTNode_TypeExpr::collect_self_nodes();
        for (auto& t : composite_types) {
            res.push_back(t.get());
        }
        return res;
    }

    vector<const IASTNode*> ASTNode_TypeExpr_Reference::collect_self_nodes() const {
        vector<const IASTNode*> res = ASTNode_TypeExpr::collect_self_nodes();
        res.push_back(referenced_type.get());
        return res;
    }

    vector<const IASTNode*> ASTNode_TypeExpr_Generic::collect_self_nodes() const {
        vector<const IASTNode*> res = ASTNode_TypeExpr::collect_self_nodes();
        for (auto& t : params) {
            res.push_back(t.get());
        }
        return res;
    }

    vector<const IASTNode*> ASTNode_TypeExpr_Function::collect_self_nodes() const {
        vector<const IASTNode*> res = ASTNode_TypeExpr::collect_self_nodes();
        for (auto& t : param_types) {
            res.push_back(t.get());
        }
        res.push_back(return_type.get());
        return res;
    }

    vector<const IASTNode*> ASTNode_TypeExpr_Array::collect_self_nodes() const {
        vector<const IASTNode*> res = ASTNode_TypeExpr::collect_self_nodes();
        res.push_back(array_type.get());
        return res;
    }
}
}

