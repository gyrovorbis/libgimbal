#include <gimbal/meta/classes/gimbal_opaque.h>
#include <gimbal/strings/gimbal_string_buffer.h>
#include <gimbal/meta/types/gimbal_variant.h>
#include "../types/gimbal_type_.h"

static GBL_RESULT GblOpaqueClass_init_(GblClass* pClass, const void* pData, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblOpaqueClass* pSelf = GBL_OPAQUE_CLASS(pClass);
    pSelf->pVTable = (const GblOpaqueClassVTable*)pData;
    GBL_API_END();
}

static GBL_RESULT GblOpaque_construct_(GblVariant* pVariant, GblUint argc, GblVariant* pArgs, GBL_IVARIANT_OP_FLAGS op) {
    GBL_API_BEGIN(NULL);
    GBL_API_VERIFY_ARG(argc == 1 || (op & GBL_IVARIANT_OP_FLAG_CONSTRUCT_DEFAULT));

    // Default constructor
    if(op & GBL_IVARIANT_OP_FLAG_CONSTRUCT_DEFAULT) {
        //GBL_API_RECORD_SET(GBL_RESULT_ERROR_INVALID_OPERATION);
        pVariant->pVoid = NULL;

    // Copy constructor
    } else if(op & GBL_IVARIANT_OP_FLAG_CONSTRUCT_COPY) {
        GBL_API_VERIFY_TYPE(pArgs[0].type, GblVariant_typeOf(pVariant));
        GBL_API_VERIFY_CALL(GblOpaque_copy(pArgs[0].pVoid,
                                          GblVariant_typeOf(&pArgs[0]),
                                          &pVariant->pVoid));

    // Move constructor
    } else if(op & GBL_IVARIANT_OP_FLAG_CONSTRUCT_MOVE) {
        GBL_API_VERIFY_TYPE(pArgs[0].type, GblVariant_typeOf(pVariant));
        pVariant->pVoid = pArgs[0].pVoid;
        GBL_API_CALL(GblVariant_invalidate(&pArgs[0]));

    // Value copying constructor
    } else if(op & GBL_IVARIANT_OP_FLAG_CONSTRUCT_VALUE_COPY) {
        GBL_API_VERIFY_TYPE(pArgs[0].type, GBL_POINTER_TYPE);
        GBL_API_VERIFY_CALL(GblOpaque_copy(pArgs[0].pVoid,
                                          GblVariant_typeOf(pVariant),
                                          &pVariant->pVoid));

    // Value moving constructor
    } else if(op & GBL_IVARIANT_OP_FLAG_CONSTRUCT_VALUE_MOVE) {
        GBL_API_VERIFY_TYPE(pArgs[0].type, GBL_POINTER_TYPE);
        pVariant->pVoid = pArgs[0].pVoid;
    }

    GBL_API_END();
}

static GBL_RESULT GblOpaque_destruct_(GblVariant* pVariant) {
    GBL_API_BEGIN(NULL);

    GBL_API_VERIFY_CALL(GblOpaque_free(pVariant->pVoid,
                                      GblVariant_typeOf(pVariant)));
    GBL_API_END();
}

static GBL_RESULT GblOpaque_set_(GblVariant* pVariant, GblUint argc, GblVariant* pArgs, GBL_IVARIANT_OP_FLAGS op) {
    GBL_API_BEGIN(NULL);
    GBL_UNUSED(argc);
    GBL_API_VERIFY_ARG(argc == 1);
    if(op & GBL_IVARIANT_OP_FLAG_SET_COPY) {
        GBL_API_VERIFY_TYPE(pArgs[0].type, GblVariant_typeOf(pVariant));
        GBL_API_VERIFY_CALL(GblOpaque_free(pVariant->pVoid, GblVariant_typeOf(pVariant)));
        GBL_API_VERIFY_CALL(GblOpaque_copy(pArgs[0].pVoid,
                                          GblVariant_typeOf(&pArgs[0]),
                                          &pVariant->pVoid));
    }
    else if(op & GBL_IVARIANT_OP_FLAG_SET_MOVE) {
        GBL_API_VERIFY_TYPE(pArgs[0].type, GblVariant_typeOf(pVariant));
        GBL_API_VERIFY_CALL(GblOpaque_free(pVariant->pVoid, GblVariant_typeOf(pVariant)));
        pVariant->pVoid = pArgs[0].pVoid;
        GBL_API_CALL(GblVariant_invalidate(&pArgs[0]));
    }
    else if(op & GBL_IVARIANT_OP_FLAG_SET_VALUE_COPY) {
        GBL_API_VERIFY_TYPE(pArgs[0].type, GBL_POINTER_TYPE);
        GBL_API_VERIFY_CALL(GblOpaque_free(pVariant->pVoid, GblVariant_typeOf(pVariant)));
        GBL_API_VERIFY_CALL(GblOpaque_copy(pArgs[0].pVoid,
                                          GblVariant_typeOf(pVariant),
                                          &pVariant->pVoid));

    } else if(op & GBL_IVARIANT_OP_FLAG_SET_VALUE_MOVE) {
        GBL_API_VERIFY_TYPE(pArgs[0].type, GBL_POINTER_TYPE);
        GBL_API_VERIFY_CALL(GblOpaque_free(pVariant->pVoid, GblVariant_typeOf(pVariant)));
        pVariant->pVoid = pArgs[0].pVoid;
    }
    GBL_API_END();
}

