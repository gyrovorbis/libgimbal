/*! \file
 *  \brief GblEvent base type and basic API
 *  \ingroup metaBuiltinTypes
 */

#ifndef GIMBAL_EVENT_H
#define GIMBAL_EVENT_H

#include "../meta/gimbal_instance.h"

/// \ingroup metaBuiltinTypes
#define GBL_EVENT_TYPE                  (GblEvent_type())
#define GBL_EVENT_STRUCT                GblEvent
#define GBL_EVENT_CLASS_STRUCT          GblEventClass
#define GBL_EVENT(instance)             (GBL_INSTANCE_CAST_PREFIX  (instance,  GBL_EVENT))
#define GBL_EVENT_CHECK(instance)       (GBL_INSTANCE_CHECK_PREFIX (instance,  GBL_EVENT))
#define GBL_EVENT_CLASS(klass)          (GBL_CLASS_CAST_PREFIX     (klass,     GBL_EVENT))
#define GBL_EVENT_CLASS_CHECK(klass)    (GBL_CLASS_CHECK_PREFIX    (klass,     GBL_EVENT))
#define GBL_EVENT_GET_CLASS(instance)   (GBL_INSTANCE_GET_CLASS_PREFIX (instance,  GBL_EVENT))

#define SELF    GblEvent* pSelf
#define CSELF   const SELF

GBL_DECLS_BEGIN

GBL_DECLARE_ENUM(GBL_EVENT_STATE) {
    GBL_EVENT_STATE_PENDING,
    GBL_EVENT_STATE_ACCEPTED,
    GBL_EVENT_STATE_IGNORED
};

typedef struct GblEventClass {
    GblClass    base;
} GblEventClass;

typedef struct GblEvent {
    union {
        GblEventClass*  pClass;
        GblInstance     base;
    };
    GBL_EVENT_STATE     state;
} GblEvent;

/* PROPERTIES:
 * 1) state
 * 2) type
 *
 */

GBL_EXPORT GblType          GblEvent_type       (void)                  GBL_NOEXCEPT;
GBL_INLINE GblEvent*        GblEvent_create     (GblType type)          GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT       GblEvent_construct  (SELF, GblType type)    GBL_NOEXCEPT;
GBL_INLINE GblRefCount      GblEvent_destruct   (SELF)                  GBL_NOEXCEPT;
GBL_INLINE GblRefCount      GblEvent_destroy    (SELF)                  GBL_NOEXCEPT;
GBL_INLINE GblType          GblEvent_typeOf     (CSELF)                 GBL_NOEXCEPT;
GBL_INLINE GBL_EVENT_STATE  GblEvent_state      (CSELF)                 GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT       GblEvent_accept     (SELF)                  GBL_NOEXCEPT;


// ========== IMPL ===========

GBL_INLINE GblEvent* GblEvent_create(GblType type) GBL_NOEXCEPT {
    return GblType_check(type, GBL_EVENT_TYPE)?
                GBL_EVENT(GblInstance_create(type)) :
                NULL;
}

GBL_INLINE GBL_RESULT GblEvent_construct(SELF, GblType type) GBL_NOEXCEPT {
    return GblType_check(type, GBL_EVENT_TYPE)?
                GblInstance_construct(GBL_INSTANCE(pSelf), type) :
                GBL_RESULT_ERROR_INVALID_TYPE;
}

GBL_INLINE GblRefCount GblEvent_destruct(SELF) GBL_NOEXCEPT {
    return GblInstance_destruct(GBL_INSTANCE(pSelf));
}

GBL_INLINE GblRefCount GblEvent_destroy(SELF) GBL_NOEXCEPT {
    return GblInstance_destroy(GBL_INSTANCE(pSelf));
}

GBL_INLINE GblType GblEvent_typeOf(CSELF) GBL_NOEXCEPT {
    return GBL_INSTANCE_TYPEOF(pSelf);
}

GBL_INLINE GBL_EVENT_STATE GblEvent_state(CSELF) GBL_NOEXCEPT {
    return pSelf->state;
}

GBL_INLINE GBL_RESULT GblEvent_accept(SELF) GBL_NOEXCEPT {
    if(pSelf->state != GBL_EVENT_STATE_PENDING) {
        return GBL_RESULT_ERROR_INVALID_ARG;
    }
    pSelf->state = GBL_EVENT_STATE_ACCEPTED;
    return GBL_RESULT_SUCCESS;
}

GBL_DECLS_END

#undef CSELF
#undef SELF


#endif // GIMBAL_EVENT_H
