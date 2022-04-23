#ifndef GIMBAL_VARIANT__H
#define GIMBAL_VARIANT__H

#include "../types/gimbal_string.h"
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

typedef struct GblVariant {
    union {
        GblBool         boolean;
        char            character;
        uint8_t         u8;
        uint16_t        u16;
        int16_t         i16;
        uint32_t        u32;
        int32_t         i32;
        uint64_t        u64;
        int16_t         i64;
        GblEnum         enumeration;
        GblFlags        flags;
        float           f32;
        double          f64;
        void*           pVoid;

        GblInt          integer;
        uint64_t        uinteger64;
        GblFloat        floating;
        void*           pVoidPtr;
        GblString       string;
    };
    GblType             type;
} GblVariant;

#define GBL_VARIANT_CONSTRUCT_TRAITS (                      \
        GBL_META_GENERIC_MACRO_NO_DEFAULT,                  \
        (                                                   \
            (GblBool,           gblVariantConstructb),      \
            (GblInt,            gblVariantConstructi),      \
            (GblUint,           gblVariantConstructu),      \
            (GblFloat,          gblVariantConstructf),      \
            (void*,             gblVariantConstructp),      \
            (const GblString*,  gblVariantConstructs),      \
            (GblString*,        gblVariantConstructs),      \
            (const char*,       gblVariantConstructc),      \
            (char*,             gblVariantConstructc),      \
            (const GblVariant*, gblVariantConstructCopy)    \
        )                                                   \
    )
#define gblVariantConstruct_N(pVariant, pValue, ...) GBL_META_GENERIC_MACRO_GENERATE(GBL_VARIANT_CONSTRUCT_TRAITS, pValue)(pVariant, pValue GBL_VA_ARGS(__VA_ARGS__))
#define gblVariantConstruct_1(pVariant)              gblVariantConstructDefault(pVariant)
#define gblVariantConstruct(...)                    GBL_VA_OVERLOAD_CALL(gblVariantConstruct, GBL_VA_OVERLOAD_SUFFIXER_1_N, __VA_ARGS__)

#define gblVariantSetc(...) GBL_VA_OVERLOAD_CALL(gblVariantSetc, GBL_VA_OVERLOAD_SUFFIXER_ARGC, __VA_ARGS__)

#define GBL_VARIANT_SET_TRAITS (                    \
        GBL_META_GENERIC_MACRO_NO_DEFAULT,        \
        (                                           \
            (GblBool,           gblVariantSetb),    \
            (GblInt,            gblVariantSeti),    \
            (GblFloat,          gblVariantSetf),    \
            (void*,             gblVariantSetp),    \
            (const GblString*,  gblVariantSets),    \
            (GblString*,        gblVariantSets),    \
            (const char*,       gblVariantSetc_2),  \
            (char*,             gblVariantSetc_2) \
        )                                           \
    )
#define gblVariantSet(pVariant, value, ...) GBL_META_GENERIC_MACRO_GENERATE(GBL_VARIANT_SET_TRAITS, value)(pVariant, value GBL_VA_ARGS(__VA_ARGS__))


#define GBL_VARIANT_MOVE_TRAITS (                    \
        GBL_META_GENERIC_MACRO_NO_DEFAULT,        \
        (                                           \
            (GblVariant*,       gblVariantMovev),    \
            (GblString*,        gblVariantMoves)    \
        )                                           \
    )
#define gblVariantMove(pVariant, value, ...) GBL_META_GENERIC_MACRO_GENERATE(GBL_VARIANT_MOVE_TRAITS, value)(pVariant, value GBL_VA_ARGS(__VA_ARGS__))



#define GBL_VARIANT_GET_TRAITS (                    \
        GBL_META_GENERIC_MACRO_NO_DEFAULT,          \
        (                                           \
            (GblBool*,          gblVariantGetb),    \
            (GblInt*,           gblVariantGeti),    \
            (GblFloat*,         gblVariantGetf),    \
            (void**,            gblVariantGetp),    \
            (const GblString**, gblVariantGets),    \
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
            (GblString*,    gblVariantTos),     \
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
            (const GblString*,  gblVariantCompares),    \
            (GblString*,        gblVariantCompares),    \
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
    GBL_API_VERIFY_TYPE(pVariant->type, GBL_TYPE_BOOL);
    GBL_API_CALL(GblVariant_getValueCopy(pVariant, pValue));
    GBL_API_END();
}

