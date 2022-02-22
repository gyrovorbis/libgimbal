#include <gimbal/core/gimbal_state.h>
#include <gimbal/objects/gimbal_context.h>


typedef struct GblState_ {
    GblContext hCtx;



} GblState_;

static GblState_    state_;
static GblState_*   pState_ = &state_;


GBL_API gblStateContextGet(GblContext* phCtx) {
    *phCtx = pState_->hCtx;
}

GBL_API gblStateContextSet(GblContext hCtx) {
    pState_->hCtx = hCtx;
}
