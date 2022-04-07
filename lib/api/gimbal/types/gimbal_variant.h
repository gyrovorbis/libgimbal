#ifndef GIMBAL_VARIANT__H
#define GIMBAL_VARIANT__H

#include "../objects/gimbal_object.h"
#include "../types/gimbal_string.h"
#include "../meta/gimbal_ivariant.h"
#include "../types/gimbal_typedefs.h"
#include <stdint.h>

#define SELF    GblVariant* pSelf
#define CSELF   const SELF

GBL_DECLS_BEGIN

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
        GblObject_      hObject;
    };
    GBL_VARIANT_TYPE    type;
} GblVariant;

#define GBL_VARIANT_CONSTRUCT_TRAITS (                      \
        GBL_META_GENERIC_MACRO_NO_DEFAULT,                  \
        (                                                   \
            (GblBool,           gblVariantConstructb),      \
            (GblInt,            gblVariantConstructi),      \
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


GBL_INLINE GBL_API gblVariantTypeSet(GblVariant* pVariant, GBL_VARIANT_TYPE type) {
    GBL_API_BEGIN(GBL_HANDLE_INVALID);
    GBL_API_VERIFY_ARG(type <= GBL_VARIANT_TYPE_BUILTIN_COUNT);
    // FREE/RELEASE SHIT IF REFERENCE TYPE
    if(pVariant->type != type) {
        if(pVariant->type == GBL_VARIANT_TYPE_STRING) {
            GBL_API_CALL(gblStringDestruct(&pVariant->string));
        }
    }
    pVariant->type = type;
    GBL_API_END();
}

// Top-level Utilities
GBL_INLINE GBL_API gblVariantTypeGet(const GblVariant* pVariant, GBL_VARIANT_TYPE* pType) {
    GBL_API_BEGIN(GBL_HANDLE_INVALID);
    GBL_API_VERIFY_POINTER(pType);
    *pType = pVariant->type;
    GBL_API_END();
}


// Get (Actual) Value
GBL_INLINE GBL_API gblVariantGetb(const GblVariant* pVariant, GblBool* pValue) {
    GBL_API_BEGIN(GBL_HANDLE_INVALID);
    GBL_API_VERIFY_POINTER(pVariant);
    GBL_API_VERIFY_POINTER(pValue);
    GBL_API_VERIFY(pVariant->type == GBL_VARIANT_TYPE_BOOL,
                   GBL_RESULT_ERROR_TYPE_MISMATCH);
    *pValue = pVariant->boolean;
    GBL_API_END();
}

GBL_INLINE GBL_API gblVariantGeti(const GblVariant* pVariant,    GblInt*      pValue) {
    GBL_API_BEGIN(GBL_HANDLE_INVALID);
    GBL_API_VERIFY_POINTER(pVariant);
    GBL_API_VERIFY_POINTER(pValue);
    GBL_API_VERIFY(pVariant->type == GBL_VARIANT_TYPE_INT,
                   GBL_RESULT_ERROR_TYPE_MISMATCH);
    *pValue = pVariant->integer;
    GBL_API_END();
}

GBL_INLINE GBL_API gblVariantGetf(const GblVariant* pVariant,    GblFloat*    pValue) {
    GBL_API_BEGIN(GBL_HANDLE_INVALID);
    GBL_API_VERIFY_POINTER(pVariant);
    GBL_API_VERIFY_POINTER(pValue);
    GBL_API_VERIFY(pVariant->type == GBL_VARIANT_TYPE_FLOAT,
                   GBL_RESULT_ERROR_TYPE_MISMATCH);
    *pValue = pVariant->floating;
    GBL_API_END();
}

GBL_INLINE GBL_API gblVariantGetp(const GblVariant* pVariant,    void**       pValue) {
    GBL_API_BEGIN(GBL_HANDLE_INVALID);
    GBL_API_VERIFY_POINTER(pVariant);
    GBL_API_VERIFY_POINTER(pValue);
    GBL_API_VERIFY(pVariant->type == GBL_VARIANT_TYPE_VOID_PTR,
                   GBL_RESULT_ERROR_TYPE_MISMATCH);
    *pValue = pVariant->pVoidPtr;
    GBL_API_END();
}


GBL_INLINE GBL_API gblVariantGets(const GblVariant* pVariant,  const GblString** pString) {
    GBL_API_BEGIN(GBL_HANDLE_INVALID);
    GBL_API_VERIFY_POINTER(pVariant);
    GBL_API_VERIFY_POINTER(pString);
    GBL_API_VERIFY(pVariant->type == GBL_VARIANT_TYPE_STRING,
                   GBL_RESULT_ERROR_TYPE_MISMATCH);
    *pString = &pVariant->string;
    GBL_API_END();
}

GBL_INLINE GBL_API gblVariantGetc(const GblVariant* pVariant, const char** ppStr) {
    const GblString* pGblStr = NULL;
    GBL_API_BEGIN(GBL_HANDLE_INVALID);
    GBL_API_CALL(gblVariantGets(pVariant, &pGblStr));
    GBL_API_CALL(gblStringCStr(pGblStr, ppStr));
    GBL_API_END();
}

// Set Value
GBL_INLINE GBL_API gblVariantSetNil(GblVariant* pVariant) {
    GBL_API_BEGIN(GBL_HANDLE_INVALID);
    GBL_API_VERIFY_POINTER(pVariant);
    GBL_API_CALL(gblVariantTypeSet(pVariant, GBL_VARIANT_TYPE_NIL));
    GBL_API_END();
}

GBL_INLINE GBL_API gblVariantSetb(GblVariant* pVariant,    GblBool     value) {
    GBL_API_BEGIN(GBL_HANDLE_INVALID);
    GBL_API_VERIFY_POINTER(pVariant);
    GBL_API_CALL(gblVariantTypeSet(pVariant, GBL_VARIANT_TYPE_BOOL));
    pVariant->boolean = value;
    GBL_API_END();
}

GBL_INLINE GBL_API gblVariantSeti(GblVariant* pVariant,    GblInt      value) {
    GBL_API_BEGIN(GBL_HANDLE_INVALID);
    GBL_API_VERIFY_POINTER(pVariant);
    GBL_API_CALL(gblVariantTypeSet(pVariant, GBL_VARIANT_TYPE_INT));
    pVariant->integer = value;
    GBL_API_END();
}

GBL_INLINE GBL_API gblVariantSetf(GblVariant* pVariant,    GblFloat       value) {
    GBL_API_BEGIN(GBL_HANDLE_INVALID);
    GBL_API_VERIFY_POINTER(pVariant);
    GBL_API_CALL(gblVariantTypeSet(pVariant, GBL_VARIANT_TYPE_FLOAT));
    pVariant->floating = value;
    GBL_API_END();
}

GBL_INLINE GBL_API gblVariantSetp(GblVariant* pVariant,    void*       pValue) {
    GBL_API_BEGIN(GBL_HANDLE_INVALID);
    GBL_API_VERIFY_POINTER(pVariant);
    GBL_API_CALL(gblVariantTypeSet(pVariant, GBL_VARIANT_TYPE_VOID_PTR));
    pVariant->pVoidPtr = pValue;
    GBL_API_END();
}

GBL_INLINE GBL_API gblVariantSets(GblVariant* pVariant, const GblString* pString) {
    GBL_API_BEGIN(GBL_HANDLE_INVALID);
    GblStringView view;
    GBL_API_VERIFY_POINTER(pVariant);
    GBL_API_VERIFY_POINTER(pString);
    view.pBuffer = pString->data.pBuffer; view.size = pString->data.size;
    if(pVariant->type != GBL_VARIANT_TYPE_STRING) {
        GBL_API_CALL(gblVariantTypeSet(pVariant, GBL_VARIANT_TYPE_STRING));
        GBL_API_CALL(gblStringConstruct(&pVariant->string, sizeof(GblString), pString->data.hCtx, &view));
    } else {
        GBL_API_CALL(gblStringAssign(&pVariant->string, &view));
    }
    GBL_API_END();
}


GBL_INLINE GBL_API gblVariantSetc_3(GblVariant* pVariant, const char* pString, GblContext hCtx) {
    GBL_API_BEGIN(GBL_HANDLE_INVALID);
    GblStringView view;
    GBL_API_VERIFY_POINTER(pVariant);
    view.pBuffer = pString; view.size = 0;
    if(pVariant->type != GBL_VARIANT_TYPE_STRING) {
        GBL_API_CALL(gblVariantTypeSet(pVariant, GBL_VARIANT_TYPE_STRING));
        GBL_API_CALL(gblStringConstruct(&pVariant->string, sizeof(GblString), hCtx, &view));
    } else {
        GBL_API_CALL(gblStringAssign(&pVariant->string, &view));
    }
    GBL_API_END();
}

GBL_INLINE GBL_API gblVariantSetc_2(GblVariant* pVariant, const char* pString) {
    return gblVariantSetc_3(pVariant, pString, GBL_CONTEXT_INVALID);
}


// (MAYBE) Convert Value

// Get (Actual) Value
GBL_INLINE GBL_API gblVariantTob(const GblVariant* pVariant, GblBool* pValue) {
    GBL_API_BEGIN(GBL_HANDLE_INVALID);
    GBL_API_VERIFY_POINTER(pVariant);
    GBL_API_VERIFY_POINTER(pValue);
    switch(pVariant->type) {
    case GBL_VARIANT_TYPE_BOOL:     *pValue = pVariant->boolean; break;
    case GBL_VARIANT_TYPE_INT:      *pValue = pVariant->integer? GBL_TRUE : GBL_FALSE; break;
    case GBL_VARIANT_TYPE_FLOAT:    *pValue = pVariant->floating != 0.0f? GBL_TRUE : GBL_FALSE; break;
    case GBL_VARIANT_TYPE_VOID_PTR: *pValue = pVariant->pVoidPtr? GBL_TRUE : GBL_FALSE; break;
    case GBL_VARIANT_TYPE_STRING:   GBL_API_CALL(gblStringTob(&pVariant->string, pValue)); break;
    default: *pValue = GBL_FALSE;
    }
    GBL_API_END();
}

GBL_INLINE GBL_API gblVariantToi(const GblVariant* pVariant,    GblInt*      pValue) {
    GBL_API_BEGIN(GBL_HANDLE_INVALID);
    GBL_API_VERIFY_POINTER(pVariant);
    GBL_API_VERIFY_POINTER(pValue);
    switch(pVariant->type) {
    case GBL_VARIANT_TYPE_BOOL:     *pValue = pVariant->boolean? 1 : 0; break;
    case GBL_VARIANT_TYPE_INT:      *pValue = pVariant->integer; break;
    case GBL_VARIANT_TYPE_FLOAT:    *pValue = (GblInt)pVariant->floating; break;
    case GBL_VARIANT_TYPE_VOID_PTR: *pValue = (uintptr_t)pVariant->pVoidPtr;
    case GBL_VARIANT_TYPE_STRING:   GBL_API_CALL(gblStringToi(&pVariant->string, pValue)); break;
    default: *pValue = 0;
    }
    GBL_API_END();
}

GBL_INLINE GBL_API gblVariantTof(const GblVariant* pVariant,    GblFloat*    pValue) {
    GBL_API_BEGIN(GBL_HANDLE_INVALID);
    GBL_API_VERIFY_POINTER(pVariant);
    GBL_API_VERIFY_POINTER(pValue);
    switch(pVariant->type) {
    case GBL_VARIANT_TYPE_INT:      *pValue = (GblFloat)pVariant->integer; break;
    case GBL_VARIANT_TYPE_FLOAT:    *pValue = pVariant->floating; break;
    case GBL_VARIANT_TYPE_STRING:   GBL_API_CALL(gblStringTof(&pVariant->string, pValue)); break;
    default: *pValue = 0.0f;
        GBL_API_VERIFY(GBL_FALSE, GBL_RESULT_ERROR_TYPE_MISMATCH, "Cannot convert type: %u to float!", pVariant->type);
    }
    GBL_API_END();
}

GBL_INLINE GBL_API gblVariantTop(const GblVariant* pVariant,    void**       pValue) {
    GBL_API_BEGIN(GBL_HANDLE_INVALID);
    GBL_API_VERIFY_POINTER(pVariant);
    GBL_API_VERIFY_POINTER(pValue);
    switch(pVariant->type) {
    case GBL_VARIANT_TYPE_VOID_PTR: *pValue = pVariant->pVoidPtr; break;
    default: *pValue = 0;
        GBL_API_VERIFY(GBL_FALSE, GBL_RESULT_ERROR_TYPE_MISMATCH, "Cannot convert type: %u to void*!", pVariant->type);
    }
    GBL_API_END();
}

GBL_INLINE GBL_API gblVariantToc(const GblVariant* pVariant, const char** ppCString) {
    GBL_API_BEGIN(GBL_HANDLE_INVALID);
    GBL_API_VERIFY_POINTER(pVariant);
    GBL_API_VERIFY_POINTER(ppCString);
    switch(pVariant->type) {
    case GBL_VARIANT_TYPE_STRING:
        GBL_API_CALL(gblVariantGetc(pVariant, ppCString));
        break;
    default: *ppCString = NULL;
        GBL_API_RECORD_SET(GBL_RESULT_ERROR_TYPE_MISMATCH, "Cannot convert type: %u to const char*!", pVariant->type);
    }
    GBL_API_END();
}


// BETTER ALREADY BE CONSTRUCTED, SO THAT IT ALREADY HAS A FUCKING CONTEXT ASSOCIATED WITH IT!
GBL_INLINE GBL_API gblVariantTos(const GblVariant* pVariant, GblString* pString) {
    GBL_API_BEGIN(GBL_HANDLE_INVALID);
    GblStringView view;
    GBL_API_VERIFY_POINTER(pVariant);
    GBL_API_VERIFY_POINTER(pString);

    if(pVariant->type == GBL_VARIANT_TYPE_STRING) {
        view.pBuffer = pVariant->string.data.pBuffer; view.size = pVariant->string.data.size;
        GBL_API_CALL(gblStringConstruct(pString, sizeof(GblString), GBL_HANDLE_INVALID, &view));
    }  else {

        GBL_API_CALL(gblStringConstruct(pString, sizeof(GblString), GBL_HANDLE_INVALID, NULL));

        switch(pVariant->type) {
        case GBL_VARIANT_TYPE_NIL:
            GBL_API_CALL(gblStringFromNil(pString));
            break;
        case GBL_VARIANT_TYPE_BOOL:
            GBL_API_CALL(gblStringFromb(pString, pVariant->boolean));
            break;
        case GBL_VARIANT_TYPE_INT:
            GBL_API_CALL(gblStringFromi(pString, pVariant->integer));
            break;
        case GBL_VARIANT_TYPE_FLOAT:
            GBL_API_CALL(gblStringFromf(pString, pVariant->floating));
            break;
        case GBL_VARIANT_TYPE_VOID_PTR:
            GBL_API_CALL(gblStringFromp(pString, pVariant->pVoidPtr));
            break;
        default: GBL_API_VERIFY(GBL_FALSE, GBL_RESULT_ERROR_TYPE_MISMATCH, "Attempted to convert unknown variant type %u to string!", pVariant->type);
            break;
        }
    }

    GBL_API_END();
}

#define GBL_VARIANT_COMPARE_INTEGRAL_DEFINE(suffix, type, initialValue) \
GBL_INLINE GBL_API gblVariantCompare##suffix(const GblVariant* pVariant, type rhs, GBL_VARIANT_OP_CMP_TYPE op, GblBool* pResult) {  \
    type val = initialValue;    \
    GBL_API_BEGIN(GBL_HANDLE_INVALID);   \
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
    GBL_API_BEGIN(GBL_HANDLE_INVALID);
    GBL_API_VERIFY_POINTER(pVariant);
    GBL_API_VERIFY_POINTER(pString);
    GBL_API_VERIFY_ARG(op < GBL_VARIANT_OP_CMP_COUNT);
    GBL_API_VERIFY_POINTER(pResult);
    if(pVariant->type == GBL_VARIANT_TYPE_STRING) {
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
            .hCtx = GBL_HANDLE_INVALID,
            .pBuffer = (char*)pCStr,
            .size = pCStr? strlen(pCStr) : 0,
            .capacity = 0,
            .elementSize = 1,
            .stackSize = 0
        }
    };
    GBL_API_BEGIN(GBL_HANDLE_INVALID);
    GBL_API_CALL(gblVariantCompares(pVariant, &tempString, op, pResult));
    GBL_API_END();
}


