#include <gimbal/meta/gimbal_boxed.h>
#include <gimbal/strings/gimbal_string_buffer.h>
#include <gimbal/types/gimbal_variant.h>

static GBL_RESULT GblBoxedClass_init_(GblClass* pClass, const void* pData, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblBoxedClass* pSelf = GBL_BOXED_CLASS(pClass);
    pSelf->pVTable = (const GblBoxedClassVTable*)pData;
    GBL_API_END();
}

static GBL_RESULT GblBoxed_construct_(GblVariant* pVariant, GblUint argc, GblVariant* pArgs, GBL_IVARIANT_OP_FLAGS op) {
    GBL_API_BEGIN(NULL);
    GBL_API_VERIFY_ARG(argc == 1 || (op & GBL_IVARIANT_OP_FLAG_CONSTRUCT_DEFAULT));

    // Default constructor
    if(op & GBL_IVARIANT_OP_FLAG_CONSTRUCT_DEFAULT) {
        //GBL_API_RECORD_SET(GBL_RESULT_ERROR_INVALID_OPERATION);
        pVariant->pVoid = NULL;

    // Copy constructor
    } else if(op & GBL_IVARIANT_OP_FLAG_CONSTRUCT_COPY) {
        GBL_API_VERIFY_TYPE(pArgs[0].type, GblVariant_type(pVariant));
        GBL_API_VERIFY_CALL(GblBoxed_copy(pArgs[0].pVoid,
                                          GblVariant_type(&pArgs[0]),
                                          &pVariant->pVoid));

    // Move constructor
    } else if(op & GBL_IVARIANT_OP_FLAG_CONSTRUCT_MOVE) {
        GBL_API_VERIFY_TYPE(pArgs[0].type, GblVariant_type(pVariant));
        pVariant->pVoid = pArgs[0].pVoid;
        GBL_API_CALL(GblVariant_invalidate(&pArgs[0]));

    // Value copying constructor
    } else if(op & GBL_IVARIANT_OP_FLAG_CONSTRUCT_VALUE_COPY) {
        GBL_API_VERIFY_TYPE(pArgs[0].type, GBL_POINTER_TYPE);
        GBL_API_VERIFY_CALL(GblBoxed_copy(pArgs[0].pVoid,
                                          GblVariant_type(pVariant),
                                          &pVariant->pVoid));

    // Value moving constructor
    } else if(op & GBL_IVARIANT_OP_FLAG_CONSTRUCT_VALUE_MOVE) {
        GBL_API_VERIFY_TYPE(pArgs[0].type, GBL_POINTER_TYPE);
        pVariant->pVoid = pArgs[0].pVoid;
    }
    GBL_API_END();
}

static GBL_RESULT GblBoxed_destruct_(GblVariant* pVariant) {
    GBL_API_BEGIN(NULL);

    GBL_API_VERIFY_CALL(GblBoxed_free(pVariant->pVoid,
                                      GblVariant_type(pVariant)));
    GBL_API_END();
}

static GBL_RESULT GblBoxed_set_(GblVariant* pVariant, GblUint argc, GblVariant* pArgs, GBL_IVARIANT_OP_FLAGS op) {
    GBL_API_BEGIN(NULL);
    GBL_UNUSED(argc);
    GBL_API_VERIFY_ARG(argc == 1);
    if(op & GBL_IVARIANT_OP_FLAG_SET_COPY) {
        GBL_API_VERIFY_TYPE(pArgs[0].type, GblVariant_type(pVariant));
        GBL_API_VERIFY_CALL(GblBoxed_free(pVariant->pVoid, GblVariant_type(pVariant)));
        GBL_API_VERIFY_CALL(GblBoxed_copy(pArgs[0].pVoid,
                                          GblVariant_type(&pArgs[0]),
                                          &pVariant->pVoid));
    }
    else if(op & GBL_IVARIANT_OP_FLAG_SET_MOVE) {
        GBL_API_VERIFY_TYPE(pArgs[0].type, GblVariant_type(pVariant));
        GBL_API_VERIFY_CALL(GblBoxed_free(pVariant->pVoid, GblVariant_type(pVariant)));
        pVariant->pVoid = pArgs[0].pVoid;
        GBL_API_CALL(GblVariant_invalidate(&pArgs[0]));
    }
    else if(op & GBL_IVARIANT_OP_FLAG_SET_VALUE_COPY) {
        GBL_API_VERIFY_TYPE(pArgs[0].type, GBL_POINTER_TYPE);
        GBL_API_VERIFY_CALL(GblBoxed_free(pVariant->pVoid, GblVariant_type(pVariant)));
        GBL_API_VERIFY_CALL(GblBoxed_copy(pArgs[0].pVoid,
                                          GblVariant_type(pVariant),
                                          &pVariant->pVoid));

    } else if(op & GBL_IVARIANT_OP_FLAG_SET_VALUE_MOVE) {
        GBL_API_VERIFY_TYPE(pArgs[0].type, GBL_POINTER_TYPE);
        GBL_API_VERIFY_CALL(GblBoxed_free(pVariant->pVoid, GblVariant_type(pVariant)));
        pVariant->pVoid = pArgs[0].pVoid;
    }
    GBL_API_END();
}

