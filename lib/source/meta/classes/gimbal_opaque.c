#include <gimbal/meta/classes/gimbal_opaque.h>
#include <gimbal/strings/gimbal_string_buffer.h>
#include <gimbal/meta/types/gimbal_variant.h>
#include "../types/gimbal_type_.h"

static GBL_RESULT GblOpaqueClass_init_(GblClass* pClass, const void* pData) {
    GBL_CTX_BEGIN(NULL);
    GblOpaqueClass* pSelf = GBL_OPAQUE_CLASS(pClass);
    pSelf->pVTable = (const GblOpaqueVTable*)pData;
    GBL_CTX_END();
}

static GBL_RESULT GblOpaque_construct_(GblVariant* pVariant, size_t  argc, GblVariant* pArgs, GBL_IVARIANT_OP_FLAGS op) {
    GBL_CTX_BEGIN(NULL);
    GBL_CTX_VERIFY_ARG(argc == 1 || (op & GBL_IVARIANT_OP_FLAG_CONSTRUCT_DEFAULT));

    // Default constructor
    if(op & GBL_IVARIANT_OP_FLAG_CONSTRUCT_DEFAULT) {
        //GBL_CTX_RECORD_SET(GBL_RESULT_ERROR_INVALID_OPERATION);
        pVariant->pVoid = NULL;

    // Copy constructor
    } else if(op & GBL_IVARIANT_OP_FLAG_CONSTRUCT_COPY) {
        GBL_CTX_VERIFY_TYPE(pArgs[0].type, GblVariant_typeOf(pVariant));
        GBL_CTX_VERIFY_CALL(GblOpaque_copy(pArgs[0].pVoid,
                                          GblVariant_typeOf(&pArgs[0]),
                                          &pVariant->pVoid));

    // Move constructor
    } else if(op & GBL_IVARIANT_OP_FLAG_CONSTRUCT_MOVE) {
        GBL_CTX_VERIFY_TYPE(pArgs[0].type, GblVariant_typeOf(pVariant));
        pVariant->pVoid = pArgs[0].pVoid;
        GBL_CTX_CALL(GblVariant_invalidate(&pArgs[0]));

    // Value copying constructor
    } else if(op & GBL_IVARIANT_OP_FLAG_CONSTRUCT_VALUE_COPY) {
        GBL_CTX_VERIFY_TYPE(pArgs[0].type, GBL_POINTER_TYPE);
        GBL_CTX_VERIFY_CALL(GblOpaque_copy(pArgs[0].pVoid,
                                          GblVariant_typeOf(pVariant),
                                          &pVariant->pVoid));

    // Value moving constructor
    } else if(op & GBL_IVARIANT_OP_FLAG_CONSTRUCT_VALUE_MOVE) {
        GBL_CTX_VERIFY_TYPE(pArgs[0].type, GBL_POINTER_TYPE);
        pVariant->pVoid = pArgs[0].pVoid;
    }

    GBL_CTX_END();
}

static GBL_RESULT GblOpaque_destruct_(GblVariant* pVariant) {
    GBL_CTX_BEGIN(NULL);

    GBL_CTX_VERIFY_CALL(GblOpaque_free(pVariant->pVoid,
                                      GblVariant_typeOf(pVariant)));
    GBL_CTX_END();
}

