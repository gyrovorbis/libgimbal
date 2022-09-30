/*! \file
 *  \brief GblEvent base type and basic API
 *  \ingroup metaBuiltinTypes
 */

#ifndef GIMBAL_EVENT_H
#define GIMBAL_EVENT_H

#include "gimbal_box.h"

/// \ingroup metaBuiltinTypes
#define GBL_EVENT_TYPE                  (GBL_TYPEOF(GblEvent))
#define GBL_EVENT(instance)             (GBL_INSTANCE_CAST(instance, GblEvent))
#define GBL_EVENT_GET_CLASS(instance)   (GBL_INSTANCE_GET_CLASS(instance,  GblEvent))

#define GBL_SELF_TYPE GblEvent

GBL_DECLS_BEGIN

GBL_DECLARE_ENUM(GBL_EVENT_STATE) {
    GBL_EVENT_STATE_PENDING,
    GBL_EVENT_STATE_ACCEPTED,
    GBL_EVENT_STATE_IGNORED
};

GBL_CLASS_DERIVE_EMPTY(GblEvent, GblBox)

GBL_INSTANCE_DERIVE(GblEvent, GblBox)
    GBL_PRIVATE()
        GBL_EVENT_STATE state;
    GBL_PRIVATE_END
GBL_INSTANCE_END

GBL_EXPORT GblType         GblEvent_type      (void)                   GBL_NOEXCEPT;
GBL_INLINE GblEvent*       GblEvent_create    (GblType type)           GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT      GblEvent_construct (GBL_SELF, GblType type) GBL_NOEXCEPT;
GBL_INLINE GBL_EVENT_STATE GblEvent_state     (GBL_CSELF)              GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT      GblEvent_accept    (GBL_SELF)               GBL_NOEXCEPT;
GBL_INLINE void            GblEvent_reset     (GBL_SELF)               GBL_NOEXCEPT;

// ========== IMPL ===========

GBL_INLINE GblEvent* GblEvent_create(GblType type) GBL_NOEXCEPT {
    return GblType_check(type, GBL_EVENT_TYPE)?
                GBL_EVENT(GblBox_create(type)) :
                NULL;
}

GBL_INLINE GBL_RESULT GblEvent_construct(GBL_SELF, GblType type) GBL_NOEXCEPT {
    return GblType_check(type, GBL_EVENT_TYPE)?
                GblBox_construct((GblBox*)pSelf, type) :
                GBL_RESULT_ERROR_INVALID_TYPE;
}

GBL_INLINE GBL_EVENT_STATE GblEvent_state(GBL_CSELF) GBL_NOEXCEPT {
    return GBL_PRIV_REF(pSelf).state;
}

GBL_INLINE GBL_RESULT GblEvent_accept(GBL_SELF) GBL_NOEXCEPT {
    if(GBL_PRIV_REF(pSelf).state != GBL_EVENT_STATE_PENDING) {
        return GBL_RESULT_ERROR_INVALID_ARG;
    }
    GBL_PRIV_REF(pSelf).state = GBL_EVENT_STATE_ACCEPTED;
    return GBL_RESULT_SUCCESS;
}

GBL_INLINE void GblEvent_reset(GBL_SELF) GBL_NOEXCEPT {
    GBL_PRIV_REF(pSelf).state = GBL_EVENT_STATE_PENDING;
}


GBL_DECLS_END

#undef GBL_SELF_TYPE


#endif // GIMBAL_EVENT_H