GBL_INLINE GBL_API gblVariantComparev(const GblVariant* pLhs,
                          const GblVariant* pRhs,
                          GBL_VARIANT_OP_CMP_TYPE op,
                          GblBool* pResult)
{
    GBL_API_BEGIN(GBL_HANDLE_INVALID);
    GBL_API_VERIFY_POINTER(pLhs);
    GBL_API_VERIFY_POINTER(pRhs);
    GBL_API_VERIFY_POINTER(pResult);
    GBL_API_VERIFY_ARG(op <= GBL_VARIANT_OP_CMP_COUNT);

    switch(pLhs->type) {
    case GBL_VARIANT_TYPE_NIL:
        if(pRhs->type == GBL_VARIANT_TYPE_NIL) {
            if(op == GBL_VARIANT_OP_CMP_EQUAL) *pResult = GBL_TRUE;
            else *pResult = GBL_FALSE;
        } else {
            if(op == GBL_VARIANT_OP_CMP_NEQUAL) *pResult = GBL_TRUE;
            else *pResult = GBL_FALSE;
        }
    break;
    case GBL_VARIANT_TYPE_BOOL: {
        GblBool value = GBL_FALSE;
        GBL_API_CALL(gblVariantTob(pRhs, &value));
        GBL_API_CALL(gblVariantCompareb(pLhs, value, op, pResult));
        break;
    }
    case GBL_VARIANT_TYPE_INT: {
        GblInt value = 0;
        GBL_API_CALL(gblVariantToi(pRhs, &value));
        GBL_API_CALL(gblVariantComparei(pLhs, value, op, pResult));
        break;
    }
    case GBL_VARIANT_TYPE_FLOAT: {
        GblFloat value = 0.0f;
        GBL_API_CALL(gblVariantTof(pRhs, &value));
        GBL_API_CALL(gblVariantComparef(pLhs, value, op, pResult));
        break;
    }
    case GBL_VARIANT_TYPE_VOID_PTR: {
        void* pValue = 0;
        GBL_API_CALL(gblVariantTop(pRhs, &pValue));
        GBL_API_CALL(gblVariantComparep(pLhs, pValue, op, pResult));
        break;
    }
    case GBL_VARIANT_TYPE_STRING: {
        GblString tempStr;
        const GblString* pRStr = NULL;
        if(pRhs->type == GBL_VARIANT_TYPE_STRING) {
            pRStr = &pRhs->string;
        } else {
            GBL_API_CALL(gblStringConstruct(&tempStr, sizeof(GblString), GBL_HANDLE_INVALID, NULL));
            GBL_API_CALL(gblVariantTos(pRhs, &tempStr));
            pRStr = &tempStr;
        }
        GBL_API_CALL(gblVariantCompares(pLhs, pRStr, op, pResult));
        break;
    }
    default: GBL_API_VERIFY(GBL_FALSE, GBL_RESULT_ERROR_TYPE_MISMATCH);
    }

    GBL_API_END();
}


