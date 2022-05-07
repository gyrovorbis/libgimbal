#include <gimbal/objects/gimbal_event.h>
#include <gimbal/core/gimbal_api_frame.h>


extern GBL_RESULT GblEvent_typeRegister_(GblEvent* pCtx) GBL_NOEXCEPT {
    GBL_API_BEGIN(pCtx);
    GblType_registerBuiltin(GBL_TYPE_BUILTIN_INDEX_EVENT,
      GBL_INVALID_TYPE,
      "Event",
      &((const GblTypeInfo) {
          .classSize        = sizeof(GblEventClass),
          .instanceSize     = sizeof(GblEvent),
      }),
      GBL_TYPE_FUNDAMENTAL_FLAG_CLASSED         |
      GBL_TYPE_FUNDAMENTAL_FLAG_INSTANTIABLE    |
      GBL_TYPE_FUNDAMENTAL_FLAG_DEEP_DERIVABLE);
    GBL_API_END();
}
