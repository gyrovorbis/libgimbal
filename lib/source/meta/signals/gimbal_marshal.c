#include <gimbal/meta/signals/gimbal_marshal.h>
#include <gimbal/meta/signals/gimbal_closure.h>
#include <gimbal/meta/signals/gimbal_c_closure.h>
#include <gimbal/meta/signals/gimbal_class_closure.h>
#include <gimbal/meta/signals/gimbal_signal_closure.h>
#include <gimbal/meta/types/gimbal_variant.h>
#include <gimbal/meta/signals/gimbal_signal.h>

GBL_EXPORT GBL_RESULT GblCClosureMarshal_VOID__VOID(GblClosure* pClosure,
                                                    GblVariant* pRetValue,
                                                    size_t      argCount,
                                                    GblVariant* pArgs,
                                                    GblPtr      pMarshalData)
{
    GBL_UNUSED(pRetValue && argCount && pArgs);

    typedef void (*CFunction)(void* pReceiver);

    GblCClosure* pCClosure = (GblCClosure*)pClosure;
    GblFnPtr     pFnPtr    = (CFunction)pMarshalData.pFunc?
                                             pMarshalData.pFunc : GBL_PRIV_REF(pCClosure).pFnCallback;

    pFnPtr();

    return GBL_RESULT_SUCCESS;
}

GBL_DEFINE_CCLOSURE_MARSHAL_VOID__(INSTANCE,
                                   1,
                                   (GblInstance*),
                                   (GblVariant_toPointer(&pArgs[0])))

GBL_DEFINE_CCLOSURE_MARSHAL_VOID__(INSTANCE_BOOL,
                                   2,
                                   (GblInstance*, GblBool),
                                   (GblVariant_toPointer(&pArgs[0]), GblVariant_toBool(&pArgs[1])))

GBL_DEFINE_CCLOSURE_MARSHAL_VOID__(INSTANCE_CHAR,
                                   2,
                                   (GblInstance*, char),
                                   (GblVariant_toPointer(&pArgs[0]), GblVariant_toChar(&pArgs[1])))

GBL_DEFINE_CCLOSURE_MARSHAL_VOID__(INSTANCE_UINT8,
                                   2,
                                   (GblInstance*, uint8_t),
                                   (GblVariant_toPointer(&pArgs[0]), GblVariant_toUint8(&pArgs[1])))

GBL_DEFINE_CCLOSURE_MARSHAL_VOID__(INSTANCE_UINT16,
                                   2,
                                   (GblInstance*, uint16_t),
                                   (GblVariant_toPointer(&pArgs[0]), GblVariant_toUint16(&pArgs[1])))

GBL_DEFINE_CCLOSURE_MARSHAL_VOID__(INSTANCE_INT16,
                                   2,
                                   (GblInstance*, int16_t),
                                   (GblVariant_toPointer(&pArgs[0]), GblVariant_toInt16(&pArgs[1])))

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

GBL_DEFINE_CCLOSURE_MARSHAL_VOID__(INSTANCE_SIZE,
                                   2,
                                   (GblInstance*, size_t),
                                   (GblVariant_toPointer(&pArgs[0]), GblVariant_toSize(&pArgs[1])))

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

GBL_DEFINE_CCLOSURE_MARSHAL_VOID__(INSTANCE_OPAQUE,
                                   2,
                                   (GblInstance*, void*),
                                   (GblVariant_toPointer(&pArgs[0]), GblVariant_toPointer(&pArgs[1])))

GBL_DEFINE_CCLOSURE_MARSHAL_VOID__(INSTANCE_INSTANCE,
                                   2,
                                   (GblInstance*, GblInstance*),
                                   (GblVariant_toPointer(&pArgs[0]), GblVariant_toPointer(&pArgs[1])))

GBL_DEFINE_CCLOSURE_MARSHAL_VOID__(INSTANCE_BOX,
                                   2,
                                   (GblInstance*, GblInstance*),
                                   (GblVariant_toPointer(&pArgs[0]), GblVariant_boxPeek(&pArgs[1])))

GBL_DEFINE_CCLOSURE_MARSHAL_VOID__(INSTANCE_INSTANCE_SIZE,
                                   3,
                                   (GblInstance*, GblInstance*, size_t),
                                   (GblVariant_toPointer(&pArgs[0]), GblVariant_toPointer(&pArgs[1]), GblVariant_toSize(&pArgs[2])));


GBL_EXPORT GBL_RESULT GblMarshal_ClassClosureMeta(GblClosure*        pClosure,
                                                  GblVariant*        pRetValue,
                                                  size_t             argCount,
                                                  GblVariant*        pArgs,
                                                  GblPtr             pMarshalData)
{
    GBL_CTX_BEGIN(NULL);
    GBL_CTX_VERIFY_ARG(argCount);

    GblClassClosure* pClassClosure = (GblClassClosure*)pClosure;

    GBL_CTX_VERIFY_TYPE(GBL_PRIV_REF(pClassClosure).classType);

    GBL_CTX_VERIFY(GBL_PRIV_REF(pClassClosure).pInstance,
                   GBL_RESULT_ERROR_INVALID_OPERATION,
                   "Attempt to meta marshal class [%s] closure with no instance pointer!",
                   GblType_name(GBL_PRIV_REF(pClassClosure).classType));

    uint8_t* pClass = (uint8_t*)GblClass_cast(GblInstance_class(GBL_PRIV_REF(pClassClosure).pInstance),
                                              GBL_PRIV_REF(pClassClosure).classType);

    pMarshalData.pFunc = *(GblFnPtr*)(uint8_t*)(pClass + GBL_PRIV_REF(pClassClosure).offset);

    GBL_CTX_VERIFY_CALL(GBL_PRIV_REF(pClosure).pFnMarshal(pClosure, pRetValue, argCount, pArgs, pMarshalData));
    GBL_CTX_END();
}

GBL_EXPORT GBL_RESULT GblMarshal_SignalForwarder(GblClosure* pClosure,
                                                 GblVariant* pRetValue,
                                                 size_t      argCount,
                                                 GblVariant* pArgs,
                                                 GblPtr      pMarshalData)
{
    GBL_UNUSED(pRetValue);
    GBL_UNUSED(pMarshalData);
    GBL_CTX_BEGIN(NULL);
    GBL_CTX_VERIFY_ARG(argCount >= 1);

    GblSignalClosure* pSignalClosure = (GblSignalClosure*)pClosure;

    GBL_CTX_VERIFY_CALL(GblSignal_emitVariants(GblVariant_toPointer(&pArgs[0]),
                                               GblQuark_toString(GBL_PRIV_REF(pSignalClosure).signalName),
                                               argCount > 1? &pArgs[1] : NULL));

    GBL_CTX_END();

}
