#ifndef GIMBAL_CONTROL_FLOW_H
#define GIMBAL_CONTROL_FLOW_H

#include "gimbal_decls.h"

GBL_DECLS_BEGIN

typedef struct GblFrame {
    GblBool raised;
} GblFrame;

#define GBL_TRY

#define GBL_VERIFY(expr, domain, code, ...)


void test1(int arg) {
    GBL_TRY {
        GBL_VERIFY(arg != 0,
                   GBL_ERROR_DOMAIN,
                   GBL_RESULT_ERROR_INVALID_ARG,
                   "Arg unacceptable: %d", arg);
    }
}

bool test2(int arg) {
    GBL_TRY {
        GBL_VERIFY(arg != 0,
                   GBL_ERROR_DOMAIN,
                   GBL_RESULT_ERROR_INVALID_ARG,
                   "Arg reesty: %d", arg);
    }

    return (pFrame->raised)?  true : false;
}

GBL_RESULT test3(int arg) {
    GBL_TRY {
        GBL_VERIFY(arg != 0,
                   GBL_ERROR_DOMAIN,
                   GBL_RESULT_ERROR_INVALID_ARG,
                   "Arg reesty: %d", arg);
    }

    return 0;
}

GBL_RESULT test4(int arg) {
    GblError_clear();

    if(arg != 0)
        GblError_raise(GBL_ERROR_DOMAIN,
                       GBL_RESULT_ERROR_INVALID_ARG);

    return 0;
}

GBL_DECLS_END

#endif // GIMBAL_CONTROL_FLOW_H