static GBL_RESULT GblBoxed_get_(GblVariant* pVariant, GblUint argc, GblVariant* pArgs, GBL_IVARIANT_OP_FLAGS op) {
    GBL_API_BEGIN(NULL);
    GBL_API_VERIFY_ARG(argc == 1);
    GBL_API_VERIFY_TYPE(pArgs[0].type, GBL_POINTER_TYPE);

    if(op & GBL_IVARIANT_OP_FLAG_GET_VALUE_COPY) {
        GBL_API_VERIFY_CALL(GblBoxed_copy(pVariant->pVoid,
                                          GblVariant_type(pVariant),
                                          pArgs[0].pVoid));

    } else if(op & GBL_IVARIANT_OP_FLAG_GET_VALUE_MOVE) {
        *((void**)pArgs->pVoid) = pVariant->pVoid;
        GBL_API_CALL(GblVariant_invalidate(pVariant));

    } else if(op & GBL_IVARIANT_OP_FLAG_GET_VALUE_PEEK) {
        *((void**)pArgs->pVoid) = pVariant->pVoid;

    }

    GBL_API_END();
}

static GBL_RESULT GblBoxed_compare_(const GblVariant* pVariant, const GblVariant* pOther, GblInt* pResult) {
    GBL_API_BEGIN(NULL);
    if(pVariant->pVoid > pOther->pVoid)         *pResult = 1;
    else if(pVariant->pVoid < pOther->pVoid)    *pResult = -1;
    else                                        *pResult = 0;
    GBL_API_END();
}

static GBL_RESULT GblBoxed_save_(const GblVariant* pVariant, GblStringBuffer* pString) {
    GBL_API_BEGIN(NULL);
    GBL_API_CALL(GblStringBuffer_appendPointer(pString, pVariant->pVoid));
    GBL_API_END();
}

static GBL_RESULT GblBoxed_load_(GblVariant* pVariant, const GblStringBuffer* pString) {
    GBL_API_BEGIN(NULL);
    pVariant->pVoid = (void*)GblStringView_toPointer(GblStringBuffer_view(pString));
    GBL_API_END();
}

static GBL_RESULT GblBoxed_convertTo_(const GblVariant* pVariant, GblVariant* pOther) {
    GBL_API_BEGIN(NULL);
    const GblType type = GblVariant_type(pOther);
    if(type == GBL_BOOL_TYPE)
        GblVariant_setBool(pOther, pVariant->pVoid? GBL_TRUE : GBL_FALSE);
    else if(type == GBL_STRING_TYPE) {
        char buffer[20];
        snprintf(buffer, sizeof(buffer), "%p", pVariant->pVoid);
        GblVariant_setString(pOther, buffer);
    } else if(type == GBL_POINTER_TYPE) {
        GblVariant_setPointer(pOther, pVariant->pVoid);
    }
    else
        GBL_API_RECORD_SET(GBL_RESULT_ERROR_INVALID_CONVERSION);
    GBL_API_END();
}

static GBL_RESULT GblBoxed_convertFrom_(const GblVariant* pVariant, GblVariant* pOther) {
    GBL_UNUSED(pVariant);
    GBL_API_BEGIN(NULL);
    const GblType type = GblVariant_type(pVariant);
    if(type == GBL_POINTER_TYPE)
        GblVariant_setValueCopy(pOther, GblVariant_type(pOther), pVariant->pVoid);
    else if(type == GBL_NIL_TYPE)
        GblVariant_setBoxedMove(pOther, GblVariant_type(pOther), NULL);
    else
        GBL_API_RECORD_SET(GBL_RESULT_ERROR_INVALID_CONVERSION);
    GBL_API_END();
}