GBL_INLINE GBL_API gblVariantGeti(const GblVariant* pVariant,    GblInt*      pValue) {
    GBL_API_BEGIN(GBL_NULL);
    GBL_API_VERIFY_POINTER(pVariant);
    GBL_API_VERIFY_POINTER(pValue);
    GBL_API_VERIFY_TYPE(pVariant->type, GBL_TYPE_INT32);
    GBL_API_CALL(GblVariant_getValueCopy(pVariant, pValue));
    GBL_API_END();
}

GBL_INLINE GBL_API gblVariantGetf(const GblVariant* pVariant,    GblFloat*    pValue) {
    GBL_API_BEGIN(GBL_NULL);
    GBL_API_VERIFY_POINTER(pVariant);
    GBL_API_VERIFY_POINTER(pValue);
    GBL_API_VERIFY_TYPE(pVariant->type, GBL_TYPE_FLOAT);
    GBL_API_CALL(GblVariant_getValueCopy(pVariant, pValue));
    GBL_API_END();
}

GBL_INLINE GBL_API gblVariantGetp(const GblVariant* pVariant,    void**     pValue) {
    GBL_API_BEGIN(GBL_NULL);
    GBL_API_VERIFY_POINTER(pVariant);
    GBL_API_VERIFY_POINTER(pValue);
    GBL_API_VERIFY_TYPE(pVariant->type, GBL_TYPE_POINTER);
    GBL_API_CALL(GblVariant_getValueCopy(pVariant, pValue));
    GBL_API_END();
}

GBL_INLINE GBL_API gblVariantGets(const GblVariant* pVariant,  const GblString** pString) {
    GBL_API_BEGIN(GBL_NULL);
    GBL_API_VERIFY_POINTER(pVariant);
    GBL_API_VERIFY_POINTER(pString);
    GBL_API_VERIFY_TYPE(pVariant->type, GBL_TYPE_STRING);
    *pString = &pVariant->string;
    GBL_API_END();
}

GBL_INLINE GBL_API gblVariantGetc(const GblVariant* pVariant, const char** ppStr) {
    GBL_API_BEGIN(GBL_NULL);
    GBL_API_VERIFY_TYPE(pVariant->type, GBL_TYPE_STRING);
    GBL_API_CALL(GblVariant_getValuePeek(pVariant, ppStr));
    GBL_API_END();
}

// Set Value
GBL_INLINE GBL_API gblVariantSetNil(GblVariant* pVariant) {
    GBL_API_BEGIN(GBL_NULL);
    GBL_API_VERIFY_POINTER(pVariant);
    GBL_API_CALL(GblVariant_setValueCopy(pVariant, GBL_TYPE_NIL));
    //GBL_API_CALL(gblVariantTypeSet(pVariant, GBL_TYPE_NIL));
    GBL_API_END();
}

GBL_INLINE GBL_API gblVariantSetb(GblVariant* pVariant,    GblBool     value) {
    GBL_API_BEGIN(GBL_NULL);
    GBL_API_VERIFY_POINTER(pVariant);
    GBL_API_CALL(GblVariant_setValueCopy(pVariant, GBL_TYPE_BOOL, value));
    //GBL_API_CALL(gblVariantTypeSet(pVariant, GBL_TYPE_BOOL));
    //pVariant->boolean = value;
    GBL_API_END();
}

GBL_INLINE GBL_API gblVariantSeti(GblVariant* pVariant,    GblInt      value) {
    GBL_API_BEGIN(GBL_NULL);
    GBL_API_VERIFY_POINTER(pVariant);
    GBL_API_CALL(GblVariant_setValueCopy(pVariant, GBL_TYPE_INT32, value));
    //GBL_API_CALL(gblVariantTypeSet(pVariant, GBL_TYPE_INT32));
    //pVariant->integer = value;
    GBL_API_END();
}

GBL_INLINE GBL_API gblVariantSetf(GblVariant* pVariant,    GblFloat       value) {
    GBL_API_BEGIN(GBL_NULL);
    GBL_API_VERIFY_POINTER(pVariant);
    GBL_API_CALL(GblVariant_setValueCopy(pVariant, GBL_TYPE_FLOAT, value));
//    GBL_API_CALL(gblVariantTypeSet(pVariant, GBL_TYPE_FLOAT));
//    pVariant->floating = value;
    GBL_API_END();
}

