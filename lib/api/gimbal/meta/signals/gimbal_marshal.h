/*! \file
 *  \brief   Builtin GblMarshalFn functions and generator macros
 *  \ingroup signals
 *
 *  This file contains the declarations and utilities for
 *  GblMarshalFn and various builtin implementations of it.
 *
 *  \sa GblMarshalFn
 *
 *  \author     2023 Falco Girgis
 *  \copyright  MIT License
 */
#ifndef GIMBAL_MARSHAL_H
#define GIMBAL_MARSHAL_H

#include "../../core/gimbal_typedefs.h"
#include "../types/gimbal_type.h"
#include <stdarg.h>

/*! \name  Generator Macros
 *  \brief Macro utilities for autogenerating a  GblMarshalFn
 *  @{
 */
//! Declares a GblMarshalFn for a GblCClosure which returns nothing and has the given name postfix (usually underscore separated arg types)
#define GBL_DECL_CCLOSURE_MARSHAL_VOID__(postFix) \
            GBL_DECL_CCLOSURE_MARSHAL_VOID___(postFix)
//! Defines a GblMarshalFn for a GblCClosure autogenerating the implementation with the given info
#define GBL_DEFINE_CCLOSURE_MARSHAL_VOID__(paramsPostfix, paramCount, paramList, argList) \
            GBL_DEFINE_CCLOSURE_MARSHAL_VOID___(paramsPostfix, paramCount, paramList, argList)
//! @}

GBL_DECLS_BEGIN

GBL_FORWARD_DECLARE_STRUCT(GblClosure);
GBL_FORWARD_DECLARE_STRUCT(GblVariant);

/*! Function signature for a generic marshal function
 *  \ingroup signals
 *
 *  Marshal functions form the basis of the closure system,
 *  and are used by all GblClosure instances. They are the primary
 *  entry point for generic function invocation, and are the glue
 *  between a generic GblClosure and an actual language-dependent
 *  real function call.
 *
 *  They take a generic list of arguments, marshal them into an
 *  actual function (which is typically stored with GblClosure),
 *  then demarshal out the return type.
 */
typedef GBL_RESULT (*GblMarshalFn)  (GblClosure* pClosure,
                                     GblVariant* pRetValue,
                                     size_t      argCount,
                                     GblVariant* pArgs,
                                     GblPtr      pMarshalData);

//! Unimplemented, but mirror of GblMarshalFn using a va_list to hold args instead of GblVariant
typedef GBL_RESULT (*GblMarshalVaFn)(GblClosure*    pClosure,
                                     GblVariant*    pRetValue,
                                     void*          pInstance,
                                     va_list        args,
                                     void*          pMarshalData,
                                     size_t         argCount,
                                     const GblType* pParamTypes);

/*! \name  Builtin Marshals
 *  \brief Marshals which come pregenerated with libGimbal
 *  @{
 */
//!< GblMarshalFn which takes nothing and returns nothing
GBL_EXPORT GBL_RESULT GblMarshal_CClosure_VOID__VOID (GblClosure*        pClosure,
                                                      GblVariant*        pRetValue,
                                                      size_t             argCount,
                                                      GblVariant*        pArgs,
                                                      GblPtr             pMarshalData) GBL_NOEXCEPT;
//!< GblMarshalFn to be used as the meta marshal for GblClassClosure
GBL_EXPORT GBL_RESULT GblMarshal_ClassClosureMeta   (GblClosure*        pClosure,
                                                     GblVariant*        pRetValue,
                                                     size_t             argCount,
                                                     GblVariant*        pArgs,
                                                     GblPtr             pMarshalData)  GBL_NOEXCEPT;
//!< GblMarshalFn to be used as the marshal for GblSignalClosure
GBL_EXPORT GBL_RESULT GblMarshal_SignalForwarder    (GblClosure*        pClosure,
                                                     GblVariant*        pRetValue,
                                                     size_t             argCount,
                                                     GblVariant*        pArgs,
                                                     GblPtr             pMarshalData)  GBL_NOEXCEPT;