GBL_INLINE GBL_API gblVariantConstructDefault(GblVariant* pVariant) {
    GBL_API_BEGIN(GBL_HANDLE_INVALID);
    GBL_API_VERIFY_POINTER(pVariant);
    memset(pVariant, 0, sizeof(GblVariant));
    pVariant->type = GBL_VARIANT_TYPE_NIL;
    GBL_API_END();
}

#define GBL_VARIANT_CONSTRUCTOR_DEFINE(suffix, argType)                                         \
    GBL_INLINE GBL_API gblVariantConstruct##suffix(GblVariant* pVariant, argType value) {       \
        GBL_API_BEGIN(GBL_HANDLE_INVALID);                                                      \
        GBL_API_CALL(gblVariantConstructDefault(pVariant));                                     \
        GBL_API_CALL(gblVariantSet(pVariant, value));                                           \
        GBL_API_END();                                                                          \
    }


GBL_VARIANT_CONSTRUCTOR_DEFINE(b, GblBool)
GBL_VARIANT_CONSTRUCTOR_DEFINE(i, GblInt)
GBL_VARIANT_CONSTRUCTOR_DEFINE(f, GblFloat)
GBL_VARIANT_CONSTRUCTOR_DEFINE(p, void*)
GBL_VARIANT_CONSTRUCTOR_DEFINE(s, const GblString*)
GBL_VARIANT_CONSTRUCTOR_DEFINE(c, const char*)