static GBL_RESULT GblOpaque_get_(GblVariant* pVariant, GblUint argc, GblVariant* pArgs, GBL_IVARIANT_OP_FLAGS op) {
    GBL_API_BEGIN(NULL);
    GBL_API_VERIFY_ARG(argc == 1);
    GBL_API_VERIFY_TYPE(pArgs[0].type, GBL_POINTER_TYPE);

    if(op & GBL_IVARIANT_OP_FLAG_GET_VALUE_COPY) {
        GBL_API_VERIFY_CALL(GblOpaque_copy(pVariant->pVoid,
                                          GblVariant_typeOf(pVariant),
                                          pArgs[0].pVoid));

    } else if(op & GBL_IVARIANT_OP_FLAG_GET_VALUE_MOVE) {
        *((void**)pArgs->pVoid) = pVariant->pVoid;
        GBL_API_CALL(GblVariant_invalidate(pVariant));

    } else if(op & GBL_IVARIANT_OP_FLAG_GET_VALUE_PEEK) {
        *((void**)pArgs->pVoid) = pVariant->pVoid;

    }

    GBL_API_END();
}

static GBL_RESULT GblOpaque_compare_(const GblVariant* pVariant, const GblVariant* pOther, GblInt* pResult) {
    GBL_API_BEGIN(NULL);
    if(pVariant->pVoid > pOther->pVoid)         *pResult = 1;
    else if(pVariant->pVoid < pOther->pVoid)    *pResult = -1;
    else                                        *pResult = 0;
    GBL_API_END();
}

static GBL_RESULT GblOpaque_save_(const GblVariant* pVariant, GblStringBuffer* pString) {
    GBL_API_BEGIN(NULL);
    GBL_API_CALL(GblStringBuffer_appendPointer(pString, pVariant->pVoid));
    GBL_API_END();
}

static GBL_RESULT GblOpaque_load_(GblVariant* pVariant, const GblStringBuffer* pString) {
    GBL_API_BEGIN(NULL);
    pVariant->pVoid = (void*)GblStringView_toPointer(GblStringBuffer_view(pString));
    GBL_API_END();
}

static GBL_RESULT GblOpaque_convertTo_(const GblVariant* pVariant, GblVariant* pOther) {
    GBL_API_BEGIN(NULL);
    const GblType type = GblVariant_typeOf(pOther);
    if(type == GBL_BOOL_TYPE)
        GblVariant_setBool(pOther, pVariant->pVoid? GBL_TRUE : GBL_FALSE);
    else if(type == GBL_STRING_TYPE) {
        char buffer[20];
        snprintf(buffer, sizeof(buffer), "%p", pVariant->pVoid);
        GblVariant_setString(pOther, buffer);
    } else if(type == GBL_POINTER_TYPE) {
        GblVariant_setPointer(pOther, GBL_POINTER_TYPE, pVariant->pVoid);
    }
    else
        GBL_API_RECORD_SET(GBL_RESULT_ERROR_INVALID_CONVERSION);
    GBL_API_END();
}

