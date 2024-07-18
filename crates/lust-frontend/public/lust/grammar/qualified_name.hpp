#pragma once

#include "lust/container/vector.hpp"
#include "lust/container/simple_string.hpp"

namespace lust
{
namespace grammar
{
    struct QualifiedName { 
        simple_string name;
        vector<simple_string> name_spaces;
    };
}
}
