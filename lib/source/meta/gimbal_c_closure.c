#include <gimbal/meta/gimbal_c_closure.h>
#include <gimbal/strings/gimbal_quark.h>
#include <gimbal/core/gimbal_api_frame.h>

GBL_EXPORT GblType GblCClosure_type(void) {
    static GblType type = GBL_INVALID_TYPE;

    static GblTypeInfo info = {
        .classSize = sizeof(GblCClosureClass),
        .instanceSize = sizeof(GblCClosure)
    };

    if(type == GBL_INVALID_TYPE) {
        GBL_API_BEGIN(NULL);
        type = GblType_registerStatic(GblQuark_internStringStatic("CClosure"),
                                      GBL_CLOSURE_TYPE,
                                      &info,
                                      GBL_TYPE_FLAG_TYPEINFO_STATIC);
        GBL_API_VERIFY_LAST_RECORD();
        GBL_API_END_BLOCK();
    }
    return type;
}