GBL_INLINE GBL_API gblVariantSetp(GblVariant* pVariant,    void*       pValue) {
    GBL_API_BEGIN(GBL_NULL);
    GBL_API_VERIFY_POINTER(pVariant);
    GBL_API_CALL(GblVariant_setValueCopy(pVariant, GBL_TYPE_POINTER, pValue));
    //GBL_API_CALL(gblVariantTypeSet(pVariant, GBL_TYPE_POINTER));
    //pVariant->pVoidPtr = pValue;
    GBL_API_END();
}

GBL_INLINE GBL_API gblVariantSets(GblVariant* pVariant, const GblString* pString) {
    GBL_API_BEGIN(GBL_NULL);
    GBL_API_VERIFY_POINTER(pVariant);
    GBL_API_VERIFY_POINTER(pString);
    const char* pChar;
    GblSize     length;
    GblContext* pCtx;

    GBL_API_CALL(gblStringCStr(pString, &pChar));
    GBL_API_CALL(gblStringLength(pString, &length));
    GBL_API_CALL(gblStringContext(pString, &pCtx));

    GBL_API_CALL(GblVariant_setValueCopy(pVariant, GBL_TYPE_STRING, pChar, length, pCtx));
#if 0
    view.pBuffer = pString->data.pBuffer; view.size = pString->data.size;
    if(pVariant->type != GBL_TYPE_STRING) {
        GBL_API_CALL(gblVariantTypeSet(pVariant, GBL_TYPE_STRING));
        GBL_API_CALL(gblStringConstruct(&pVariant->string, sizeof(GblString), pString->data.hCtx, &view));
    } else {
        GBL_API_CALL(gblStringAssign(&pVariant->string, &view));
    }
#endif
    GBL_API_END();
}


