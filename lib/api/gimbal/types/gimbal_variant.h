#ifndef GIMBAL_VARIANT__H
#define GIMBAL_VARIANT__H

#include "../ifaces/gimbal_ivariant.h"
#include "../meta/gimbal_variant.h"
#include "../types/gimbal_typedefs.h"
#include <stdint.h>

#define SELF    GblVariant* pSelf
#define CSELF   const SELF

GBL_DECLS_BEGIN


#define GBL_META_VARIANT_OP_CMP_TYPE_TABLE (                                                        \
        ( GBL_VARIANT_OP_CMP_TYPE, VariantOpCmpType, "Comparison operator type for Variants", gblVariantOpCmpTypeString),      \
        (                                                                                           \
            (GBL_VARIANT_OP_CMP_EQUAL,      0x0,    Equal,          "Equal"),                       \
            (GBL_VARIANT_OP_CMP_NEQUAL,     0x1,    NotEqual,       "Not Equal"),                   \
            (GBL_VARIANT_OP_CMP_LESS,       0x2,    Less,           "Less Than"),                   \
            (GBL_VARIANT_OP_CMP_LEQUAL,     0x3,    LessOrEqual,    "Less Than Or Equal"),          \
            (GBL_VARIANT_OP_CMP_GREATER,    0x4,    Greater,        "Greater Than"),                \
            (GBL_VARIANT_OP_CMP_GEQUAL,     0x5,    GreaterOrEqual, "Greater Than or Equal"),       \
            (GBL_VARIANT_OP_CMP_COUNT,      0x6,    BuiltinCount,   "Builtin comparison opCount")   \
        )                                                                                           \
    )

GBL_ENUM_TABLE_DECLARE(GBL_META_VARIANT_OP_CMP_TYPE_TABLE);


#define GBL_VARIANT_CONSTRUCT_TRAITS (                           \
        GBL_META_GENERIC_MACRO_NO_DEFAULT,                       \
        (                                                        \
            (GblBool,                gblVariantConstructb),      \
            (GblInt,                 gblVariantConstructi),      \
            (GblUint,                gblVariantConstructu),      \
            (GblFloat,               gblVariantConstructf),      \
            (void*,                  gblVariantConstructp),      \
            (const GblStringBuffer*, gblVariantConstructs),      \
            (GblStringBuffer*,       gblVariantConstructs),      \
            (const char*,            gblVariantConstructc),      \
            (char*,                  gblVariantConstructc),      \
            (const GblVariant*,      gblVariantConstructCopy)    \
        )                                                        \
    )
#define gblVariantConstruct_N(pVariant, ...) GBL_META_GENERIC_MACRO_GENERATE(GBL_VARIANT_CONSTRUCT_TRAITS, GBL_ARG_1(__VA_ARGS__))(pVariant, __VA_ARGS__)
#define gblVariantConstruct_1(pVariant)     gblVariantConstructDefault(pVariant)
#define gblVariantConstruct(...)            GBL_VA_OVERLOAD_CALL(gblVariantConstruct, GBL_VA_OVERLOAD_SUFFIXER_1_N, __VA_ARGS__)

#define gblVariantSetc(...) GBL_VA_OVERLOAD_CALL(gblVariantSetc, GBL_VA_OVERLOAD_SUFFIXER_ARGC, __VA_ARGS__)

#define GBL_VARIANT_SET_TRAITS (                    \
        GBL_META_GENERIC_MACRO_NO_DEFAULT,          \
        (                                           \
            (GblBool,           gblVariantSetb),    \
            (GblInt,            gblVariantSeti),    \
            (GblFloat,          gblVariantSetf),    \
            (void*,             gblVariantSetp),    \
            (const GblStringBuffer*,  gblVariantSets),    \
            (GblStringBuffer*,        gblVariantSets),    \
            (const char*,       gblVariantSetc_2),  \
            (char*,             gblVariantSetc_2)   \
        )                                           \
    )
#define gblVariantSet(pVariant, ...) GBL_META_GENERIC_MACRO_GENERATE(GBL_VARIANT_SET_TRAITS, GBL_ARG_1(__VA_ARGS__))(pVariant, __VA_ARGS__)


#define GBL_VARIANT_MOVE_TRAITS (                    \
        GBL_META_GENERIC_MACRO_NO_DEFAULT,        \
        (                                           \
            (GblVariant*,       gblVariantMovev),    \
            (GblStringBuffer*,        gblVariantMoves)    \
        )                                           \
    )
