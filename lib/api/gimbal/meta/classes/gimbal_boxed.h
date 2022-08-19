/*! \file
 *  \brief GblBoxedClass and API for managing boxed types
 *  \ingroup metaBuiltinTypes
 */

#ifndef GIMBAL_BOXED_H
#define GIMBAL_BOXED_H

#include "gimbal_primitives.h"

/// \ingroup metaBuiltinTypes
#define GBL_BOXED_TYPE                  (GBL_BUILTIN_TYPE(BOXED))
#define GBL_BOXED_CLASS_STRUCT          GblBoxedClass
#define GBL_BOXED_TYPE_CHECK(type)      (GblType_check(type, GBL_BOXED_TYPE))

#define GBL_BOXED_CLASS(klass)          (GBL_CLASS_CAST_PREFIX(klass, GBL_BOXED))
#define GBL_BOXED_CLASS_CHECK(klass)    (GBL_CLASS_CHECK_PREFIX(klass, GBL_BOXED))
#define GBL_BOXED_CLASS_TRY(klass)      (GBL_CLASS_TRY_PREFIX(klass, GBL_BOXED))

#define GBL_BOXED_GET_CLASS(variant)    (GBL_BOXED_CLASS(GblClass_weakRefDefault(GblVariant_typeOf(&variant))))
#define GBL_BOXED_TRY_CLASS(variant)    (GBL_BOXED_CLASS_TRY(GblClass_weakRefDefault(GblVariant_typeOf(&variant))))

GBL_DECLS_BEGIN

typedef GBL_RESULT (*GblBoxedCopyFn)    (void* pBoxed, void** ppNewBoxed);
typedef GBL_RESULT (*GblBoxedFreeFn)    (void* pBoxed);

typedef struct GblBoxedClassVTable {
    GblBoxedCopyFn      pFnCopy;
    GblBoxedFreeFn      pFnFree;
} GblBoxedClassVTable;

GBL_CLASS_DERIVE(GblBoxedClass, GblPrimitiveClass)
    const GblBoxedClassVTable*      pVTable;
GBL_CLASS_END

GBL_EXPORT GblType     GblBoxed_register   (const char*                pName,
                                            const GblBoxedClassVTable* pVTable)    GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT  GblBoxed_copy       (void*      pBoxed,
                                            GblType    type,
                                            void**     ppNewBoxed)                 GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT  GblBoxed_free       (void*      pBoxed,
                                            GblType    type)                       GBL_NOEXCEPT;

GBL_DECLS_END

#endif // GIMBAL_BOXED_H