static GBL_RESULT GblOpaque_set_(GblVariant* pVariant, size_t  argc, GblVariant* pArgs, GBL_IVARIANT_OP_FLAGS op) {
    GBL_CTX_BEGIN(NULL);
    GBL_UNUSED(argc);
    GBL_CTX_VERIFY_ARG(argc == 1);
    if(op & GBL_IVARIANT_OP_FLAG_SET_COPY) {
        GBL_CTX_VERIFY_TYPE(pArgs[0].type, GblVariant_typeOf(pVariant));
        GBL_CTX_VERIFY_CALL(GblOpaque_free(pVariant->pVoid, GblVariant_typeOf(pVariant)));
        GBL_CTX_VERIFY_CALL(GblOpaque_copy(pArgs[0].pVoid,
                                          GblVariant_typeOf(&pArgs[0]),
                                          &pVariant->pVoid));
    }
    else if(op & GBL_IVARIANT_OP_FLAG_SET_MOVE) {
        GBL_CTX_VERIFY_TYPE(pArgs[0].type, GblVariant_typeOf(pVariant));
        GBL_CTX_VERIFY_CALL(GblOpaque_free(pVariant->pVoid, GblVariant_typeOf(pVariant)));
        pVariant->pVoid = pArgs[0].pVoid;
        GBL_CTX_CALL(GblVariant_invalidate(&pArgs[0]));
    }
    else if(op & GBL_IVARIANT_OP_FLAG_SET_VALUE_COPY) {
        GBL_CTX_VERIFY_TYPE(pArgs[0].type, GBL_POINTER_TYPE);
        GBL_CTX_VERIFY_CALL(GblOpaque_free(pVariant->pVoid, GblVariant_typeOf(pVariant)));
        GBL_CTX_VERIFY_CALL(GblOpaque_copy(pArgs[0].pVoid,
                                          GblVariant_typeOf(pVariant),
                                          &pVariant->pVoid));

    } else if(op & GBL_IVARIANT_OP_FLAG_SET_VALUE_MOVE) {
        GBL_CTX_VERIFY_TYPE(pArgs[0].type, GBL_POINTER_TYPE);
        GBL_CTX_VERIFY_CALL(GblOpaque_free(pVariant->pVoid, GblVariant_typeOf(pVariant)));
        pVariant->pVoid = pArgs[0].pVoid;
    }
    GBL_CTX_END();
}

static GBL_RESULT GblOpaque_get_(GblVariant* pVariant, size_t argc, GblVariant* pArgs, GBL_IVARIANT_OP_FLAGS op) {
    GBL_CTX_BEGIN(NULL);
    GBL_CTX_VERIFY_ARG(argc == 1);
    GBL_CTX_VERIFY_TYPE(pArgs[0].type, GBL_POINTER_TYPE);

    if(op & GBL_IVARIANT_OP_FLAG_GET_VALUE_COPY) {
        GBL_CTX_VERIFY_CALL(GblOpaque_copy(pVariant->pVoid,
                                          GblVariant_typeOf(pVariant),
                                          pArgs[0].pVoid));

    } else if(op & GBL_IVARIANT_OP_FLAG_GET_VALUE_MOVE) {
        *((void**)pArgs->pVoid) = pVariant->pVoid;
        GBL_CTX_CALL(GblVariant_invalidate(pVariant));

    } else if(op & GBL_IVARIANT_OP_FLAG_GET_VALUE_PEEK) {
        *((void**)pArgs->pVoid) = pVariant->pVoid;

    }

    GBL_CTX_END();
}

static GBL_RESULT GblOpaque_compare_(const GblVariant* pVariant, const GblVariant* pOther, int* pResult) {
    GBL_CTX_BEGIN(NULL);
    if(pVariant->pVoid > pOther->pVoid)         *pResult = 1;
    else if(pVariant->pVoid < pOther->pVoid)    *pResult = -1;
    else                                        *pResult = 0;
    GBL_CTX_END();
}

static GBL_RESULT GblOpaque_save_(const GblVariant* pVariant, GblStringBuffer* pString) {
    GBL_CTX_BEGIN(NULL);
    GBL_CTX_CALL(GblStringBuffer_appendPointer(pString, pVariant->pVoid));
    GBL_CTX_END();
}

static GBL_RESULT GblOpaque_load_(GblVariant* pVariant, const GblStringBuffer* pString) {
    GBL_CTX_BEGIN(NULL);
    pVariant->pVoid = (void*)GblStringView_toPointer(GblStringBuffer_view(pString));
    GBL_CTX_END();
}

static GBL_RESULT GblOpaque_convertTo_(const GblVariant* pVariant, GblVariant* pOther) {
    GBL_CTX_BEGIN(NULL);
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
        GBL_CTX_RECORD_SET(GBL_RESULT_ERROR_INVALID_CONVERSION);
    GBL_CTX_END();
}

static GBL_RESULT GblOpaque_convertFrom_(const GblVariant* pVariant, GblVariant* pOther) {
    GBL_UNUSED(pVariant);
    GBL_CTX_BEGIN(NULL);
    const GblType type = GblVariant_typeOf(pVariant);
    if(type == GBL_POINTER_TYPE)
        GblVariant_setValueCopy(pOther, GblVariant_typeOf(pOther), pVariant->pVoid);
    else if(type == GBL_NIL_TYPE)
        GblVariant_setOpaqueMove(pOther, GblVariant_typeOf(pOther), NULL);
    else
        GBL_CTX_RECORD_SET(GBL_RESULT_ERROR_INVALID_CONVERSION);
    GBL_CTX_END();
}