#define gblVariantMove(pVariant, ...) GBL_META_GENERIC_MACRO_GENERATE(GBL_VARIANT_MOVE_TRAITS, GBL_ARG_1(__VA_ARGS__))(pVariant, __VA_ARGS__)



#define GBL_VARIANT_GET_TRAITS (                    \
        GBL_META_GENERIC_MACRO_NO_DEFAULT,          \
        (                                           \
            (GblBool*,          gblVariantGetb),    \
            (GblInt*,           gblVariantGeti),    \
            (GblFloat*,         gblVariantGetf),    \
            (void**,            gblVariantGetp),    \
            (const GblStringBuffer**, gblVariantGets),    \
            (const char**,      gblVariantGetc)     \
        )                                           \
    )
#define gblVariantGet(pVariant, pValue) GBL_META_GENERIC_MACRO_GENERATE(GBL_VARIANT_GET_TRAITS, pValue)(pVariant, pValue)

#define GBL_VARIANT_TO_TRAITS (                 \
        GBL_META_GENERIC_MACRO_NO_DEFAULT,      \
        (                                       \
            (GblBool*,      gblVariantTob),     \
            (GblInt*,       gblVariantToi),     \
            (GblFloat*,     gblVariantTof),     \
            (void**,        gblVariantTop),     \
            (GblStringBuffer*,    gblVariantTos),     \
            (const char**,  gblVaraintToc)      \
        )                                       \
    )

#define GBL_VARIANT_COMPARE_TRAITS (                    \
        GBL_META_GENERIC_MACRO_NO_DEFAULT,              \
        (                                               \
            (GblBool,           gblVariantCompareb),    \
            (GblInt,            gblVariantComparei),    \
            (GblFloat,          gblVariantComparef),    \
            (const void*,       gblVariantComparep),    \
            (void*,             gblVariantComparep),    \
            (const GblStringBuffer*,  gblVariantCompares),    \
            (GblStringBuffer*,        gblVariantCompares),    \
            (const GblVariant*, gblVariantComparev),    \
            (GblVariant*,       gblVariantComparev),    \
            (const char*,       gblVariantComparec),    \
            (char*,             gblVariantComparec)     \
        )                                               \
    )



#define gblVariantTo(pVariant, value) GBL_META_GENERIC_MACRO_GENERATE(GBL_VARIANT_TO_TRAITS, value)(pVariant, value)
#define gblVariantCompare(pVariant, rhs, op, result) GBL_META_GENERIC_MACRO_GENERATE(GBL_VARIANT_COMPARE_TRAITS, rhs)(pVariant, rhs, op, result)

#if 0
GBL_INLINE GBL_API gblVariantTypeSet(GblVariant* pVariant, GblType type) {
    GBL_API_BEGIN(GBL_NULL);
    // FREE/RELEASE SHIT IF REFERENCE TYPE
    if(pVariant->type && pVariant->type != type) {
        GblVariant_destruct(pVariant);
    }
    pVariant->type = type;
    GBL_API_END();
}
#endif

// Top-level Utilities
GBL_INLINE GBL_API gblVariantTypeGet(const GblVariant* pVariant, GblType* pType) {
    GBL_API_BEGIN(GBL_NULL);
    GBL_API_VERIFY_POINTER(pType);
    *pType = pVariant->type;
    GBL_API_END();
}


// Get (Actual) Value
GBL_INLINE GBL_API gblVariantGetb(const GblVariant* pVariant, GblBool* pValue) {
    GBL_API_BEGIN(GBL_NULL);
    GBL_API_VERIFY_POINTER(pVariant);
    GBL_API_VERIFY_POINTER(pValue);
    GBL_API_VERIFY_TYPE(pVariant->type, GBL_BOOL_TYPE);
    GBL_API_CALL(GblVariant_getValueCopy(pVariant, pValue));
    GBL_API_END();
}

GBL_INLINE GBL_API gblVariantGeti(const GblVariant* pVariant,    GblInt*      pValue) {
    GBL_API_BEGIN(GBL_NULL);
    GBL_API_VERIFY_POINTER(pVariant);
    GBL_API_VERIFY_POINTER(pValue);
    GBL_API_VERIFY_TYPE(pVariant->type, GBL_INT32_TYPE);
    GBL_API_CALL(GblVariant_getValueCopy(pVariant, pValue));
    GBL_API_END();
}

