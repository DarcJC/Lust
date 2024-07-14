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

}
}

