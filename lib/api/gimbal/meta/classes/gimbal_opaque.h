/*! \file
 *  \brief GblOpaqueClass and API for managing Opaque types
 *  \ingroup metaBuiltinTypes
 */

#ifndef GIMBAL_OPAQUE_H
#define GIMBAL_OPAQUE_H

#include "gimbal_primitives.h"

/// \ingroup metaBuiltinTypes
#define GBL_OPAQUE_TYPE                  (GblOpaque_type())

#define GBL_OPAQUE_CLASS(klass)          (GBL_CLASS_CAST(klass, GBL_OPAQUE_TYPE, GblOpaqueClass))
#define GBL_OPAQUE_GET_CLASS(variant)    (GBL_OPAQUE_CLASS(GblClass_weakRefDefault(GblVariant_typeOf(&variant))))

GBL_DECLS_BEGIN

typedef GBL_RESULT (*GblOpaqueCopyFn)(void* pOpaque, void** ppNewOpaque);
typedef GBL_RESULT (*GblOpaqueFreeFn)(void* pOpaque);

typedef struct GblOpaqueClassVTable {
    GblOpaqueCopyFn pFnCopy;
    GblOpaqueFreeFn pFnFree;
} GblOpaqueClassVTable;

GBL_CLASS_DERIVE(GblOpaque, GblPrimitive)
    const GblOpaqueClassVTable* pVTable;
GBL_CLASS_END

GBL_EXPORT GblType    GblOpaque_type     (void)                                GBL_NOEXCEPT;

GBL_EXPORT GblType    GblOpaque_register (const char*                 pName,
                                          const GblOpaqueClassVTable* pVTable) GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT GblOpaque_copy     (void*      pOpaque,
                                          GblType    type,
                                          void**     ppNewOpaque)              GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT GblOpaque_free     (void*      pOpaque,
                                          GblType    type)                     GBL_NOEXCEPT;

GBL_DECLS_END

#endif // GIMBAL_Opaque_H