GBL_INLINE GBL_API gblVariantGetf(const GblVariant* pVariant,    GblFloat*    pValue) {
    GBL_API_BEGIN(GBL_NULL);
    GBL_API_VERIFY_POINTER(pVariant);
    GBL_API_VERIFY_POINTER(pValue);
    GBL_API_VERIFY_TYPE(pVariant->type, GBL_FLOAT_TYPE);
    GBL_API_CALL(GblVariant_getValueCopy(pVariant, pValue));
    GBL_API_END();
}

GBL_INLINE GBL_API gblVariantGetp(const GblVariant* pVariant,    void**     pValue) {
    GBL_API_BEGIN(GBL_NULL);
    GBL_API_VERIFY_POINTER(pVariant);
    GBL_API_VERIFY_POINTER(pValue);
    GBL_API_VERIFY_TYPE(pVariant->type, GBL_POINTER_TYPE);
    GBL_API_CALL(GblVariant_getValueCopy(pVariant, pValue));
    GBL_API_END();
}

GBL_INLINE GBL_API gblVariantGets(const GblVariant* pVariant,  const GblStringBuffer** pString) {
    GBL_API_BEGIN(GBL_NULL);
    GBL_API_VERIFY_POINTER(pVariant);
    GBL_API_VERIFY_POINTER(pString);
    GBL_API_VERIFY_TYPE(pVariant->type, GBL_STRING_TYPE);
    *pString = &pVariant->string;
    GBL_API_END();
}

GBL_INLINE GBL_API gblVariantGetc(const GblVariant* pVariant, const char** ppStr) {
    GBL_API_BEGIN(GBL_NULL);
    GBL_API_VERIFY_TYPE(pVariant->type, GBL_STRING_TYPE);
    GBL_API_CALL(GblVariant_getValuePeek(pVariant, ppStr));
    GBL_API_END();
}

// Set Value
GBL_INLINE GBL_API gblVariantSetNil(GblVariant* pVariant) {
    GBL_API_BEGIN(GBL_NULL);
    GBL_API_VERIFY_POINTER(pVariant);
    GBL_API_CALL(GblVariant_setValueCopy(pVariant, GBL_NIL_TYPE));
    //GBL_API_CALL(gblVariantTypeSet(pVariant, GBL_NIL_TYPE));
    GBL_API_END();
}

GBL_INLINE GBL_API gblVariantSetb(GblVariant* pVariant,    GblBool     value) {
    GBL_API_BEGIN(GBL_NULL);
    GBL_API_VERIFY_POINTER(pVariant);
    GBL_API_CALL(GblVariant_setValueCopy(pVariant, GBL_BOOL_TYPE, value));
    //GBL_API_CALL(gblVariantTypeSet(pVariant, GBL_BOOL_TYPE));
    //pVariant->boolean = value;
    GBL_API_END();
}

GBL_INLINE GBL_API gblVariantSeti(GblVariant* pVariant,    GblInt      value) {
    GBL_API_BEGIN(GBL_NULL);
    GBL_API_VERIFY_POINTER(pVariant);
    GBL_API_CALL(GblVariant_setValueCopy(pVariant, GBL_INT32_TYPE, value));
    //GBL_API_CALL(gblVariantTypeSet(pVariant, GBL_INT32_TYPE));
    //pVariant->i32 = value;
    GBL_API_END();
}

GBL_INLINE GBL_API gblVariantSetf(GblVariant* pVariant,    GblFloat       value) {
    GBL_API_BEGIN(GBL_NULL);
    GBL_API_VERIFY_POINTER(pVariant);
    GBL_API_CALL(GblVariant_setValueCopy(pVariant, GBL_FLOAT_TYPE, value));
//    GBL_API_CALL(gblVariantTypeSet(pVariant, GBL_FLOAT_TYPE));
//    pVariant->f32 = value;
    GBL_API_END();
}

GBL_INLINE GBL_API gblVariantSetp(GblVariant* pVariant,    void*       pValue) {
    GBL_API_BEGIN(GBL_NULL);
    GBL_API_VERIFY_POINTER(pVariant);
    GBL_API_CALL(GblVariant_setValueCopy(pVariant, GBL_POINTER_TYPE, pValue));
    //GBL_API_CALL(gblVariantTypeSet(pVariant, GBL_POINTER_TYPE));
    //pVariant->pVoid = pValue;
    GBL_API_END();
}