GBL_INLINE GBL_API gblVariantConstructt(GblVariant* pVariant, GBL_VARIANT_TYPE type) {
    GBL_API_BEGIN(GBL_HANDLE_INVALID);
    GBL_API_VERIFY_POINTER(pVariant);
    GBL_API_VERIFY_ARG(type <= GBL_VARIANT_TYPE_BUILTIN_COUNT);
    GBL_API_CALL(gblVariantTypeSet(pVariant, type));
    if(type == GBL_VARIANT_TYPE_STRING) {
        GBL_API_CALL(gblVariantConstructc(pVariant, NULL));
    }
    GBL_API_END();
}

GBL_INLINE GBL_API gblVariantConstructCopy(GblVariant* pVariant, const GblVariant* pOther) {
    GBL_API_BEGIN(GBL_HANDLE_INVALID);
    GBL_API_VERIFY_POINTER(pVariant);
    GBL_API_VERIFY_POINTER(pOther);

    switch(pOther->type) {
    case GBL_VARIANT_TYPE_STRING: {
        const GblString* pString = NULL;
        GBL_API_CALL(gblVariantGets(pOther, &pString));
        GBL_API_CALL(gblVariantConstructs(pVariant, pString));
        break;
    }
    default: //handle all value type without giving a fuck!
        memcpy(pVariant, pOther, sizeof(GblVariant));
        break;
    }

    GBL_API_END();
}

