/*! \file
 *  \brief Builtin closure marshals and generator macros
 *  \ingroup signals
 */

#ifndef GIMBAL_MARSHAL_H
#define GIMBAL_MARSHAL_H

#include "gimbal_typedefs.h"
#include "../meta/gimbal_type.h"

#define GBL_DECL_CCLOSURE_MARSHAL_VOID__(postFix) \
GBL_EXPORT GBL_RESULT GblMarshal_CClosure_VOID__##postFix(GblClosure* pClosure,     \
                                                          GblVariant* pRetValue,    \
                                                          GblSize     argCount,     \
                                                          GblVariant* pArgs,        \
                                                          GblPtr      pMarshalData) GBL_NOEXCEPT

GBL_FORWARD_DECLARE_STRUCT(GblClosure);

typedef GBL_RESULT (*GblMarshalFn)  (GblClosure*        pClosure,
                                     GblVariant*        pRetValue,
                                     GblSize            argCount,
                                     GblVariant*        pArgs,
                                     GblPtr             pMarshalData);

typedef GBL_RESULT (*GblMarshalVaFn)(GblClosure*        pClosure,
                                     GblVariant*        pRetValue,
                                     void*              pInstance,
                                     va_list            args,
                                     void*              pMarshalData,
                                     GblSize            argCount,
                                     const GblType*     pParamTypes);

GBL_EXPORT GBL_RESULT GblMarshal_CClosure_VOID__VOID (GblClosure*        pClosure,
                                                      GblVariant*        pRetValue,
                                                      GblSize            argCount,
                                                      GblVariant*        pArgs,
                                                      GblPtr             pMarshalData)   GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT GblMarshal_ClassClosureMeta   (GblClosure*        pClosure,
                                                     GblVariant*        pRetValue,
                                                     GblSize            argCount,
                                                     GblVariant*        pArgs,
                                                     GblPtr             pMarshalData)   GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT GblMarshal_SignalForwarder    (GblClosure*        pClosure,
                                                     GblVariant*        pRetValue,
                                                     GblSize            argCount,
                                                     GblVariant*        pArgs,
                                                     GblPtr             pMarshalData)   GBL_NOEXCEPT;


GBL_DECL_CCLOSURE_MARSHAL_VOID__(INSTANCE_BOOL);
GBL_DECL_CCLOSURE_MARSHAL_VOID__(INSTANCE_UINT32);
GBL_DECL_CCLOSURE_MARSHAL_VOID__(INSTANCE_INT32);
GBL_DECL_CCLOSURE_MARSHAL_VOID__(INSTANCE_UINT64);
GBL_DECL_CCLOSURE_MARSHAL_VOID__(INSTANCE_INT64);
GBL_DECL_CCLOSURE_MARSHAL_VOID__(INSTANCE_FLOAT);
GBL_DECL_CCLOSURE_MARSHAL_VOID__(INSTANCE_DOUBLE);
GBL_DECL_CCLOSURE_MARSHAL_VOID__(INSTANCE_STRING);
GBL_DECL_CCLOSURE_MARSHAL_VOID__(INSTANCE_ENUM);
GBL_DECL_CCLOSURE_MARSHAL_VOID__(INSTANCE_FLAGS);
GBL_DECL_CCLOSURE_MARSHAL_VOID__(INSTANCE_POINTER);
GBL_DECL_CCLOSURE_MARSHAL_VOID__(INSTANCE_BOXED);
// add event type


#define GBL_DEFINE_CCLOSURE_MARSHAL_VOID__(paramsPostfix, paramCount, paramList, argList)       \
    GBL_EXPORT GBL_RESULT GblMarshal_CClosure_VOID__##paramsPostfix(GblClosure* pClosure,       \
                                                                    GblVariant* pRetValue,      \
                                                                    GblSize     argCount,       \
                                                                    GblVariant* pArgs,          \
                                                                    GblPtr      pMarshalData)   \
    {                                                                                           \
        GBL_UNUSED(pRetValue);                                                                  \
        typedef void (*CFunction)(GBL_EVAL paramList);                                          \
        GBL_ASSERT(argCount >= paramCount);                                                     \
        GBL_ASSERT(!pRetValue || GblVariant_typeOf(pRetValue) == GBL_INVALID_TYPE);               \
        GblCClosure* pCClosure = (GblCClosure*)pClosure;                                        \
        CFunction pFnPtr = (CFunction)((pMarshalData.pFunc)?                                    \
                            pMarshalData.pFunc : pCClosure->pFnCCallback);                      \
        pFnPtr(GBL_EVAL argList);                                                               \
        return GBL_RESULT_SUCCESS;                                                              \
    }


#endif // GIMBAL_MARSHAL_H