GBL_INLINE GBL_API gblVariantSets(GblVariant* pVariant, const GblStringBuffer* pString) {
    GBL_API_BEGIN(GBL_NULL);
    GBL_API_VERIFY_POINTER(pVariant);
    GBL_API_VERIFY_POINTER(pString);


    GBL_API_CALL(GblVariant_setValueCopy(pVariant,
                                         GBL_STRING_TYPE,
                                         GblStringBuffer_cString(pString),
                                         GblStringBuffer_length(pString),
                                         GblStringBuffer_context(pString)));
#if 0
    view.pBuffer = pString->data.pBuffer; view.length = pString->data.size;
    if(pVariant->type != GBL_STRING_TYPE) {
        GBL_API_CALL(gblVariantTypeSet(pVariant, GBL_STRING_TYPE));
        GBL_API_CALL(GblStringBuffer_construct(&pVariant->string, sizeof(GblStringBuffer), pString->data.hCtx, &view));
    } else {
        GBL_API_CALL(GblStringBuffer_assign(&pVariant->string, &view));
    }
#endif
    GBL_API_END();
}


GBL_INLINE GBL_API gblVariantSetc_3(GblVariant* pVariant, const char* pString, GblContext* pCtx) {
    GBL_API_BEGIN(GBL_NULL);
    GblStringView view;
    GBL_API_VERIFY_POINTER(pVariant);

    GBL_API_CALL(GblVariant_setValueCopy(pVariant, GBL_STRING_TYPE, pString, 0, pCtx));
    /*
    view.pBuffer = pString; view.length = 0;
    if(pVariant->type != GBL_STRING_TYPE) {
        GBL_API_CALL(gblVariantTypeSet(pVariant, GBL_STRING_TYPE));
        GBL_API_CALL(GblStringBuffer_construct(&pVariant->string, sizeof(GblStringBuffer), hCtx, &view));
    } else {
        GBL_API_CALL(GblStringBuffer_assign(&pVariant->string, &view));
    }*/
    GBL_API_END();
}

GBL_INLINE GBL_API gblVariantSetc_2(GblVariant* pVariant, const char* pString) {
    return gblVariantSetc_3(pVariant, pString, GBL_NULL);
}


// (MAYBE) Convert Value

// Get (Actual) Value
GBL_INLINE GBL_API gblVariantTob(const GblVariant* pVariant, GblBool* pValue) {
    GBL_API_BEGIN(GBL_NULL);
    GBL_API_VERIFY_POINTER(pVariant);
    GBL_API_VERIFY_POINTER(pValue);
    {
        const GblType type = GblVariant_type(pVariant);
        if(type == GBL_BOOL_TYPE)           *pValue = pVariant->boolean;
        else if(type == GBL_INT32_TYPE)     *pValue = pVariant->i32? GBL_TRUE : GBL_FALSE;
        else if(type == GBL_FLOAT_TYPE)     *pValue = pVariant->f32 != 0.0f? GBL_TRUE : GBL_FALSE;
        else if(type == GBL_POINTER_TYPE)   *pValue = pVariant->pVoid? GBL_TRUE : GBL_FALSE;
        else if(type == GBL_STRING_TYPE)    *pValue = GblStringBuffer_toBool(&pVariant->string);
        else *pValue = GBL_FALSE;
    }
    GBL_API_END();
}

GBL_INLINE GBL_API gblVariantToi(const GblVariant* pVariant,    GblInt*      pValue) {
    GBL_API_BEGIN(GBL_NULL);
    GBL_API_VERIFY_POINTER(pVariant);
    GBL_API_VERIFY_POINTER(pValue);
    {
        const GblType type = GblVariant_type(pVariant);
        if(type == GBL_BOOL_TYPE)           *pValue = pVariant->boolean? 1 : 0;
        else if(type == GBL_INT32_TYPE)     *pValue = pVariant->i32;
        else if(type == GBL_FLOAT_TYPE)     *pValue = (GblInt)pVariant->f32;
        else if(type == GBL_POINTER_TYPE)   *pValue = (uintptr_t)pVariant->pVoid;
        else if(type ==GBL_STRING_TYPE)     *pValue = GblStringBuffer_toInt(&pVariant->string);
        else *pValue = 0;

    }
    GBL_API_END();
}