GBL_INLINE GBL_API gblVariantConstructMove(GblVariant* pVariant, GblVariant* pOther) {
    GBL_API_BEGIN(GBL_HANDLE_INVALID);
    GBL_API_VERIFY_POINTER(pVariant);
    GBL_API_VERIFY_POINTER(pOther);

    memcpy(pVariant, pOther, sizeof(GblVariant));
    GBL_API_CALL(gblVariantConstructCopy(pVariant, pOther));
    //memset(pOther, 0, sizeof(GblVariant));
    pOther->type = GBL_VARIANT_TYPE_NIL;
    GBL_API_END();
}

GBL_INLINE GBL_API gblVariantCopy(GblVariant* pVariant, const GblVariant* pOther) {
    GBL_API_BEGIN(GBL_HANDLE_INVALID);
    GBL_API_CALL(gblVariantTypeSet(pVariant, GBL_VARIANT_TYPE_NIL));
    GBL_API_CALL(gblVariantConstructCopy(pVariant, pOther));
    GBL_API_END();
}


GBL_INLINE GBL_API gblVariantMovev(GblVariant* pVariant, GblVariant* pOther) {
    GBL_API_BEGIN(GBL_HANDLE_INVALID);
    GBL_API_CALL(gblVariantTypeSet(pVariant, GBL_VARIANT_TYPE_NIL));
    GBL_API_CALL(gblVariantConstructMove(pVariant, pOther));
    GBL_API_END();
}