GBL_INLINE GBL_API gblVariantSetc_3(GblVariant* pVariant, const char* pString, GblContext* pCtx) {
    GBL_API_BEGIN(GBL_NULL);
    GblStringView view;
    GBL_API_VERIFY_POINTER(pVariant);

    GBL_API_CALL(GblVariant_setValueCopy(pVariant, GBL_TYPE_STRING, pString, 0, pCtx));
    /*
    view.pBuffer = pString; view.size = 0;
    if(pVariant->type != GBL_TYPE_STRING) {
        GBL_API_CALL(gblVariantTypeSet(pVariant, GBL_TYPE_STRING));
        GBL_API_CALL(gblStringConstruct(&pVariant->string, sizeof(GblString), hCtx, &view));
    } else {
        GBL_API_CALL(gblStringAssign(&pVariant->string, &view));
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
        if(type == GBL_TYPE_BOOL)           *pValue = pVariant->boolean;
        else if(type == GBL_TYPE_INT32)     *pValue = pVariant->i32? GBL_TRUE : GBL_FALSE;
        else if(type == GBL_TYPE_FLOAT)     *pValue = pVariant->floating != 0.0f? GBL_TRUE : GBL_FALSE;
        else if(type == GBL_TYPE_POINTER)   *pValue = pVariant->pVoidPtr? GBL_TRUE : GBL_FALSE;
        else if(type == GBL_TYPE_STRING)    GBL_API_CALL(gblStringTob(&pVariant->string, pValue));
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
        if(type == GBL_TYPE_BOOL)           *pValue = pVariant->boolean? 1 : 0;
        else if(type == GBL_TYPE_INT32)     *pValue = pVariant->integer;
        else if(type == GBL_TYPE_FLOAT)     *pValue = (GblInt)pVariant->floating;
        else if(type == GBL_TYPE_POINTER)   *pValue = (uintptr_t)pVariant->pVoidPtr;
        else if(type ==GBL_TYPE_STRING)     GBL_API_CALL(gblStringToi(&pVariant->string, pValue));
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
        if(type == GBL_TYPE_INT32)          *pValue = (GblFloat)pVariant->integer;
        else if(type == GBL_TYPE_FLOAT)    *pValue = pVariant->floating;
        else if(type == GBL_TYPE_STRING)   GBL_API_CALL(gblStringTof(&pVariant->string, pValue));
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
        if(type == GBL_TYPE_POINTER) *pValue = pVariant->pVoidPtr;
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
        if(type == GBL_TYPE_STRING)
            GBL_API_CALL(gblVariantGetc(pVariant, ppCString));
        else {
            *ppCString = NULL;
            GBL_API_RECORD_SET(GBL_RESULT_ERROR_TYPE_MISMATCH, "Cannot convert type: %u to const char*!", pVariant->type);
        }
    }
    GBL_API_END();
}


// BETTER ALREADY BE CONSTRUCTED, SO THAT IT ALREADY HAS A FUCKING CONTEXT ASSOCIATED WITH IT!
GBL_INLINE GBL_API gblVariantTos(const GblVariant* pVariant, GblString* pString) {
    GBL_API_BEGIN(GBL_NULL);
    GblStringView view;
    GBL_API_VERIFY_POINTER(pVariant);
    GBL_API_VERIFY_POINTER(pString);

    if(pVariant->type == GBL_TYPE_STRING) {
        view.pBuffer = pVariant->string.data.pBuffer; view.size = pVariant->string.data.size;
        GBL_API_CALL(gblStringConstruct(pString, sizeof(GblString), GBL_NULL, &view));
    }  else {

        GBL_API_CALL(gblStringConstruct(pString, sizeof(GblString), GBL_NULL, NULL));
        const GblType type = GblVariant_type(pVariant);
        if(type == GBL_TYPE_NIL)
            GBL_API_CALL(gblStringFromNil(pString));
        else if(type == GBL_TYPE_BOOL)
            GBL_API_CALL(gblStringFromb(pString, pVariant->boolean));
        else if(type == GBL_TYPE_INT32)
            GBL_API_CALL(gblStringFromi(pString, pVariant->integer));
        else if(type == GBL_TYPE_UINT32)
            GBL_API_CALL(gblStringFromu(pString, pVariant->u32));
        else if(type == GBL_TYPE_FLOAT)
            GBL_API_CALL(gblStringFromf(pString, pVariant->floating));
        else if(type == GBL_TYPE_POINTER)
            GBL_API_CALL(gblStringFromp(pString, pVariant->pVoidPtr));
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

GBL_INLINE GBL_API gblVariantCompares(const GblVariant* pVariant, const GblString* pString, GBL_VARIANT_OP_CMP_TYPE op, GblBool* pResult) {
    GblString tempString;
    const GblString* pThisString = NULL;
    GBL_API_BEGIN(GBL_NULL);
    GBL_API_VERIFY_POINTER(pVariant);
    GBL_API_VERIFY_POINTER(pString);
    GBL_API_VERIFY_ARG(op < GBL_VARIANT_OP_CMP_COUNT);
    GBL_API_VERIFY_POINTER(pResult);
    if(pVariant->type == GBL_TYPE_STRING) {
        pThisString = &pVariant->string;
    } else {
        GBL_API_CALL(gblVariantTos(pVariant, &tempString));
        pThisString = &tempString;
    }
    switch(op) {
    case GBL_VARIANT_OP_CMP_EQUAL: {
        GblInt result = 0;
        GBL_API_CALL(gblStringCompare(pThisString, pString, &result));
        *pResult = result == 0;
        break;
    }
    case GBL_VARIANT_OP_CMP_NEQUAL: {
        GblInt result = 1;
        GBL_API_CALL(gblStringCompare(pThisString, pString, &result));
        *pResult = result != 0;
        break;
    }
    default: {
        const char* pBuffer1 = NULL;
        const char* pBuffer2 = NULL;
        GblSize strLen1 = 0;
        GblSize strLen2 = 0;
        GBL_API_CALL(gblStringCStr(pThisString, &pBuffer1));
        GBL_API_CALL(gblStringCStr(pString, &pBuffer2));
        GBL_API_CALL(gblStringLength(pThisString, &strLen1));
        GBL_API_CALL(gblStringLength(pString, &strLen2));
        const int cmpValue = strncmp(pBuffer1, pBuffer2, strLen1 < strLen2? strLen1 : strLen2);
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
    const GblString tempString = {
        .data = {
            .pCtx = GBL_NULL,
            .pBuffer = (char*)pCStr,
            .size = pCStr? strlen(pCStr) : 0,
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
        if(type == GBL_TYPE_NIL) {
            if(pRhs->type == GBL_TYPE_NIL) {
                if(op == GBL_VARIANT_OP_CMP_EQUAL) *pResult = GBL_TRUE;
                else *pResult = GBL_FALSE;
            } else {
                if(op == GBL_VARIANT_OP_CMP_NEQUAL) *pResult = GBL_TRUE;
                else *pResult = GBL_FALSE;
            }
        } else if(type ==  GBL_TYPE_BOOL) {
            GblBool value = GBL_FALSE;
            GBL_API_CALL(gblVariantTob(pRhs, &value));
            GBL_API_CALL(gblVariantCompareb(pLhs, value, op, pResult));
        } else if(type == GBL_TYPE_INT32) {
            GblInt value = 0;
            GBL_API_CALL(gblVariantToi(pRhs, &value));
            GBL_API_CALL(gblVariantComparei(pLhs, value, op, pResult));
        } else if(type == GBL_TYPE_FLOAT) {
            GblFloat value = 0.0f;
            GBL_API_CALL(gblVariantTof(pRhs, &value));
            GBL_API_CALL(gblVariantComparef(pLhs, value, op, pResult));
        } else if(type == GBL_TYPE_POINTER) {
            void* pValue = 0;
            GBL_API_CALL(gblVariantTop(pRhs, &pValue));
            GBL_API_CALL(gblVariantComparep(pLhs, pValue, op, pResult));
        } else if(type == GBL_TYPE_STRING) {
            GblString tempStr;
            const GblString* pRStr = NULL;
            if(pRhs->type == GBL_TYPE_STRING) {
                pRStr = &pRhs->string;
            } else {
                GBL_API_CALL(gblStringConstruct(&tempStr, sizeof(GblString), GBL_NULL, NULL));
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
    GBL_API_CALL(GblVariant_constructDefault(pVariant, GBL_TYPE_NIL));
    GBL_API_END();
}

#define GBL_VARIANT_CONSTRUCTOR_DEFINE(suffix, argType, type)                                   \
    GBL_INLINE GBL_API gblVariantConstruct##suffix(GblVariant* pVariant, argType value) {       \
        GBL_API_BEGIN(GBL_NULL);                                                      \
        GBL_API_CALL(GblVariant_constructValueCopy(pVariant, type, value));                     \
        GBL_API_END();                                                                          \
    }


GBL_VARIANT_CONSTRUCTOR_DEFINE(b, GblBool,          GBL_TYPE_BOOL)
GBL_VARIANT_CONSTRUCTOR_DEFINE(u, GblUint,          GBL_TYPE_UINT32)
GBL_VARIANT_CONSTRUCTOR_DEFINE(i, GblInt,           GBL_TYPE_INT32)
GBL_VARIANT_CONSTRUCTOR_DEFINE(f, GblFloat,         GBL_TYPE_FLOAT)
GBL_VARIANT_CONSTRUCTOR_DEFINE(p, void*,            GBL_TYPE_POINTER)

GBL_INLINE GBL_API gblVariantConstructc(GblVariant* pVariant, const char* pString) {
    GBL_API_BEGIN(NULL);
    GBL_API_CALL(GblVariant_constructValueCopy(pVariant, GBL_TYPE_STRING, pString, 0, NULL));
    GBL_API_END();
}

GBL_INLINE GBL_API gblVariantConstructs(GblVariant* pVariant, const GblString* pString) {
    GBL_API_BEGIN(NULL);
    const char* pChar;
    GblSize     length;
    GblContext* pCtx;

    GBL_API_CALL(gblStringCStr(pString, &pChar));
    GBL_API_CALL(gblStringLength(pString, &length));
    GBL_API_CALL(gblStringContext(pString, &pCtx));

    GBL_API_CALL(GblVariant_constructValueCopy(pVariant, GBL_TYPE_STRING, pChar, length, pCtx));
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
    GBL_API_CALL(GblVariant_setValueMove(pVariant, GBL_TYPE_STRING, pBuffer, capacity));
    /*
    GBL_API_CALL(gblVariantTypeSet(pVariant, GBL_TYPE_STRING));
    GBL_API_CALL(gblStringGive(&pVariant->string, pBuffer, capacity));
    GBL_API_END();*/
    GBL_API_END();
}

GBL_INLINE GBL_API gblVariantMoves(GblVariant* pVariant, GblString* pOther) {
    GblBool stack = GBL_FALSE;
    char* pOtherBuff = NULL;
    GblSize otherCapacity = 0;
    GBL_API_BEGIN(GBL_NULL);
    GBL_API_VERIFY_POINTER(pOther);
    GBL_API_CALL(gblStringIsStack(pOther, &stack));
    if(stack) {
        GBL_API_CALL(gblVariantSets(pVariant, pOther));
    } else {
        GBL_API_CALL(gblStringTake(pOther, &pOtherBuff, &otherCapacity));
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
