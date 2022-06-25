#ifndef GIMBAL_MEMORY_RESOURCE_HPP
#define GIMBAL_MEMORY_RESOURCE_HPP

#include "gimbal_compiler.h"

#ifdef GBL_CPP_17
#   if defined(__APPLE__) || defined(__ANDROID__) || defined(__wasm__)
#       include <experimental/memory_resource>
        namespace gimbal {
            namespace pmr = std::experimental::pmr;
        }
#   else
#       include <memory_resource>
        namespace gimbal {
            namespace pmr = std::pmr;
        }
#       define GBL_PMR_VECTOR
#       define GBL_PMR_STRING
#       define GBL_LEX_CMP_3WAY
#   endif
#endif


#endif // GIMBAL_MEMORY_RESOURCE_HPP