//== MAKE ME TAKE A BUFFER INSTEAD ===
GBL_INLINE GBL_API gblVariantMovec(GblVariant* pVariant, char* pBuffer, GblSize capacity) {
    GBL_API_BEGIN(GBL_HANDLE_INVALID);
    GBL_API_CALL(gblVariantTypeSet(pVariant, GBL_VARIANT_TYPE_STRING));
    GBL_API_CALL(gblStringGive(&pVariant->string, pBuffer, capacity));
    GBL_API_END();
}

GBL_INLINE GBL_API gblVariantMoves(GblVariant* pVariant, GblString* pOther) {
    GblBool stack = GBL_FALSE;
    char* pOtherBuff = NULL;
    GblSize otherCapacity = 0;
    GBL_API_BEGIN(GBL_HANDLE_INVALID);
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
    GBL_API_BEGIN(GBL_HANDLE_INVALID);
    GBL_API_VERIFY_POINTER(pVariant);
    GBL_API_CALL(gblVariantTypeSet(pVariant, GBL_VARIANT_TYPE_NIL));
    GBL_API_END();
}


// add types to metatype shit (already handled via EnvTable)

//can call metamethods on tables and shit!


#if 0
//GBL_API gblVariantTypeString(GBL_VARIANT_TYPE type, const char* pBuff, GblSize* pSize);
//GBL_API gblVariantTypeClass(GBL_VARIANT_TYPE type, GBL_VARIANT_TYPE_CLASS* pClass);
//GBL_API gblVariantMetaType(GBL_VARIANT_TYPE type, const char* pTypeName, GblMetaType** ppMetaType);
//GBL_API gblVariantCompatibleTypes(GBL_VARIANT_TYPE type1, GBL_VARIANT_TYPE type2, GblBool* pResult);
//GBL_API gblVariantCommonType(const GblVariant* pLhs,
          //                   const GblVariant* pRhs,
          //                 GBL_VARIANT_TYPE* pType);