GBL_INLINE GBL_API gblVariantTof(const GblVariant* pVariant,    GblFloat*    pValue) {
    GBL_API_BEGIN(GBL_NULL);
    GBL_API_VERIFY_POINTER(pVariant);
    GBL_API_VERIFY_POINTER(pValue);
    {
        const GblType type = GblVariant_type(pVariant);
        if(type == GBL_INT32_TYPE)          *pValue = (GblFloat)pVariant->i32;
        else if(type == GBL_FLOAT_TYPE)    *pValue = pVariant->f32;
        else if(type == GBL_STRING_TYPE)   *pValue = GblStringBuffer_toFloat(&pVariant->string);
        else {
            *pValue = 0.0f;
            GBL_API_VERIFY(GBL_FALSE, GBL_RESULT_ERROR_TYPE_MISMATCH, "Cannot convert type: %u to float!", pVariant->type);
        }
    }
    GBL_API_END();
}

GBL_INLINE GBL_API gblVariantTop(const GblVariant* pVariant,    void**       pValue) {
    GBL_API_BEGIN(GBL_NULL);
    GBL_API_VERIFY_POINTER(pVariant);
    GBL_API_VERIFY_POINTER(pValue);
    {
        const GblType type = GblVariant_type(pVariant);
        if(type == GBL_POINTER_TYPE) *pValue = pVariant->pVoid;
        else {
            *pValue = 0;
            GBL_API_VERIFY(GBL_FALSE, GBL_RESULT_ERROR_TYPE_MISMATCH, "Cannot convert type: %u to void*!", pVariant->type);
        }
    }
    GBL_API_END();
}

GBL_INLINE GBL_API gblVariantToc(const GblVariant* pVariant, const char** ppCString) {
    GBL_API_BEGIN(GBL_NULL);
    GBL_API_VERIFY_POINTER(pVariant);
    GBL_API_VERIFY_POINTER(ppCString);
    {
        const GblType type = GblVariant_type(pVariant);
        if(type == GBL_STRING_TYPE)
            GBL_API_CALL(gblVariantGetc(pVariant, ppCString));
        else {
            *ppCString = NULL;
            GBL_API_RECORD_SET(GBL_RESULT_ERROR_TYPE_MISMATCH, "Cannot convert type: %u to const char*!", pVariant->type);
        }
    }
    GBL_API_END();
}


// BETTER ALREADY BE CONSTRUCTED, SO THAT IT ALREADY HAS A FUCKING CONTEXT ASSOCIATED WITH IT!
GBL_INLINE GBL_API gblVariantTos(const GblVariant* pVariant, GblStringBuffer* pString) {
    GBL_API_BEGIN(GBL_NULL);
    GblStringView view;
    GBL_API_VERIFY_POINTER(pVariant);
    GBL_API_VERIFY_POINTER(pString);

    if(pVariant->type == GBL_STRING_TYPE) {
        view.pData = (char*)pVariant->string.data.pData; view.length = pVariant->string.data.size;
        GBL_API_CALL(GblStringBuffer_construct(pString, view, sizeof(GblStringBuffer)));
    }  else {

        GBL_API_CALL(GblStringBuffer_construct(pString));
        const GblType type = GblVariant_type(pVariant);
        if(type == GBL_NIL_TYPE)
            GBL_API_CALL(GblStringBuffer_appendNil(pString));
        else if(type == GBL_BOOL_TYPE)
            GBL_API_CALL(GblStringBuffer_appendBool(pString, pVariant->boolean));
        else if(type == GBL_INT32_TYPE)
            GBL_API_CALL(GblStringBuffer_appendInt(pString, pVariant->i32));
        else if(type == GBL_UINT32_TYPE)
            GBL_API_CALL(GblStringBuffer_appendUint(pString, pVariant->u32));
        else if(type == GBL_FLOAT_TYPE)
            GBL_API_CALL(GblStringBuffer_appendFloat(pString, pVariant->f32));
        else if(type == GBL_POINTER_TYPE)
            GBL_API_CALL(GblStringBuffer_appendPointer(pString, pVariant->pVoid));
        else
            GBL_API_VERIFY(GBL_FALSE, GBL_RESULT_ERROR_TYPE_MISMATCH, "Attempted to convert unknown variant type %u to string!", pVariant->type);
        }

    GBL_API_END();
}

