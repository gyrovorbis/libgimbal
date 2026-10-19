#ifndef GIMBAL_MEMORY_RESOURCE_HPP
#define GIMBAL_MEMORY_RESOURCE_HPP

#include "gimbal_platform.h"

#ifdef GBL_CPP_17
#   include GBL_PMR_HEADER_
    namespace gimbal {
        namespace pmr = GBL_PMR_NAMESPACE_;
    }
#endif


#endif // GIMBAL_MEMORY_RESOURCE_HPP