extern GBL_RESULT GblBoxed_typeRegister_(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);

    static const GblIVariantIFaceVTable boxedIVariantIFace =  {
        .supportedOps = GBL_IVARIANT_OP_FLAG_CONSTRUCT_DEFAULT      |
                        GBL_IVARIANT_OP_FLAG_CONSTRUCT_COPY         |
                        GBL_IVARIANT_OP_FLAG_CONSTRUCT_MOVE         |
                        GBL_IVARIANT_OP_FLAG_CONSTRUCT_VALUE_COPY   |
                        GBL_IVARIANT_OP_FLAG_CONSTRUCT_VALUE_MOVE   |
                        GBL_IVARIANT_OP_FLAG_SET_COPY               |
                        GBL_IVARIANT_OP_FLAG_SET_MOVE               |
                        GBL_IVARIANT_OP_FLAG_SET_VALUE_COPY         |
                        GBL_IVARIANT_OP_FLAG_SET_VALUE_MOVE         |
                        GBL_IVARIANT_OP_FLAG_GET_VALUE_COPY         |
                        GBL_IVARIANT_OP_FLAG_GET_VALUE_MOVE         |
                        GBL_IVARIANT_OP_FLAG_GET_VALUE_PEEK,
        .pSetValueFmt   = { "p"},
        .pGetValueFmt   = { "p" },
        .pFnConstruct   = GblBoxed_construct_,
        .pFnDestruct    = GblBoxed_destruct_,
        .pFnSet         = GblBoxed_set_,
        .pFnGet         = GblBoxed_get_,
        .pFnCompare     = GblBoxed_compare_,
        .pFnSave        = GblBoxed_save_,
        .pFnLoad        = GblBoxed_load_
    };

    GblPrimitive_registerBuiltin(GBL_TYPE_BUILTIN_INDEX_BOXED,
                                 GblQuark_internStringStatic("boxed"),
                                 sizeof(GblBoxedClass),
                                 0,
                                 &boxedIVariantIFace,
                                 GBL_TYPE_FUNDAMENTAL_FLAG_DEEP_DERIVABLE |
                                 GBL_TYPE_FLAG_ABSTRACT);
    GBL_API_VERIFY_LAST_RECORD();

    GBL_API_CALL(GblVariant_registerConverter(GBL_BOXED_TYPE, GBL_BOOL_TYPE, GblBoxed_convertTo_));
    GBL_API_CALL(GblVariant_registerConverter(GBL_BOXED_TYPE, GBL_STRING_TYPE, GblBoxed_convertTo_));
    GBL_API_CALL(GblVariant_registerConverter(GBL_BOXED_TYPE, GBL_POINTER_TYPE, GblBoxed_convertTo_));

    GBL_API_CALL(GblVariant_registerConverter(GBL_NIL_TYPE, GBL_BOXED_TYPE, GblBoxed_convertFrom_));
    GBL_API_CALL(GblVariant_registerConverter(GBL_POINTER_TYPE, GBL_BOXED_TYPE, GblBoxed_convertFrom_));

    GBL_API_END();
}

GBL_EXPORT GblType GblBoxed_register(const char*                   pName,
                                     const GblBoxedClassVTable*    pVTable)
{
    GblType type = GBL_INVALID_TYPE;
    GBL_API_BEGIN(NULL);

    type = GblType_registerStatic(GBL_BOXED_TYPE,
                                  GblQuark_internString(pName),
                                  &(const GblTypeInfo) {
                                      .pFnClassInit     = GblBoxedClass_init_,
                                      .classSize        = sizeof(GblBoxedClass),
                                      .pClassData       = pVTable,
                                  },
                                  GBL_TYPE_FLAGS_NONE);
    GBL_API_VERIFY_LAST_RECORD();

    GBL_API_CALL(GblVariant_registerConverter(GBL_NIL_TYPE, type, GblBoxed_convertFrom_));
    GBL_API_CALL(GblVariant_registerConverter(GBL_POINTER_TYPE, type, GblBoxed_convertFrom_));


    GBL_API_END_BLOCK();
    return type;
}

GBL_EXPORT GBL_RESULT GblBoxed_copy(void*      pBoxed,
                                    GblType    type,
                                    void**     ppNewBoxed)
{
    GBL_API_BEGIN(NULL);
    GblBoxedClass* pClass = GBL_BOXED_CLASS(GblClass_peek(type));
    GBL_API_VERIFY_LAST_RECORD();
    GBL_API_VERIFY_CALL(pClass->pVTable->pFnCopy(pBoxed, ppNewBoxed));
    GBL_API_END();
}

GBL_EXPORT GBL_RESULT GblBoxed_free(void*      pBoxed,
                                    GblType    type)
{
    GBL_API_BEGIN(NULL);
    GblBoxedClass* pClass = GBL_BOXED_CLASS(GblClass_peek(type));
    GBL_API_VERIFY_LAST_RECORD();
    GBL_API_VERIFY_CALL(pClass->pVTable->pFnFree(pBoxed));
    GBL_API_END();
}