static GBL_RESULT GblOpaque_convertFrom_(const GblVariant* pVariant, GblVariant* pOther) {
    GBL_UNUSED(pVariant);
    GBL_API_BEGIN(NULL);
    const GblType type = GblVariant_typeOf(pVariant);
    if(type == GBL_POINTER_TYPE)
        GblVariant_setValueCopy(pOther, GblVariant_typeOf(pOther), pVariant->pVoid);
    else if(type == GBL_NIL_TYPE)
        GblVariant_setOpaqueMove(pOther, GblVariant_typeOf(pOther), NULL);
    else
        GBL_API_RECORD_SET(GBL_RESULT_ERROR_INVALID_CONVERSION);
    GBL_API_END();
}

GBL_EXPORT GblType GblOpaque_type(void) {
    static GblType type = GBL_INVALID_TYPE;

    static const GblIVariantIFaceVTable opaqueIVariantIFace =  {
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
        .pFnConstruct   = GblOpaque_construct_,
        .pFnDestruct    = GblOpaque_destruct_,
        .pFnSet         = GblOpaque_set_,
        .pFnGet         = GblOpaque_get_,
        .pFnCompare     = GblOpaque_compare_,
        .pFnSave        = GblOpaque_save_,
        .pFnLoad        = GblOpaque_load_
    };


    if(type == GBL_INVALID_TYPE) {
        GBL_API_BEGIN(NULL);

        type = GblPrimitive_register(GblQuark_internStringStatic("opaque"),
                                     sizeof(GblOpaqueClass),
                                     0,
                                     &opaqueIVariantIFace,
                                     GBL_TYPE_FLAG_ABSTRACT);

        GBL_API_VERIFY_LAST_RECORD();

        GBL_API_CALL(GblVariant_registerConverter(type, GBL_BOOL_TYPE,       GblOpaque_convertTo_));
        GBL_API_CALL(GblVariant_registerConverter(type, GBL_STRING_TYPE,     GblOpaque_convertTo_));
        GBL_API_CALL(GblVariant_registerConverter(type, GBL_POINTER_TYPE,    GblOpaque_convertTo_));

        GBL_API_CALL(GblVariant_registerConverter(GBL_NIL_TYPE,     type,    GblOpaque_convertFrom_));
        GBL_API_CALL(GblVariant_registerConverter(GBL_POINTER_TYPE, type,    GblOpaque_convertFrom_));

        GBL_API_END_BLOCK();

    }
    return type;
}

GBL_EXPORT GblType GblOpaque_register(const char*                   pName,
                                      const GblOpaqueClassVTable*   pVTable)
{
    GblType type = GBL_INVALID_TYPE;
    GBL_API_BEGIN(NULL);

    type = GblType_registerStatic(GblQuark_internString(pName),
                                  GBL_OPAQUE_TYPE,
                                  &(const GblTypeInfo) {
                                      .pFnClassInit     = GblOpaqueClass_init_,
                                      .classSize        = sizeof(GblOpaqueClass),
                                      .pClassData       = pVTable,
                                  },
                                  GBL_TYPE_FLAGS_NONE);
    GBL_API_VERIFY_LAST_RECORD();

    GBL_API_CALL(GblVariant_registerConverter(GBL_NIL_TYPE, type, GblOpaque_convertFrom_));
    GBL_API_CALL(GblVariant_registerConverter(GBL_POINTER_TYPE, type, GblOpaque_convertFrom_));


    GBL_API_END_BLOCK();
    return type;
}

GBL_EXPORT GBL_RESULT GblOpaque_copy(void*     pOpaque,
                                    GblType    type,
                                    void**     ppNewOpaque)
{
    GBL_API_BEGIN(NULL);
    GblOpaqueClass* pClass = GBL_OPAQUE_CLASS(GblClass_weakRefDefault(type));
    GBL_API_VERIFY_LAST_RECORD();
    GBL_API_VERIFY_CALL(pClass->pVTable->pFnCopy(pOpaque, ppNewOpaque));
    GBL_API_END();
}

GBL_EXPORT GBL_RESULT GblOpaque_free(void*     pOpaque,
                                    GblType    type)
{
    GBL_API_BEGIN(NULL);
    GblOpaqueClass* pClass = GBL_OPAQUE_CLASS(GblClass_weakRefDefault(type));
    GBL_API_VERIFY_LAST_RECORD();
    GBL_API_VERIFY_CALL(pClass->pVTable->pFnFree(pOpaque));
    GBL_API_END();
}