static GBL_RESULT GblOpaque_ref_copy_(void* pOpaque, void** ppNewOpaque) {
    *ppNewOpaque = GblRef_ref(pOpaque);

    return GBL_RESULT_SUCCESS;
}

static GBL_RESULT GblOpaque_ref_free_(void* pOpaque) {
    GblRef_unref(pOpaque);

    return GBL_RESULT_SUCCESS;
}

GBL_EXPORT GblType GblOpaque_type(void) {
    static GblType type = GBL_INVALID_TYPE;

    static const GblIVariantVTable opaqueIVariantIFace =  {
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
        GBL_CTX_BEGIN(NULL);

        type = GblPrimitive_register(GblQuark_internStringStatic("GblOpaque"),
                                     sizeof(GblOpaqueClass),
                                     0,
                                     &opaqueIVariantIFace,
                                     GBL_TYPE_FLAG_ABSTRACT);

        GBL_CTX_VERIFY_LAST_RECORD();

        GBL_CTX_CALL(GblVariant_registerConverter(type, GBL_BOOL_TYPE,    GblOpaque_convertTo_));
        GBL_CTX_CALL(GblVariant_registerConverter(type, GBL_STRING_TYPE,  GblOpaque_convertTo_));
        GBL_CTX_CALL(GblVariant_registerConverter(type, GBL_POINTER_TYPE, GblOpaque_convertTo_));

        GBL_CTX_CALL(GblVariant_registerConverter(GBL_NIL_TYPE,     type, GblOpaque_convertFrom_));
        GBL_CTX_CALL(GblVariant_registerConverter(GBL_POINTER_TYPE, type, GblOpaque_convertFrom_));

        GBL_CTX_END_BLOCK();

    }
    return type;
}

GBL_EXPORT GblType GblOpaque_register(const char*            pName,
                                      const GblOpaqueVTable* pVTable)
{
    GblType type = GBL_INVALID_TYPE;
    GBL_CTX_BEGIN(NULL);

    type = GblType_register(GblQuark_internString(pName),
                            GBL_OPAQUE_TYPE,
                            &(const GblTypeInfo) {
                                .pFnClassInit     = GblOpaqueClass_init_,
                                .classSize        = sizeof(GblOpaqueClass),
                                .pClassData       = pVTable,
                            },
                            GBL_TYPE_FLAGS_NONE);
    GBL_CTX_VERIFY_LAST_RECORD();

    GBL_CTX_CALL(GblVariant_registerConverter(GBL_NIL_TYPE, type, GblOpaque_convertFrom_));
    GBL_CTX_CALL(GblVariant_registerConverter(GBL_POINTER_TYPE, type, GblOpaque_convertFrom_));


    GBL_CTX_END_BLOCK();
    return type;
}

GBL_EXPORT GblType GblOpaque_registerRef(const char* pName) {
    const static GblOpaqueVTable vtbl = {
        GblOpaque_ref_copy_,
        GblOpaque_ref_free_
    };

    return GblOpaque_register(pName, &vtbl);
}

GBL_EXPORT GBL_RESULT GblOpaque_copy(void*     pOpaque,
                                     GblType    type,
                                     void**     ppNewOpaque)
{
    GBL_CTX_BEGIN(NULL);
    GblOpaqueClass* pClass = GBL_OPAQUE_CLASS(GblClass_weakRefDefault(type));
    GBL_CTX_VERIFY_LAST_RECORD();
    GBL_CTX_VERIFY_CALL(pClass->pVTable->pFnCopy(pOpaque, ppNewOpaque));
    GBL_CTX_END();
}

GBL_EXPORT GBL_RESULT GblOpaque_free(void*     pOpaque,
                                    GblType    type)
{
    GBL_CTX_BEGIN(NULL);
    GblOpaqueClass* pClass = GBL_OPAQUE_CLASS(GblClass_weakRefDefault(type));
    GBL_CTX_VERIFY_LAST_RECORD();
    GBL_CTX_VERIFY_CALL(pClass->pVTable->pFnFree(pOpaque));
    GBL_CTX_END();
}