#define GBL_DECL_CCLOSURE_MARSHAL_VOID___(postFix)                \
    GBL_EXPORT GBL_RESULT                                         \
    GblMarshal_CClosure_VOID__##postFix(GblClosure* pClosure,     \
                                        GblVariant* pRetValue,    \
                                        size_t      argCount,     \
                                        GblVariant* pArgs,        \
                                        GblPtr      pMarshalData) GBL_NOEXCEPT

GBL_DECL_CCLOSURE_MARSHAL_VOID__(INSTANCE);

GBL_DECL_CCLOSURE_MARSHAL_VOID__(INSTANCE_BOOL);
GBL_DECL_CCLOSURE_MARSHAL_VOID__(INSTANCE_CHAR);
GBL_DECL_CCLOSURE_MARSHAL_VOID__(INSTANCE_UINT8);
GBL_DECL_CCLOSURE_MARSHAL_VOID__(INSTANCE_UINT16);
GBL_DECL_CCLOSURE_MARSHAL_VOID__(INSTANCE_INT16);
GBL_DECL_CCLOSURE_MARSHAL_VOID__(INSTANCE_UINT32);
GBL_DECL_CCLOSURE_MARSHAL_VOID__(INSTANCE_INT32);
GBL_DECL_CCLOSURE_MARSHAL_VOID__(INSTANCE_UINT64);
GBL_DECL_CCLOSURE_MARSHAL_VOID__(INSTANCE_INT64);
GBL_DECL_CCLOSURE_MARSHAL_VOID__(INSTANCE_SIZE);
GBL_DECL_CCLOSURE_MARSHAL_VOID__(INSTANCE_FLOAT);
GBL_DECL_CCLOSURE_MARSHAL_VOID__(INSTANCE_DOUBLE);
GBL_DECL_CCLOSURE_MARSHAL_VOID__(INSTANCE_STRING);
GBL_DECL_CCLOSURE_MARSHAL_VOID__(INSTANCE_ENUM);
GBL_DECL_CCLOSURE_MARSHAL_VOID__(INSTANCE_FLAGS);
GBL_DECL_CCLOSURE_MARSHAL_VOID__(INSTANCE_POINTER);
GBL_DECL_CCLOSURE_MARSHAL_VOID__(INSTANCE_OPAQUE);
GBL_DECL_CCLOSURE_MARSHAL_VOID__(INSTANCE_INSTANCE);
GBL_DECL_CCLOSURE_MARSHAL_VOID__(INSTANCE_BOX);

GBL_DECL_CCLOSURE_MARSHAL_VOID__(INSTANCE_INSTANCE_SIZE);

//! @}

GBL_DECLS_END

#define GBL_DEFINE_CCLOSURE_MARSHAL_VOID___(paramsPostfix, paramCount, paramList, argList)      \
    GBL_EXPORT GBL_RESULT GblMarshal_CClosure_VOID__##paramsPostfix(GblClosure* pClosure,       \
                                                                    GblVariant* pRetValue,      \
                                                                    size_t      argCount,       \
                                                                    GblVariant* pArgs,          \
                                                                    GblPtr      pMarshalData)   \
    GBL_NOEXCEPT {                                                                              \
        GBL_UNUSED(pRetValue, argCount);                                                        \
        typedef void (*CFunction)(GBL_EVAL paramList);                                          \
        GBL_ASSERT(argCount >= paramCount);                                                     \
        GBL_ASSERT(!pRetValue || GblVariant_typeOf(pRetValue) == GBL_INVALID_TYPE);             \
        GblCClosure* pCClosure = (GblCClosure*)pClosure;                                        \
        CFunction pFnPtr = (CFunction)((pMarshalData.pFunc)?                                    \
                            pMarshalData.pFunc : GBL_PRIV_REF(pCClosure).pFnCallback);          \
        pFnPtr(GBL_EVAL argList);                                                               \
        return GBL_RESULT_SUCCESS;                                                              \
    }

#endif // GIMBAL_MARSHAL_H
