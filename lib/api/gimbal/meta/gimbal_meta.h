#ifndef GIMBAL_META_H
#define GIMBAL_META_H

#include "../core/gimbal_api_generators.h"
#include "../types/gimbal_result.h"

#ifdef __cplusplus
extern "C" {
#endif

#define GBL_META_VARIANT_TYPE_TABLE (                                                           \
        ( GBL_VARIANT_TYPE, VariantType, "Enum for GblVariant builtin types", gblVariantTypeString), \
        (                                                                                       \
            (GBL_VARIANT_TYPE_NIL,              0x0,    Nil,            "Nil"),                 \
            (GBL_VARIANT_TYPE_BOOL,             0x1,    Bool,           "Bool"),                \
            (GBL_VARIANT_TYPE_INT,              0x2,    Int,            "Integer"),             \
            (GBL_VARIANT_TYPE_FLOAT,            0x3,    Float,          "Float"),               \
            (GBL_VARIANT_TYPE_VOID_PTR,         0x4,    VoidPtr,        "Void Pointer"),        \
            (GBL_VARIANT_TYPE_STRING,           0x5,    String,         "String"),              \
            (GBL_VARIANT_TYPE_OBJECT,           0x6,    Object,         "Object"),              \
            (GBL_VARIANT_TYPE_BUILTIN_COUNT,    0x7,    BuiltinCount,   "Builtin Count")        \
        )                                                                                       \
    )

GBL_ENUM_TABLE_DECLARE(GBL_META_VARIANT_TYPE_TABLE);


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


#define GBL_META_TYPE_VTABLE_TABLE (                                                               \
        ( GBL_META_TYPE_VTABLE, MetaTypeVtable, "Vtable for GblMetaTypes", gblMetaTypeVTableString),     \
        (                                                                                          \
            (GBL_META_TYPE_VTABLE_TO,           0x0,    To,          "To"),                        \
            (GBL_META_TYPE_VTABLE_COMPARE,      0x1,    Compare,     "Compare"),                   \
            (GBL_META_TYPE_VTABLE_DESTRUCTOR,   0x2,    Destructor,  "Destructor"),                     \
            (GBL_META_TYPE_VTABLE_COUNT,        0x3,    BuiltinCount, "Builtin virtual method count")  \
        )                                                                                          \
    )

GBL_ENUM_TABLE_DECLARE(GBL_META_TYPE_VTABLE_TABLE);

GBL_FORWARD_DECLARE_STRUCT(GblVariant);


typedef GBL_RESULT (*GblMetaTypeDestructorFn)(GblVariant*);
typedef GBL_RESULT (*GblMetaTypeToFn)(const GblVariant*, GblVariant*);
typedef GBL_RESULT (*GblMetaTypeCompareFn)(const GblVariant*, GblVariant*, GBL_VARIANT_OP_CMP_TYPE, GblBool*);

typedef void* GblVirtualFn;

typedef struct GblVTableEntry
{
   int d;  /* d and i are used in multiple inheritance */
   int i;
   GblVirtualFn pFunc;
} GblVTableEntry;


typedef struct GblMetaType {
    GblVTableEntry* pVTable;
    const char* pTypeName;
    GBL_VARIANT_TYPE variantType;
} GblMetaType;


#define GBL_META_OBJECT_TYPE_TABLE (                                                            \
        ( GBL_OBJECT_TYPE, ObjectType, "Enum for GblObject runtime types", gblMetaObjectTypeString),                     \
        (                                                                                       \
            (GBL_OBJECT_TYPE_OBJECT,            0x0,    Object,         "Object"),              \
            (GBL_OBJECT_TYPE_HANDLE,            0x1,    Handle,         "Handle"),              \
            (GBL_OBJECT_TYPE_CONTEXT,           0x2,    Context,        "Context"),             \
            (GBL_OBJECT_TYPE_BUILTIN_COUNT,     0x3,    BuiltinCount,   "# of Builtin Types"),  \
            (GBL_OBJECT_TYPE_USER,              0x4,    User,           "First User Type")      \
        )                                                                                       \
    )

GBL_ENUM_TABLE_DECLARE(GBL_META_OBJECT_TYPE_TABLE);


#ifdef __cplusplus
}
#endif

#endif // GIMBAL_META_H
