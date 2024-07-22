#pragma once

// Introduce feature testing header in C++20 and above
#if __cplusplus >= 202002L
#   include <version>
#endif

#if defined (__cpp_lib_execution)
#   if __cpp_lib_execution >= 201902L
#       define LUST_EXECUTION_CAN_VECTORIZATION std::execution::unseq
#   else
#       define LUST_EXECUTION_CAN_VECTORIZATION std::execution::seq
#   endif // __cpp_lib_execution >= 201902L
#else // !defined (__cpp_lib_execution)
#   define LUST_EXECUTION_CAN_VECTORIZATION 
#endif // defined (__cpp_lib_execution)

