/*! \file
 *  \brief GblIVariant interface for GblVariant compatibility
 *  \ingroup interfaces
 *
 *  \todo
 *  - Shouldn't the RHS variant automatically be invalidated after a "move" operation?
 *
 *  \author 2023 Falco Girgis
 *  \copyright MIT License
 */
#ifndef GIMBAL_IVARIANT_H
#define GIMBAL_IVARIANT_H

#include "gimbal_interface.h"
#include <stdarg.h>

/*! \name Type System
 *  \brief Type UUID and cast operators
 *  @{
 */
#define GBL_IVARIANT_TYPE                   (GBL_BUILTIN_TYPE(IVARIANT))
#define GBL_IVARIANT(instance)              ((GblIVariant*)GblInstance_cast(GBL_INSTANCE(instance), GBL_IVARIANT_TYPE))
#define GBL_IVARIANT_CLASS(klass)           ((GblIVariantClass*)GblClass_cast(GBL_CLASS(klass), GBL_IVARIANT_TYPE))
#define GBL_IVARIANT_GET_CLASS(instance)    (GBL_IVARIANT_CLASS(GblInstance_class(GBL_INSTANCE(instance))))
//! @}

#define GBL_IVARIANT_VALUE_VAR_ARG_MAX          4

GBL_DECLS_BEGIN

GBL_FORWARD_DECLARE_STRUCT(GblVariant);
GBL_FORWARD_DECLARE_STRUCT(GblStringBuffer);

GBL_DECLARE_FLAGS(GBL_IVARIANT_OP_FLAGS) {
    GBL_IVARIANT_OP_FLAG_RELOCATABLE          = 0x00001,
    GBL_IVARIANT_OP_FLAG_CONSTRUCT_DEFAULT    = 0x00002,
    GBL_IVARIANT_OP_FLAG_CONSTRUCT_COPY       = 0x00004,
    GBL_IVARIANT_OP_FLAG_CONSTRUCT_MOVE       = 0x00008,
    GBL_IVARIANT_OP_FLAG_CONSTRUCT_VALUE_COPY = 0x00010,
    GBL_IVARIANT_OP_FLAG_CONSTRUCT_VALUE_MOVE = 0x00020,
    GBL_IVARIANT_OP_FLAG_CONSTRUCT_MASK       = 0x0003e,
    GBL_IVARIANT_OP_FLAG_SET_COPY             = 0x00040,
    GBL_IVARIANT_OP_FLAG_SET_MOVE             = 0x00080,
    GBL_IVARIANT_OP_FLAG_SET_VALUE_COPY       = 0x00100,
    GBL_IVARIANT_OP_FLAG_SET_VALUE_MOVE       = 0x00200,
    GBL_IVARIANT_OP_FLAG_SET_MASK             = 0x003c0,
    GBL_IVARIANT_OP_FLAG_GET_VALUE_COPY       = 0x00400,
    GBL_IVARIANT_OP_FLAG_GET_VALUE_PEEK       = 0x00800,
    GBL_IVARIANT_OP_FLAG_GET_VALUE_MOVE       = 0x01000,
    GBL_IVARIANT_OP_FLAG_GET_MASK             = 0x01c00,
    GBL_IVARIANT_OP_FLAG_VALUELESS_TYPE       = 0x40000
};

#define VARIANT     GblVariant* pVariant
#define CVARIANT    const VARIANT

//! Virtual method table for GblIVariantClass
typedef struct GblIVariantVTable {
    GBL_IVARIANT_OP_FLAGS   supportedOps;
    char                    pSetValueFmt[GBL_IVARIANT_VALUE_VAR_ARG_MAX];
    char                    pGetValueFmt[GBL_IVARIANT_VALUE_VAR_ARG_MAX];

    GBL_RESULT (*pFnConstruct)(VARIANT, size_t argc, GblVariant* pArgs, GBL_IVARIANT_OP_FLAGS op);
    GBL_RESULT (*pFnDestruct) (VARIANT);
    GBL_RESULT (*pFnSet)      (VARIANT, size_t argc, GblVariant* pArgs, GBL_IVARIANT_OP_FLAGS op);
    GBL_RESULT (*pFnGet)      (VARIANT, size_t argc, GblVariant* pArgs, GBL_IVARIANT_OP_FLAGS op);
    GBL_RESULT (*pFnCompare)  (CVARIANT, const GblVariant* pOther, int* pResult);
    GBL_RESULT (*pFnSave)     (CVARIANT, GblStringBuffer* pString);
    GBL_RESULT (*pFnLoad)     (VARIANT,  const GblStringBuffer* pString);
} GblIVariantVTable;

/*! \struct GblIVariantClass
 *  \extends GblInterface
 *  \brief GblInterface for GblVariant type compatibilty
 *
 *  GblIVariantClass represents an interface that must
 *  be implemented by any type wishing to be stored
 *  within, convertible to/from, or compatible with a
 *  GblVariant in any way.
 *
 *  \sa GblVariant
 */
GBL_INTERFACE_DERIVE(GblIVariant)
    const GblIVariantVTable* pVTable;
GBL_INTERFACE_END

#define GBL_SELF_TYPE GblIVariantClass
#define GBL_KEY       const GblVariant* pKey
#define GBL_VALUE     GblVariant* pValue

GBL_EXPORT GBL_RESULT GblIVariantClass_validate           (GBL_CSELF)                                     GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT GblIVariantClass_constructDefault   (GBL_CSELF, VARIANT)                            GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT GblIVariantClass_constructCopy      (GBL_CSELF, VARIANT,  const GblVariant* pOther) GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT GblIVariantClass_constructMove      (GBL_CSELF, VARIANT,  GblVariant* pOther)       GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT GblIVariantClass_constructValueCopy (GBL_CSELF, VARIANT,  va_list* pVarArgs)        GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT GblIVariantClass_constructValueMove (GBL_CSELF, VARIANT,  va_list* pVarArgs)        GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT GblIVariantClass_setCopy            (GBL_CSELF, VARIANT,  const GblVariant* pOther) GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT GblIVariantClass_setMove            (GBL_CSELF, VARIANT,  GblVariant* pOther)       GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT GblIVariantClass_setValueCopy       (GBL_CSELF, VARIANT,  va_list* pVarArgs)        GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT GblIVariantClass_setValueMove       (GBL_CSELF, VARIANT,  va_list* pVarArgs)        GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT GblIVariantClass_getValueCopy       (GBL_CSELF, CVARIANT, va_list* pVarArgs)        GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT GblIVariantClass_getValuePeek       (GBL_CSELF, CVARIANT, va_list* pVarArgs)        GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT GblIVariantClass_getValueMove       (GBL_CSELF, CVARIANT, va_list* pVarArgs)        GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT GblIVariantClass_destruct           (GBL_CSELF, VARIANT)                            GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT GblIVariantClass_compare            (GBL_CSELF,
                                                           CVARIANT,
                                                           const GblVariant* pOther,
                                                           int* pCmpResult)                               GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT GblIVariantClass_save               (GBL_CSELF, CVARIANT, GblStringBuffer* pString) GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT GblIVariantClass_load               (GBL_CSELF,
                                                           VARIANT,
                                                           const GblStringBuffer* pString)                GBL_NOEXCEPT;

#undef CVARIANT
#undef VARIANT

#undef GBL_SELF_TYPE
#undef GBL_KEY
#undef GBL_VALUE

GBL_DECLS_END

#endif // GIMBAL_IVARIANT_H