//GBL_API gblVariantCopy(GblVariant* pVariant, const GblVariant* pRhs);

GBL_DECLARE_ENUM(GBL_VARIANT_METAKEY) {
    GBL_VARIANT_METAKEY_INDEX,           // can be tables
    GBL_VARIANT_METAKEY_NEW_INDEX,       // can be tables
    GBL_VARIANT_METAKEY_CALL,

    GBL_VARIANT_METAKEY_PAIRS,
    GBL_VARIANT_METAKEY_IPAIRS,

    GBL_VARIANT_METAKEY_NAME,
    GBL_VARIANT_METAKEY_LENGTH,
    GBL_VARIANT_METAKEY_OP,
    GBL_VARIANT_METKEY_COUNT
};

GBL_DECLARE_ENUM(GBL_VARIANT_OP) {
    GBL_VARIANT_OP_CMP_EQUAL,
    GBL_VARIANT_OP_CMP_NEQUAL,
    GBL_VARIANT_OP_CMP_LESS,
    GBL_VARIANT_OP_CMP_LEQUAL,
    GBL_VARIANT_OP_CMP_GREATER,
    GBL_VARIANT_OP_CMP_GEQUAL,
    GBL_VARIANT_OP_UNARY_NEGATE,
    GBL_VARIANT_OP_UNARY_NOT,
    GBL_VARIANT_OP_BINARY_AND,
    GBL_VARIANT_OP_BINARY_OR,
    GBL_VARIANT_OP_BINARY_XOR,
    GBL_VARIANT_OP_BINARY_SHL,
    GBL_VARIANT_OP_BINARY_SHR,
    GBL_VARIANT_OP_BINARY_ADD,
    GBL_VARIANT_OP_BINARY_SUB,
    GBL_VARIANT_OP_BINARY_MUL,
    GBL_VARIANT_OP_BINARY_DIV,
    GBL_VARIANT_OP_BINARY_MOD,
    GBL_VARIANT_OP_BINARY_CAT,
    GBL_VARIANT_OP_COUNT
};

