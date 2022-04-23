#ifndef GIMBAL_EVENT_H
#define GIMBAL_EVENT_H

#include "../meta/gimbal_instance.h"

#define GBL_EVENT_TYPE                  GBL_TYPE_EVENT
#define GBL_EVENT_STRUCT                GblEvent
#define GBL_EVENT_CLASS_STRUCT          GblEventClass
#define GBL_EVENT(instance)             (GBL_TYPE_CAST_INSTANCE_PREFIX  (instance,  GBL_EVENT))
#define GBL_EVENT_CHECK(instance)       (GBL_TYPE_CHECK_INSTANCE_PREFIX (instance,  GBL_EVENT))
#define GBL_EVENT_CLASS(klass)          (GBL_TYPE_CAST_CLASS_PREFIX     (klass,     GBL_EVENT))
#define GBL_EVENT_CLASS_CHECK(klass)    (GBL_TYPE_CHECK_CLASS_PREFIX    (klass,     GBL_EVENT))
#define GBL_EVENT_GET_CLASS(instance)   (GBL_TYPE_CAST_GET_CLASS_PREFIX (instance,  GBL_EVENT))

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


GBL_INLINE GblEvent*        GblEvent_create     (GblType type)          GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT       GblEvent_construct  (SELF, GblType type)    GBL_NOEXCEPT;
GBL_INLINE GblRefCount      GblEvent_destruct   (SELF)                  GBL_NOEXCEPT;
GBL_INLINE GblRefCount      GblEvent_destroy    (SELF)                  GBL_NOEXCEPT;
GBL_INLINE GblType          GblEvent_type       (CSELF)                 GBL_NOEXCEPT;
GBL_INLINE GBL_EVENT_STATE  GblEvent_state      (CSELF)                 GBL_NOEXCEPT;
GBL_INLINE GBL_RESULT       GblEvent_accept     (SELF)                  GBL_NOEXCEPT;






// ========== INLINE IMPLEMENTATIONS ===========

GBL_INLINE GblEvent* GblEvent_create(GblType type) GBL_NOEXCEPT {
    return gblTypeIsA(type, GBL_TYPE_EVENT)?
                GBL_EVENT(gblTypeInstanceCreate(type)) :
                NULL;
}

GBL_INLINE GBL_RESULT GblEvent_construct(SELF, GblType type) GBL_NOEXCEPT {
    return gblTypeIsA(type, GBL_TYPE_EVENT)?
                gblTypeInstanceConstruct(type, GBL_INSTANCE(pSelf)) :
                GBL_RESULT_ERROR_INVALID_TYPE;
}

GBL_INLINE GblRefCount GblEvent_destruct(SELF) GBL_NOEXCEPT {
    return gblTypeInstanceDestruct(GBL_INSTANCE(pSelf));
}

GBL_INLINE GblRefCount GblEvent_destroy(SELF) GBL_NOEXCEPT {
    return gblTypeInstanceDestroy(GBL_INSTANCE(pSelf));
}

GBL_INLINE GblType GblEvent_type(CSELF) GBL_NOEXCEPT {
    return GBL_INSTANCE_TYPE(pSelf);
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
