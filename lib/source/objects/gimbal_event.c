#include <gimbal/objects/gimbal_event.h>
#include <gimbal/core/gimbal_api_frame.h>


extern GBL_RESULT GblEvent_typeRegister_(GblEvent* pCtx) GBL_NOEXCEPT {
    GBL_API_BEGIN(pCtx);
    gblTypeRegisterBuiltin(27,
      GBL_TYPE_INVALID,
      "Event",
      &((const GblTypeInfo) {
          .classSize        = sizeof(GblEventClass),
          .classAlign       = GBL_ALIGNOF(GblEventClass),
          .instanceSize     = sizeof(GblEvent),
          .instanceAlign    = GBL_ALIGNOF(GblEvent),
      }),
      GBL_TYPE_FUNDAMENTAL_FLAG_CLASSED         |
      GBL_TYPE_FUNDAMENTAL_FLAG_INSTANTIABLE    |
      GBL_TYPE_FUNDAMENTAL_FLAG_DEEP_DERIVABLE);
    GBL_API_END();
}