typedef GBL_RESULT (*GblVariantMetaConstructorFn)   (GblVariant*, const GblVariant*);
typedef GBL_RESULT (*GblVariantMetaDestructorFn)    (GblVariant*, const GblVariant*);
typedef GBL_RESULT (*GblVariantMetaValueSetFn)      (const GblVariant*, GblVariant*);
typedef GBL_RESULT (*GblVariantMetaValueGetFn)      (GblVariant*, const GblVariant*);
typedef GBL_RESULT (*GblVariantMetaMethodFn)        (GBL_VARIANT_METAKEY, GblVariant*, const GblVariant*, const GblVariant*);

GBL_DECLARE_ENUM(GBL_USERDATA_TYPE) {
    GBL_METATYPE_USERDATA_UNKNOWN
};

GBL_DECLARE_ENUM(GBL_VARIANT_METATYPE_FLAGS) {
    GBL_VARIANT_META_FLAGS_NONE,
    GBL_VARIANT_META_REFERENCE_TYPE,
    GBL_VARIANT_META_FLAGS_VARIANT_COMPATIBLE,
    GBL_VARIANT_META_FLAGS_CONTAINER,
    GBL_VARIANT_META_FLAGS_USERDATA,
    GBL_VARIANT_META_FLAGS_RESIZABLE
};

typedef struct GblVariantMetaType {
    GBL_VARIANT_TYPE                variantType;
    const char*                     pTypeName;
    GblEnum                         flags;
    GblSize                         sizeOf;
    GblSize                         alignOf;
    GblVariantMetaConstructorFn     pFnConstructor;
    GblVariantMetaDestructorFn      pFnDestructor;
    GblVariantMetaValueGetFn        pFnValueGet;
    GblVariantMetaValueSetFn        pFnValueSet;
    GblVariantMetaMethodFn          pFnMetaMethod;
} GblVariantMetaType;

typedef struct GblMetaType {
    GBL_VARIANT_TYPE variantType;
} GblMetaType;


typedef GBL_RESULT (*GblFunction)(GblContext, GblVariant);

#define GBL_META_OBJECT_TYPE_TABLE (                                                            \
        ( GBL_OBJECT_TYPE, ObjectType, "Enum for GblObject runtime types"),                     \
        (                                                                                       \
            (GBL_OBJECT_TYPE_OBJECT,            0x0,    Object,         "Object"),              \
            (GBL_OBJECT_TYPE_HANDLE,            0x1,    Handle,         "Handle"),              \
            (GBL_OBJECT_TYPE_CONTEXT,           0x2,    Context,        "Context"),             \
            (GBL_OBJECT_TYPE_BUILTIN_COUNT,     0x3,    BuiltinCount,   "# of Builtin Types"),  \
            (GBL_OBJECT_TYPE_USER,              0x4,    User,           "First User Type")      \
        )                                                                                       \
    )

GBL_ENUM_TABLE_DECLARE(GBL_META_OBJECT_TYPE_TABLE);


#define GBL_META_OBJECT_TYPE_TABLE (                                                            \

GBL_DECLARE_ENUM(GBL_VARIANT_TYPE_CLASS) {
    GBL_VARIANT_TYPE_CLASS_NIL,
    GBL_VARIANT_TYPE_CLASS_BOOL,
    GBL_VARIANT_TYPE_CLASS_NUMBER,
    GBL_VARIANT_TYPE_CLASS_STRING,
    GBL_VARIANT_TYPE_CLASS_TABLE,
    GBL_VARIANT_TYPE_CLASS_PTR,
    GBL_VARIANT_TYPE_CLASS_COUNT
};



#endif


#undef CSELF
#undef SELF


GBL_DECLS_END








#endif // GBLBAL_VARIANT__H
