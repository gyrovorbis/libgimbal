﻿#ifndef GIMBAL_VARIANT_H
#define GIMBAL_VARIANT_H

#include "gimbal_types.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

GBL_DECLARE_OPAQUE(GblContext);
GBL_FORWARD_DECLARE_STRUCT(GblTable);
GBL_FORWARD_DECLARE_STRUCT(GblVariant);
// 1 - built-in stack compatible value types
// 2 - basic fundamental proxy meta types (GblStringView, GblString, GblTable, GblError, etc)
// 3 - userdata/table proxy metatypes + references


typedef GBL_RESULT (*GblFunction)(GblContext, GblVariant);

GBL_DECLARE_ENUM(GBL_VARIANT_TYPE_CLASS) {
    GBL_VARIANT_TYPE_CLASS_NIL,
    GBL_VARIANT_TYPE_CLASS_BOOL,
    GBL_VARIANT_TYPE_CLASS_NUMBER,
    GBL_VARIANT_TYPE_CLASS_STRING,
    GBL_VARIANT_TYPE_CLASS_TABLE,
    GBL_VARIANT_TYPE_CLASS_PTR,
    GBL_VARIANT_TYPE_CLASS_COUNT
};

GBL_DECLARE_ENUM(GBL_VARIANT_TYPE) {
    GBL_VARIANT_TYPE_NIL,
    GBL_VARIANT_TYPE_BOOL,
    GBL_VARIANT_TYPE_INT,
    GBL_VARIANT_TYPE_FLOAT,
    GBL_VARIANT_TYPE_STRING,
    GBL_VARIANT_TYPE_TABLE,
    GBL_VARIANT_TYPE_FUNCTION,
    GBL_VARIANT_TYPE_VOID_PTR,
    GBL_VARIANT_TYPE_USERDATA,
    GBL_VARIANT_TYPE_HANDLE,
    GBL_VARIANT_TYPE_COUNT
};

typedef union GblVariantValue {
    GblInt          integer;
    GblReal         real;
    GblSize         size;
    GblFunction     func;
    const char*     pString;
    void*           pVoidPtr;
    uintptr_t       uintptr;
    uint64_t        uint64;
} GblVariantValue;

typedef struct GblVariant {
    GblVariantValue     value;
    GBL_VARIANT_TYPE    type;
} GblVariant;



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

// GblVariantMetaType <=> QMetaType
// GblVariant <=> QMeta

// Top-level Utilities
GBL_API gblVariantType(const GblVariant* pVariant, GBL_VARIANT_TYPE* pType);
GBL_API gblVariantTypeString(GBL_VARIANT_TYPE type, const char* pBuff, GblSize* pSize);
GBL_API gblVariantTypeClass(GBL_VARIANT_TYPE type, GBL_VARIANT_TYPE_CLASS* pClass);

GBL_API gblVariantMetaType(GBL_VARIANT_TYPE type, const char* pTypeName, GblMetaType** ppMetaType);
GBL_API gblVariantCompatibleTypes(GBL_VARIANT_TYPE type1, GBL_VARIANT_TYPE type2, GblBool* pResult);
GBL_API gblVariantCommonType(const GblVariant* pLhs,
                             const GblVariant* pRhs,
                             GBL_VARIANT_TYPE* pType);
GBL_API gblVariantIsNil(const GblVariant* pVariant,   GblBool*    pValue);
GBL_API gblVariantCopy(GblVariant* pVariant, const GblVariant* pRhs);

// Get (Actual) Value
GBL_API gblVariantGetb(const GblVariant* pVariant,    GblBool*     pValue);
GBL_API gblVariantGets(const GblVariant* pVariant,    const char** ppValue);
GBL_API gblVariantGetv(const GblVariant* pVariant,    void**       pValue);
GBL_API gblVariantGett(const GblVariant* pVariant,    GblTable**   ppTable);
GBL_API gblVariantGeti(const GblVariant* pVariant,    GblInt*      pValue);
GBL_API gblVariantGetf(const GblVariant* pVariant,    GblReal*    pValue);

// Set Value
GBL_API gblVariantSetNil(GblVariant* pVariant);
GBL_API gblVariantSetb(GblVariant* pVariant,    GblBool     value);
GBL_API gblVariantSets(GblVariant* pVariant,    const char* pValue);
GBL_API gblVariantSetv(GblVariant* pVariant,    void*       pValue);
GBL_API gblVariantSett(GblVariant* pVariant,    const GblTable* pTable);
GBL_API gblVariantSeti(GblVariant* pVariant,    GblInt      value);
GBL_API gblVariantSetf(GblVariant* pVariant,    GblReal       value);

// (MAYBE) Convert Value
GBL_API gblVariantNil(const GblVariant* pVariant,  GblBool*    pValue);
GBL_API gblVariantb(const GblVariant* pVariant,    GblBool*    pValue);
GBL_API gblVariants(const GblVariant* pVariant,    const char* pBuffer, GblSize* pSize);
GBL_API gblVariantv(const GblVariant* pVariant,    void**       pValue);
GBL_API gblVarianti(const GblVariant* pVariant,    GblInt*     pValue);
GBL_API gblVariantf(const GblVariant* pVariant,    GblReal*    pValue);

GBL_API gblVariantCompare(const GblVariant* pLhs,
                          const GblVariant* pRhs,
                          GBL_VARIANT_OP compOp,
                          GblBool* pResult); //does strcmp too?

// add types to metatype shit (already handled via EnvTable)

//can call metamethods on tables and shit!


#ifdef __cplusplus
}
#endif








#endif // GBLBAL_PROPERTIES_H