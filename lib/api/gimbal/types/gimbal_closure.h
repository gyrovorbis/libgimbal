#ifndef GIMBAL_CLOSURE_H
#define GIMBAL_CLOSURE_H

#include "../types/gimbal_typedefs.h"
#include "../types/gimbal_ref.h"

#define SELF    GblClosure* pSelf
#define CSELF   const SELF

GBL_DECLS_BEGIN

GBL_FORWARD_DECLARE_STRUCT(GblClosure);


typedef GBL_RESULT (*GblClosureDestructFn)(GblClosure* pClosure);

typedef GBL_RESULT (*GblMarshalFn)  (GblClosure* pClosure,
                                     GblVariant* pRetValue,
                                     GblSize     argCount,
                                     GblVariant* pArgs);

typedef struct GblClosure {
    GblMarshalFn            pFnMarshal;
    void*                   pUserdata;
    GblClosureDestructFn    pFnDestructor;
} GblClosure;

typedef struct GblCClosure {
    GblClosure  base;
    GblFnPtr    pFnCCallback;
} GblCClosure;


GBL_INLINE GblClosure* GblClosure_create            (GblSize                allocSize,
                                                     void*                  pUserdata,
                                                     GblClosureDestructFn   pFnDtor)    GBL_NOEXCEPT;

GBL_EXPORT GblClosure* GblClosure_createWithContext(GblSize                 allocSize,
                                                    void*                   pUserdata,
                                                    GblClosureDestructFn    pFnDtor,
                                                    GblContext*             pCtx)       GBL_NOEXCEPT;

GBL_INLINE GblClosure* GblCClosure_create           (GblFnPtr               pFnCFunc,
                                                     void*                  pUserdata)  GBL_NOEXCEPT;

GBL_INLINE GblClosure* GblCClosure_createWithContext(GblFnPtr               pFnCFunc,
                                                     void*                  pUserdata,
                                                     GblContext*            pCtx)       GBL_NOEXCEPT;

GBL_INLINE GblClosure* GblClosure_ref               (SELF)                              GBL_NOEXCEPT;

GBL_INLINE GblRefCount GblClosure_unref             (SELF)                              GBL_NOEXCEPT;


GBL_INLINE GblRefCount GblClosure_refCount          (CSELF)                             GBL_NOEXCEPT;
GBL_INLINE GblContext* GblClosure_context           (CSELF)                             GBL_NOEXCEPT;

GBL_INLINE void        GblClosure_marshalSet        (SELF,
                                                    GblMarshalFn       pFnMarshall)     GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT  GblClosure_invoke           (SELF,
                                                    GblVariant*        pRetValue,
                                                    GblSize            argCount,
                                                    GblVariant*        pArgValues)     GBL_NOEXCEPT;

// ========== IMPL ==========

GBL_INLINE GblClosure* GblClosure_create(GblSize allocSize, void* pUserdata, GblClosureDestructFn pFnDtor) GBL_NOEXCEPT {
    return GblClosure_createWithContext(allocSize, pUserdata, pFnDtor, NULL);
}

GBL_INLINE GblClosure* GblCClosure_create(GblFnPtr pFnCFunc, void* pUserdata) GBL_NOEXCEPT {
    return GblCClosure_createWithContext(pFnCFunc, pUserdata, NULL);
}

GBL_INLINE GblClosure* GblCClosure_createWithContext(GblFnPtr pFnCFunc, void* pUserdata, GblContext* pCtx) GBL_NOEXCEPT {
    GblCClosure* pCClosure = (GblCClosure*)GblClosure_createWithContext(sizeof(GblCClosure), pUserdata, NULL, pCtx);
    pCClosure->pFnCCallback = pFnCFunc;
    return &pCClosure->base;
}

GBL_INLINE GblClosure* GblClosure_ref(SELF) GBL_NOEXCEPT {
    return GblRef_acquire(pSelf);
}

GBL_INLINE GblRefCount GblClosure_unref(SELF) GBL_NOEXCEPT {
    return GblRef_releaseWithDtor(pSelf, (GblRefDestructFn)pSelf->pFnDestructor);
}

GBL_INLINE GblRefCount GblClosure_refCount(CSELF) GBL_NOEXCEPT {
    return GblRef_refCount(pSelf);
}

GBL_INLINE GblContext* GblClosure_context(CSELF) GBL_NOEXCEPT {
    return GblRef_context(pSelf);
}

GBL_INLINE void GblClosure_marshalSet(SELF, GblMarshalFn pFnMarshal) GBL_NOEXCEPT {
    GBL_ASSERT(pSelf);
    pSelf->pFnMarshal = pFnMarshal;
}


GBL_DECLS_END

#undef CSELF
#undef SELF

#endif // GIMBAL_CLOSURE_H