#define GBL_VARIANT_COMPARE_INTEGRAL_DEFINE(suffix, type, initialValue) \
GBL_INLINE GBL_API gblVariantCompare##suffix(const GblVariant* pVariant, type rhs, GBL_VARIANT_OP_CMP_TYPE op, GblBool* pResult) {  \
    type val = initialValue;    \
    GBL_API_BEGIN(GBL_NULL);   \
    GBL_API_VERIFY_POINTER(pVariant);   \
    GBL_API_VERIFY_ARG(op < GBL_VARIANT_OP_CMP_COUNT);  \
    GBL_API_VERIFY_POINTER(pResult);    \
    GBL_API_CALL(gblVariantTo##suffix(pVariant, &val)); \
    switch(op) {    \
    case GBL_VARIANT_OP_CMP_EQUAL:      *pResult = (val == rhs); break; \
    case GBL_VARIANT_OP_CMP_NEQUAL:     *pResult = (val != rhs); break; \
    case GBL_VARIANT_OP_CMP_LESS:       *pResult = (val <  rhs); break; \
    case GBL_VARIANT_OP_CMP_LEQUAL:     *pResult = (val <= rhs); break; \
    case GBL_VARIANT_OP_CMP_GREATER:    *pResult = (val >  rhs); break;\
    case GBL_VARIANT_OP_CMP_GEQUAL:     *pResult = (val >= rhs); break; \
    }   \
    GBL_API_END();  \
}

GBL_VARIANT_COMPARE_INTEGRAL_DEFINE(b, GblBool, GBL_FALSE)
GBL_VARIANT_COMPARE_INTEGRAL_DEFINE(i, GblInt, 0)
GBL_VARIANT_COMPARE_INTEGRAL_DEFINE(f, GblFloat, 0.0f)
GBL_VARIANT_COMPARE_INTEGRAL_DEFINE(p, void*, NULL)

GBL_INLINE GBL_API gblVariantCompares(const GblVariant* pVariant, const GblStringBuffer* pString, GBL_VARIANT_OP_CMP_TYPE op, GblBool* pResult) {
    GblStringBuffer tempString;
    const GblStringBuffer* pThisString = NULL;
    GBL_API_BEGIN(GBL_NULL);
    GBL_API_VERIFY_POINTER(pVariant);
    GBL_API_VERIFY_POINTER(pString);
    GBL_API_VERIFY_ARG(op < GBL_VARIANT_OP_CMP_COUNT);
    GBL_API_VERIFY_POINTER(pResult);
    if(pVariant->type == GBL_STRING_TYPE) {
        pThisString = &pVariant->string;
    } else {
        GBL_API_CALL(gblVariantTos(pVariant, &tempString));
        pThisString = &tempString;
    }
    switch(op) {
    case GBL_VARIANT_OP_CMP_EQUAL: {
        GblInt result = GblStringBuffer_compare(pThisString, pString);
        *pResult = result == 0;
        break;
    }
    case GBL_VARIANT_OP_CMP_NEQUAL: {
        GblInt result = GblStringBuffer_compare(pThisString, pString);
        *pResult = result != 0;
        break;
    }
    default: {
        const char* pBuffer1 = GblStringBuffer_cString(pThisString);
        const char* pBuffer2 = GblStringBuffer_cString(pString);
        GblSize     strLen1 = GblStringBuffer_length(pThisString);
        GblSize     strLen2 = GblStringBuffer_length(pString);
        const int   cmpValue = strncmp(pBuffer1, pBuffer2, strLen1 < strLen2? strLen1 : strLen2);
        switch(op) {
        case GBL_VARIANT_OP_CMP_LESS:       *pResult = (cmpValue < 0 ); break;
        case GBL_VARIANT_OP_CMP_LEQUAL:     *pResult = (cmpValue <= 0); break;
        case GBL_VARIANT_OP_CMP_GREATER:    *pResult = (cmpValue > 0 ); break;
        case GBL_VARIANT_OP_CMP_GEQUAL:     *pResult = (cmpValue >= 0); break;
        }
    }}
    GBL_API_END();
}


GBL_INLINE GBL_API gblVariantComparec(const GblVariant* pVariant, const char* pCStr, GBL_VARIANT_OP_CMP_TYPE op, GblBool* pResult) {
    GBL_ASSERT(pCStr);
    GblSize strlength = pCStr? strlen(pCStr) : 0;
    GBL_UNUSED(strlength);
    GBL_ASSERT(strlength < UINT16_MAX);
    const GblStringBuffer tempString = {
        .data = {
            .pCtx = GBL_NULL,
            .pData = (uint8_t*)pCStr,
            .size = (uint16_t)strlength,
            .capacity = 0,
            .elementSize = 1,
            .stackSize = 0
        }
    };
    GBL_API_BEGIN(GBL_NULL);
    GBL_API_CALL(gblVariantCompares(pVariant, &tempString, op, pResult));
    GBL_API_END();
}


