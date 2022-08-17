/*! \file
 *  \brief GblIVariant interface for GblVariant compatibility
 *  \ingroup interfaces
 */


#ifndef GIMBAL_IVARIANT_H
#define GIMBAL_IVARIANT_H

#include "gimbal_interface.h"

#define GBL_IVARIANT_TYPE                       (GBL_BUILTIN_TYPE(IVARIANT))
#define GBL_IVARIANT_STRUCT                     GblIVariant
#define GBL_IVARIANT_CLASS_STRUCT               GblIVariantIFace
#define GBL_IVARIANT(instance)                  (GBL_INSTANCE_CAST_PREFIX       (instance,  GBL_IVARIANT))
#define GBL_IVARIANT_CHECK(instance)            (GBL_INSTANCE_CHECK_PREFIX      (instance,  GBL_IVARIANT))
#define GBL_IVARIANT_IFACE(klass)               (GBL_CLASS_CAST_PREFIX          (klass,     GBL_IVARIANT))
#define GBL_IVARIANT_IFACE_CHECK(klass)         (GBL_CLASS_CHECK_PREFIX         (klass,     GBL_IVARIANT))
#define GBL_IVARIANT_GET_IFACE(instance)        (GBL_INSTANCE_GET_CLASS_PREFIX (instance,  GBL_IVARIANT))

#define GBL_IVARIANT_VALUE_VAR_ARG_MAX          4

GBL_DECLS_BEGIN

GBL_FORWARD_DECLARE_STRUCT_PRIVATE(GblIVariant);

GBL_DECLARE_FLAGS(GBL_IVARIANT_OP_FLAGS) {
    GBL_IVARIANT_OP_FLAG_RELOCATABLE           = 0x0001,
    GBL_IVARIANT_OP_FLAG_CONSTRUCT_DEFAULT     = 0x0002,  //without this, assume default 0 init is fine
    GBL_IVARIANT_OP_FLAG_CONSTRUCT_COPY        = 0x0004,
    GBL_IVARIANT_OP_FLAG_CONSTRUCT_MOVE        = 0x0008,
    GBL_IVARIANT_OP_FLAG_CONSTRUCT_VALUE_COPY  = 0x0010,
    GBL_IVARIANT_OP_FLAG_CONSTRUCT_VALUE_MOVE  = 0x0020,
    GBL_IVARIANT_OP_FLAG_CONSTRUCT_MASK        = 0x003e,
    GBL_IVARIANT_OP_FLAG_SET_COPY              = 0x0040,  //without this, assume memcpy works
    GBL_IVARIANT_OP_FLAG_SET_MOVE              = 0x0080,
    GBL_IVARIANT_OP_FLAG_SET_VALUE_COPY        = 0x0100,
    GBL_IVARIANT_OP_FLAG_SET_VALUE_MOVE        = 0x0200,
    GBL_IVARIANT_OP_FLAG_SET_MASK              = 0x03c0,
    GBL_IVARIANT_OP_FLAG_GET_VALUE_COPY        = 0x0400,
    GBL_IVARIANT_OP_FLAG_GET_VALUE_PEEK        = 0x0800,
    GBL_IVARIANT_OP_FLAG_GET_VALUE_MOVE        = 0x1000,
    GBL_IVARIANT_OP_FLAG_GET_MASK              = 0x1c00,
    GBL_IVARIANT_OP_FLAG_VALUELESS_TYPE        = 0x2000
};

#define VARIANT     GblVariant* pVariant
#define CVARIANT    const VARIANT

typedef struct GblIVariantIFaceVTable {
    GBL_IVARIANT_OP_FLAGS   supportedOps;
    char                    pSetValueFmt[GBL_IVARIANT_VALUE_VAR_ARG_MAX];
    char                    pGetValueFmt[GBL_IVARIANT_VALUE_VAR_ARG_MAX];

    GBL_RESULT (*pFnConstruct)(VARIANT,  GblUint argc, GblVariant* pArgs, GBL_IVARIANT_OP_FLAGS op);
    GBL_RESULT (*pFnSet)      (VARIANT,  GblUint argc, GblVariant* pArgs, GBL_IVARIANT_OP_FLAGS op);
    GBL_RESULT (*pFnGet)      (VARIANT,  GblUint argc, GblVariant* pArgs, GBL_IVARIANT_OP_FLAGS op);
    GBL_RESULT (*pFnDestruct) (VARIANT);
    GBL_RESULT (*pFnCompare)  (CVARIANT, const GblVariant* pOther, GblInt* pResult);
    GBL_RESULT (*pFnSave)     (CVARIANT, GblStringBuffer* pString);
    GBL_RESULT (*pFnLoad)     (VARIANT,  const GblStringBuffer* pString);
} GblIVariantIFaceVTable;

typedef struct GblIVariantIFace {
    GblInterface                    base;
    const GblIVariantIFaceVTable*   pVTable;
} GblIVariantIFace;


#define SELF    GblIVariantIFace* pSelf
#define CSELF   const SELF

GBL_API GblIVariantIFace_validate            (CSELF)                                        GBL_NOEXCEPT;
GBL_API GblIVariantIFace_constructDefault    (CSELF, VARIANT)                               GBL_NOEXCEPT;
GBL_API GblIVariantIFace_constructCopy       (CSELF, VARIANT,  const GblVariant* pOther)    GBL_NOEXCEPT;
GBL_API GblIVariantIFace_constructMove       (CSELF, VARIANT,  GblVariant* pOther)          GBL_NOEXCEPT;
GBL_API GblIVariantIFace_constructValueCopy  (CSELF, VARIANT,  va_list* pVarArgs)           GBL_NOEXCEPT;
GBL_API GblIVariantIFace_constructValueMove  (CSELF, VARIANT,  va_list* pVarArgs)           GBL_NOEXCEPT;
GBL_API GblIVariantIFace_setCopy             (CSELF, VARIANT,  const GblVariant* pOther)    GBL_NOEXCEPT;
GBL_API GblIVariantIFace_setMove             (CSELF, VARIANT,  GblVariant* pOther)          GBL_NOEXCEPT;
GBL_API GblIVariantIFace_setValueCopy        (CSELF, VARIANT,  va_list* pVarArgs)           GBL_NOEXCEPT;
GBL_API GblIVariantIFace_setValueMove        (CSELF, VARIANT,  va_list* pVarArgs)           GBL_NOEXCEPT;
GBL_API GblIVariantIFace_getValueCopy        (CSELF, CVARIANT, va_list* pVarArgs)           GBL_NOEXCEPT;
GBL_API GblIVariantIFace_getValuePeek        (CSELF, CVARIANT, va_list* pVarArgs)           GBL_NOEXCEPT;
GBL_API GblIVariantIFace_getValueMove        (CSELF, CVARIANT, va_list* pVarArgs)           GBL_NOEXCEPT;
GBL_API GblIVariantIFace_destruct            (CSELF, VARIANT)                               GBL_NOEXCEPT;

GBL_API GblIVariantIFace_compare             (CSELF,
                                              CVARIANT,
                                              const GblVariant* pOther,
                                              GblInt* pCmpResult)                           GBL_NOEXCEPT;

GBL_API GblIVariantIFace_save                (CSELF,
                                              CVARIANT,
                                              GblStringBuffer* pString)                     GBL_NOEXCEPT;

GBL_API GblIVariantIFace_load                (CSELF,
                                              VARIANT,
                                              const GblStringBuffer* pString)               GBL_NOEXCEPT;

#undef CVARIANT
#undef VARIANT

#undef CSELF
#undef SELF

GBL_DECLS_END

#endif // GIMBAL_IVARIANT_H
