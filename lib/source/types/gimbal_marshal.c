#include <gimbal/types/gimbal_marshal.h>
#include <gimbal/types/gimbal_closure.h>
#include <gimbal/types/gimbal_variant.h>
#include <gimbal/meta/gimbal_signal.h>

GBL_EXPORT GBL_RESULT GblCClosureMarshal_VOID__VOID(GblClosure*        pClosure,
                                                    GblVariant*        pRetValue,
                                                    GblSize            argCount,
                                                    GblVariant*        pArgs,
                                                    GblPtr             pMarshalData)
{
    GBL_UNUSED(pRetValue && argCount && pArgs);

    typedef void (*CFunction)(void* pReceiver);

    GblCClosure*    pCClosure   = (GblCClosure*)pClosure;
    GblFnPtr        pFnPtr      = (CFunction)pMarshalData.pFunc?
                                             pMarshalData.pFunc : pCClosure->pFnCCallback;

    pFnPtr(pClosure->pUserdata);

    return GBL_RESULT_SUCCESS;
}

GBL_DEFINE_CCLOSURE_MARSHAL_VOID__(INSTANCE_BOOL,
                                   2,
                                   (GblInstance*, GblBool),
                                   (GblVariant_toPointer(&pArgs[0]), GblVariant_toBool(&pArgs[1])))

GBL_DEFINE_CCLOSURE_MARSHAL_VOID__(INSTANCE_UINT32,
                                   2,
                                   (GblInstance*, uint32_t),
                                   (GblVariant_toPointer(&pArgs[0]), GblVariant_toUint32(&pArgs[1])))

GBL_DEFINE_CCLOSURE_MARSHAL_VOID__(INSTANCE_INT32,
                                   2,
                                   (GblInstance*, int32_t),
                                   (GblVariant_toPointer(&pArgs[0]), GblVariant_toInt32(&pArgs[1])))

GBL_DEFINE_CCLOSURE_MARSHAL_VOID__(INSTANCE_UINT64,
                                   2,
                                   (GblInstance*, uint64_t),
                                   (GblVariant_toPointer(&pArgs[0]), GblVariant_toUint64(&pArgs[1])))

GBL_DEFINE_CCLOSURE_MARSHAL_VOID__(INSTANCE_INT64,
                                   2,
                                   (GblInstance*, int64_t),
                                   (GblVariant_toPointer(&pArgs[0]), GblVariant_toInt64(&pArgs[1])))

GBL_DEFINE_CCLOSURE_MARSHAL_VOID__(INSTANCE_FLOAT,
                                   2,
                                   (GblInstance*, float),
                                   (GblVariant_toPointer(&pArgs[0]), GblVariant_toFloat(&pArgs[1])))

GBL_DEFINE_CCLOSURE_MARSHAL_VOID__(INSTANCE_DOUBLE,
                                   2,
                                   (GblInstance*, double),
                                   (GblVariant_toPointer(&pArgs[0]), GblVariant_toDouble(&pArgs[1])))

GBL_DEFINE_CCLOSURE_MARSHAL_VOID__(INSTANCE_STRING,
                                   2,
                                   (GblInstance*, const char*),
                                   (GblVariant_toPointer(&pArgs[0]), GblVariant_toString(&pArgs[1])))

GBL_DEFINE_CCLOSURE_MARSHAL_VOID__(INSTANCE_ENUM,
                                   2,
                                   (GblInstance*, GblEnum),
                                   (GblVariant_toPointer(&pArgs[0]), GblVariant_toEnum(&pArgs[1])))

GBL_DEFINE_CCLOSURE_MARSHAL_VOID__(INSTANCE_FLAGS,
                                   2,
                                   (GblInstance*, GblFlags),
                                   (GblVariant_toPointer(&pArgs[0]), GblVariant_toFlags(&pArgs[1])))

GBL_DEFINE_CCLOSURE_MARSHAL_VOID__(INSTANCE_POINTER,
                                   2,
                                   (GblInstance*, void*),
                                   (GblVariant_toPointer(&pArgs[0]), GblVariant_toPointer(&pArgs[1])))

GBL_DEFINE_CCLOSURE_MARSHAL_VOID__(INSTANCE_BOXED,
                                   2,
                                   (GblInstance*, void*),
                                   (GblVariant_toPointer(&pArgs[0]), GblVariant_toPointer(&pArgs[1])))


GBL_EXPORT GBL_RESULT GblMarshal_ClassClosureMeta(GblClosure*        pClosure,
                                                  GblVariant*        pRetValue,
                                                  GblSize            argCount,
                                                  GblVariant*        pArgs,
                                                  GblPtr             pMarshalData)
{
    GBL_API_BEGIN(GblClosure_context(pClosure));
    GBL_API_VERIFY_ARG(argCount);

    GblClassClosure* pClassClosure = (GblClassClosure*)pClosure;

    GBL_API_VERIFY_TYPE(pClassClosure->classType);

    GBL_API_VERIFY(pClosure->pUserdata,
                   GBL_RESULT_ERROR_INVALID_OPERATION,
                   "Attempt to meta marshal class [%s] closure with no instance pointer!",
                   GblType_name(pClassClosure->classType));

    uint8_t* pClass = GBL_INSTANCE_GET_CLASS(pClosure->pUserdata, pClassClosure->classType, uint8_t);
    pMarshalData.pFunc = *(GblFnPtr*)(uint8_t*)(pClass + pClassClosure->offset);

    GBL_API_VERIFY_CALL(pClosure->pFnMarshal(pClosure, pRetValue, argCount, pArgs, pMarshalData));
    GBL_API_END();
}

GBL_EXPORT GBL_RESULT GblMarshal_SignalForwarder(GblClosure* pClosure,
                                                 GblVariant* pRetValue,
                                                 GblSize     argCount,
                                                 GblVariant* pArgs,
                                                 GblPtr      pMarshalData)
{
    GBL_UNUSED(pRetValue);
    GBL_UNUSED(pMarshalData);
    GBL_API_BEGIN(GblClosure_context(pClosure));
    GBL_API_VERIFY_ARG(argCount >= 1);

    GblSignalClosure* pSignalClosure = (GblSignalClosure*)pClosure;

    GBL_API_VERIFY_CALL(GblSignal_emitVariants(GblVariant_toPointer(&pArgs[0]),
                                               pSignalClosure->pSignalName,
                                               argCount > 1? &pArgs[1] : NULL));

    GBL_API_END();

}
