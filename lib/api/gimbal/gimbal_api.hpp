#ifndef GIMBAL_API_HPP
#define GIMBAL_API_HPP

#include "gimbal_api.h"

#define GBL_API_TRY \
    do { \
        gimbal::Result result = GBL_RESULT_SUCCESS; \
        try

#define GBL_API_CATCH() \
        GBL_RESULT_CATCH(result); \
        GBL_API_RESULT_SET(result, "SHIT"); \
    } while(0)


#endif // GIMBAL_API_HPP
