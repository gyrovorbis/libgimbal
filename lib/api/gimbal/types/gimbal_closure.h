/*! \file
 *  \brief Generic and builtin closure types and API.
 *  \ingroup signals
 */

#ifndef GIMBAL_CLOSURE_H
#define GIMBAL_CLOSURE_H

#include "../types/gimbal_typedefs.h"
#include "../types/gimbal_ref.h"
#include "gimbal_marshal.h"

#define GBL_CALLBACK(fnPtr)   ((GblFnPtr)fnPtr)

#define SELF    GblClosure* pSelf
#define CSELF   const SELF

GBL_DECLS_BEGIN

GBL_FORWARD_DECLARE_STRUCT(GblClosure);

typedef GBL_RESULT (*GblClosureDestructFn)(GblClosure* pClosure);

typedef enum GBL_CLOSURE_FLAGS {
    GBL_CLOSURE_FLAGS_NONE
} GBL_CLOSURE_FLAGS;

typedef struct GblClosure {
    GblMarshalFn            pFnMetaMarshal;
    void*                   pMetaMarshalData;
    GblMarshalFn            pFnMarshal;
    void*                   pUserdata;
    GblFlags                flags;
    GblClosureDestructFn    pFnDestructor;
} GblClosure;

typedef struct GblCClosure {
    GblClosure  base;
    GblFnPtr    pFnCCallback;
} GblCClosure;

typedef struct GblClassClosure {
    GblClosure  base;
    GblType     classType;
    GblSize     offset;
} GblClassClosure;

typedef struct GblSignalClosure {
    GblClosure  base;
    const char* pSignalName;
} GblSignalClosure;

GBL_EXPORT GblClosure* GblClosure_current           (void)                              GBL_NOEXCEPT;

GBL_INLINE GblClosure* GblClosure_create            (GblSize                allocSize,
                                                     void*                  pUserdata,
                                                     GblClosureDestructFn   pFnDtor)    GBL_NOEXCEPT;

GBL_EXPORT GblClosure* GblClosure_createWithContext (GblSize                allocSize,
                                                     void*                  pUserdata,
                                                     GblClosureDestructFn   pFnDtor,
                                                     GblContext*            pCtx)       GBL_NOEXCEPT;

GBL_INLINE GblClosure* GblCClosure_create           (GblFnPtr               pFnCFunc,
                                                     void*                  pUserdata)  GBL_NOEXCEPT;

GBL_INLINE GblClosure* GblCClosure_createWithContext(GblFnPtr               pFnCFunc,
                                                     void*                  pUserdata,
                                                     GblContext*            pCtx)       GBL_NOEXCEPT;

GBL_INLINE GblClosure* GblClassClosure_create       (GblType                classType,
                                                     GblSize                offset,
                                                     void*                  pUserdata)  GBL_NOEXCEPT;


GBL_INLINE GblClosure* GblClassClosure_createWithContext
                                                    (GblType                classType,
                                                     GblSize                offset,
                                                     void*                  pUserdata,
                                                     GblContext*            pCtx)       GBL_NOEXCEPT;

GBL_INLINE GblClosure* GblSignalClosure_create      (const char*            pSignalName,
                                                     void*                  pUserdata)  GBL_NOEXCEPT;

GBL_INLINE GblClosure* GblSignalClosure_createWithContext
                                                    (const char*            pSignalName,
                                                     void*                  pUserdata,
                                                     GblContext*            pCtx)       GBL_NOEXCEPT;


GBL_INLINE GblClosure* GblClosure_ref               (SELF)                              GBL_NOEXCEPT;

GBL_INLINE GblRefCount GblClosure_unref             (SELF)                              GBL_NOEXCEPT;


GBL_INLINE GblRefCount GblClosure_refCount          (CSELF)                             GBL_NOEXCEPT;
GBL_INLINE GblContext* GblClosure_context           (CSELF)                             GBL_NOEXCEPT;

GBL_INLINE void        GblClosure_setUserdata       (SELF,
                                                     void* pUserdata)                   GBL_NOEXCEPT;

GBL_INLINE void        GblClosure_setMarshal        (SELF,
                                                     GblMarshalFn    pFnMarshal)        GBL_NOEXCEPT;

GBL_INLINE void        GblClosure_setMetaMarshal    (SELF,
                                                     GblMarshalFn    pFnMetaMarshal,
                                                     void*           pMetaMarshalData)  GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT  GblClosure_invoke            (SELF,
                                                     GblVariant*     pRetValue,
                                                     GblSize         argCount,
                                                     GblVariant*     pArgValues)        GBL_NOEXCEPT;

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

GBL_INLINE void GblClosure_setUserdata(SELF, void* pUserdata) GBL_NOEXCEPT {
    GBL_ASSERT(pSelf);
    pSelf->pUserdata = pUserdata;
}

GBL_INLINE void GblClosure_setMarshal(SELF, GblMarshalFn pFnMarshal) GBL_NOEXCEPT {
    GBL_ASSERT(pSelf);
    pSelf->pFnMarshal = pFnMarshal;
}

GBL_INLINE void GblClosure_setMetaMarshal(SELF, GblMarshalFn pFnMetaMarshal, void* pMetaMarshalData) GBL_NOEXCEPT {
    GBL_ASSERT(pSelf);
    pSelf->pFnMetaMarshal = pFnMetaMarshal;
    pSelf->pMetaMarshalData = pMetaMarshalData;
}

GBL_INLINE GblClosure* GblClassClosure_createWithContext(GblType                classType,
                                                         GblSize                offset,
                                                         void*                  pUserdata,
                                                         GblContext*            pCtx)       GBL_NOEXCEPT
{
    GblClassClosure* pClassClosure =
            (GblClassClosure*)GblClosure_createWithContext(sizeof(GblClassClosure),
                                                           pUserdata,
                                                           NULL,
                                                           pCtx);
    pClassClosure->classType = classType;
    pClassClosure->offset = offset;
    GblClosure_setMetaMarshal(&pClassClosure->base, GblMarshal_ClassClosureMeta, NULL);
    return &pClassClosure->base;
}



GBL_INLINE GblClosure* GblClassClosure_create(GblType                classType,
                                              GblSize                offset,
                                              void*                  pUserdata)  GBL_NOEXCEPT
{
    return GblClassClosure_createWithContext(classType, offset, pUserdata, NULL);
}

GBL_INLINE GblClosure* GblSignalClosure_create(const char* pSignalName,
                                               void*       pUserdata)  GBL_NOEXCEPT
{
    return GblSignalClosure_createWithContext(pSignalName, pUserdata, NULL);
}

GBL_INLINE GblClosure* GblSignalClosure_createWithContext(const char* pSignalName,
                                                          void*       pUserdata,
                                                          GblContext* pCtx)       GBL_NOEXCEPT
{
    GblSignalClosure* pSignalClosure =
            (GblSignalClosure*)GblClosure_createWithContext(sizeof(GblSignalClosure),
                                                            pUserdata,
                                                            NULL,
                                                            pCtx);
    pSignalClosure->pSignalName = pSignalName;
    GblClosure_setMarshal(&pSignalClosure->base,
                          GblMarshal_SignalForwarder);

    return &pSignalClosure->base;
}


GBL_DECLS_END

#undef CSELF
#undef SELF

#endif // GIMBAL_CLOSURE_H