GBL_INLINE GBL_API gblVariantComparev(const GblVariant* pLhs,
                          const GblVariant* pRhs,
                          GBL_VARIANT_OP_CMP_TYPE op,
                          GblBool* pResult)
{
    GBL_API_BEGIN(GBL_NULL);
    GBL_API_VERIFY_POINTER(pLhs);
    GBL_API_VERIFY_POINTER(pRhs);
    GBL_API_VERIFY_POINTER(pResult);
    GBL_API_VERIFY_ARG(op <= GBL_VARIANT_OP_CMP_COUNT);
    {
        const GblType type = GblVariant_type(pLhs);
        if(type == GBL_NIL_TYPE) {
            if(pRhs->type == GBL_NIL_TYPE) {
                if(op == GBL_VARIANT_OP_CMP_EQUAL) *pResult = GBL_TRUE;
                else *pResult = GBL_FALSE;
            } else {
                if(op == GBL_VARIANT_OP_CMP_NEQUAL) *pResult = GBL_TRUE;
                else *pResult = GBL_FALSE;
            }
        } else if(type ==  GBL_BOOL_TYPE) {
            GblBool value = GBL_FALSE;
            GBL_API_CALL(gblVariantTob(pRhs, &value));
            GBL_API_CALL(gblVariantCompareb(pLhs, value, op, pResult));
        } else if(type == GBL_INT32_TYPE) {
            GblInt value = 0;
            GBL_API_CALL(gblVariantToi(pRhs, &value));
            GBL_API_CALL(gblVariantComparei(pLhs, value, op, pResult));
        } else if(type == GBL_FLOAT_TYPE) {
            GblFloat value = 0.0f;
            GBL_API_CALL(gblVariantTof(pRhs, &value));
            GBL_API_CALL(gblVariantComparef(pLhs, value, op, pResult));
        } else if(type == GBL_POINTER_TYPE) {
            void* pValue = 0;
            GBL_API_CALL(gblVariantTop(pRhs, &pValue));
            GBL_API_CALL(gblVariantComparep(pLhs, pValue, op, pResult));
        } else if(type == GBL_STRING_TYPE) {
            GblStringBuffer tempStr;
            const GblStringBuffer* pRStr = NULL;
            if(pRhs->type == GBL_STRING_TYPE) {
                pRStr = &pRhs->string;
            } else {
                GBL_API_CALL(GblStringBuffer_construct(&tempStr));
                GBL_API_CALL(gblVariantTos(pRhs, &tempStr));
                pRStr = &tempStr;
            }
            GBL_API_CALL(gblVariantCompares(pLhs, pRStr, op, pResult));
        } else {
            GBL_API_VERIFY(GBL_FALSE, GBL_RESULT_ERROR_TYPE_MISMATCH);
        }
    }
    GBL_API_END();
}


GBL_INLINE GBL_API gblVariantConstructDefault(GblVariant* pVariant) {
    GBL_API_BEGIN(GBL_NULL);
    GBL_API_VERIFY_POINTER(pVariant);
    GBL_API_CALL(GblVariant_constructDefault(pVariant, GBL_NIL_TYPE));
    GBL_API_END();
}

#define GBL_VARIANT_CONSTRUCTOR_DEFINE(suffix, argType, type)                                   \
    GBL_INLINE GBL_API gblVariantConstruct##suffix(GblVariant* pVariant, argType value) {       \
        GBL_API_BEGIN(GBL_NULL);                                                      \
        GBL_API_CALL(GblVariant_constructValueCopy(pVariant, type, value));                     \
        GBL_API_END();                                                                          \
    }


GBL_VARIANT_CONSTRUCTOR_DEFINE(b, GblBool,          GBL_BOOL_TYPE)
GBL_VARIANT_CONSTRUCTOR_DEFINE(u, GblUint,          GBL_UINT32_TYPE)
GBL_VARIANT_CONSTRUCTOR_DEFINE(i, GblInt,           GBL_INT32_TYPE)
GBL_VARIANT_CONSTRUCTOR_DEFINE(f, GblFloat,         GBL_FLOAT_TYPE)
GBL_VARIANT_CONSTRUCTOR_DEFINE(p, void*,            GBL_POINTER_TYPE)

