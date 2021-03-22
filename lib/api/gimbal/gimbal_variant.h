#ifndef GBLBAL_VARIANT_H
#define GBLBAL_VARIANT_H

#include "gimbal_types.h"
#include <stdint.h>

GBL_FORWARD_DECLARE_STRUCT(GblTable);

// 1 - built-in stack compatible value types
// 2 - basic fundamental proxy meta types (GblStringView, GblString, GblTable, GblError, etc)
// 3 - userdata/table proxy metatypes + references

typedef GBL_RESULT (*GblFunction)(GblContext*);

#if 0
typedef enum GblMetaObject {
    GblMetaType* pType;
    void* pUserdata;
} GblMetaObject;
#endif

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
    GBL_VARIANT_TYPE_COUNT
};

typedef union GblVariantValue {
    GblInt          integer;
    GblReal         real;
    GblSize         size;
    GblFunction     func;
    const char*     pString;
    void*           pVoidPtr;

#ifdef GBLBAL_EXT_TABLES
    GblTable        table; metaType
#endif
} GblVariantValue;


/* "Container" types
 * a. generically iterable/copyable/etc
 * b. contains internal element type
 * c. all generically iterable with next semantics and shit
 * d. size
 */

/* First "custom" types:
a. GblTable
b. GblString
c. GblRef
d. GBL_RESULT (int, string, bool/nil conversions)
*/
typedef struct Gblariant {
    GblVariantValue value;
    GBL_VARIANT_TYPE type;
} GblVariant;

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
    GBL_VARIANT_OP_BINARY_CAT,  //appending tables and strings!!
    GBL_VARIANT_OP_COMPARISON_COUNT
};

typedef GBL_RESULT (*GblMetaTypeVTableVariantGet)(const GblVariant*, GblVariant*);
typedef GBL_RESULT (*GblMetaTypeVTableVariantSet)(GblVariant*, const GblVariant*);
typedef GBL_RESULT (*GblMetaTypeVTableHash)(const GblVariant*, GblHash*);
typedef GBL_RESULT (*GblMetaTypeVTableComp)(const GblVariant*, const GblVariant*, GBL_VARIANT_OP, GblBool*);

GBL_DECLARE_ENUM(GBL_METATYPE_FLAGS) {
    GBL_METATYPE_FLAGS_NONE,
    GBL_METATYPE_FLAGS_VARIANT_COMPATIBLE,
    GBL_METATYPE_FLAGS_CONTAINER,
    GBL_METATYPE_FLAGS_USERDATA,
    GBL_METATYPE_FLAGS_RESIZABLE
};

typedef struct GblMetaType {
    const char* pTypeName;
    const char* pDescription;
    GblMetaTypeVTableVariantGet     pGet;
    GblMetaTypeVTableVariantSet     pSet;
    GblMetaTypeVTableComp           pCompare;
    //PROPERTIES
    //METHODS
} GblMetaType;

// Top-level Utilities
GBL_API gblVariantType(const GblVariant* pVariant, GBL_VARIANT_TYPE* pType);
GBL_API gblVariantTypeString(GBL_VARIANT_TYPE type, const char* pBuff, GblSize* pSize);
GBL_API gblVariantTypeClass(GBL_VARIANT_TYPE type, GBL_VARIANT_TYPE_CLASS* pClass);
GBL_API gblVariantMetaType(GBL_VARIANT_TYPE type, GblMetaType** ppMetaType);
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











#endif // GBLBAL_PROPERTIES_H
