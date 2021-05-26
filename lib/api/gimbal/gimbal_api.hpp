#ifndef GIMBAL_API_HPP
#define GIMBAL_API_HPP

#include "gimbal_api.h"

// FIX ME FOR WHEN EXCEPTIONS ARE DISABLED!!!
#define GBL_API_TRY \
    do { \
        gimbal::Result result = GBL_RESULT_SUCCESS; \
        try

#define GBL_API_CATCH() \
        GBL_RESULT_CATCH(result); \
        if(!result.isSuccess()) GBL_API_RECORD_SET(result, Result(result).toString().data()); \
    } while(0)


#if 0
#define GBL_RESULT_CATCH(code) \
catch(const ResultException& resultException) {     \
    code = resultException.getResult();  \
}   \
catch(...) {    \
    code = Result(Result::ErrorUnhandledException);  \
}

#endif

#endif // GIMBAL_API_HPP