GBL_INLINE GBL_API gblVariantConstructc(GblVariant* pVariant, const char* pString) {
    GBL_API_BEGIN(NULL);
    GBL_API_CALL(GblVariant_constructValueCopy(pVariant, GBL_STRING_TYPE, pString, 0, NULL));
    GBL_API_END();
}

GBL_INLINE GBL_API gblVariantConstructs(GblVariant* pVariant, const GblStringBuffer* pString) {
    GBL_API_BEGIN(NULL);
    const char* pChar = GblStringBuffer_cString(pString);
    GblSize     length = GblStringBuffer_length(pString);
    GblContext* pCtx = GblStringBuffer_context(pString);
    GBL_API_CALL(GblVariant_constructValueCopy(pVariant, GBL_STRING_TYPE, pChar, length, pCtx));
    GBL_API_END();
}

GBL_INLINE GBL_API gblVariantConstructt(GblVariant* pVariant, GblType type) {
    GBL_API_BEGIN(GBL_NULL);
    GBL_API_CALL(GblVariant_constructDefault(pVariant, type));
    GBL_API_END();
}

GBL_INLINE GBL_API gblVariantConstructCopy(GblVariant* pVariant, const GblVariant* pOther) {
    GBL_API_BEGIN(GBL_NULL);
    GBL_API_VERIFY_POINTER(pVariant);
    GBL_API_VERIFY_POINTER(pOther);

    GBL_API_CALL(GblVariant_constructCopy(pVariant, pOther));
    GBL_API_END();
}

GBL_INLINE GBL_API gblVariantConstructMove(GblVariant* pVariant, GblVariant* pOther) {
    GBL_API_BEGIN(GBL_NULL);
    GBL_API_VERIFY_POINTER(pVariant);
    GBL_API_VERIFY_POINTER(pOther);
    GBL_API_CALL(GblVariant_constructMove(pVariant, pOther));
    GBL_API_END();
}

GBL_INLINE GBL_API gblVariantCopy(GblVariant* pVariant, const GblVariant* pOther) {
    GBL_API_BEGIN(GBL_NULL);
    GBL_API_CALL(GblVariant_setCopy(pVariant, pOther));
    GBL_API_END();
}


GBL_INLINE GBL_API gblVariantMovev(GblVariant* pVariant, GblVariant* pOther) {
    GBL_API_BEGIN(GBL_NULL);
    GBL_API_CALL(GblVariant_setMove(pVariant, pOther));
    GBL_API_END();
}

//== MAKE ME TAKE A BUFFER INSTEAD ===
GBL_INLINE GBL_API gblVariantMovec(GblVariant* pVariant, char* pBuffer, GblSize capacity) {
    GBL_API_BEGIN(GBL_NULL);
    GBL_API_CALL(GblVariant_setValueMove(pVariant, GBL_STRING_TYPE, pBuffer, capacity));
    /*
    GBL_API_CALL(gblVariantTypeSet(pVariant, GBL_STRING_TYPE));
    GBL_API_CALL(GblStringBuffer_give(&pVariant->string, pBuffer, capacity));
    GBL_API_END();*/
    GBL_API_END();
}

GBL_INLINE GBL_API gblVariantMoves(GblVariant* pVariant, GblStringBuffer* pOther) {
    GblBool stack = GBL_FALSE;
    char* pOtherBuff = NULL;
    GblSize otherCapacity = 0;
    GBL_API_BEGIN(GBL_NULL);
    GBL_API_VERIFY_POINTER(pOther);
    stack = GblStringBuffer_stack(pOther);
    if(stack) {
        GBL_API_CALL(gblVariantSets(pVariant, pOther));
    } else {
        GBL_API_CALL(GblStringBuffer_take(pOther, &pOtherBuff, &otherCapacity));
        GBL_API_CALL(gblVariantMovec(pVariant, pOtherBuff, otherCapacity));
    }
    GBL_API_END();
}

GBL_INLINE GBL_API gblVariantDestruct(GblVariant* pVariant) {
    GBL_API_BEGIN(GBL_NULL);
    GBL_API_VERIFY_POINTER(pVariant);
    GBL_API_CALL(GblVariant_destruct(pVariant));
    GBL_API_END();
}


#undef CSELF
#undef SELF


GBL_DECLS_END








#endif // GBLBAL_VARIANT__H
