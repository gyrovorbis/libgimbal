/*! \file
 *  \brief Helper defines for struct, enum, flags, handle delcarations
 *  \ingroup core
 *  \sa gimbal_macro_utilities.h
 */

#ifndef GIMBAL_DECLS_H
#define GIMBAL_DECLS_H

#include "gimbal_typedefs.h"
#include "../preprocessor/gimbal_macro_utils.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
#   define GBL_DECLS_BEGIN extern "C" {
#   define GBL_DECLS_END   }
#else
#   define GBL_DECLS_BEGIN
#   define GBL_DECLS_END
#endif

#define GBL_CLASS_IMPL_INTERFACE(iface) \
    iface##Class iface##Impl;

#define GBL_CLASS_DERIVE_N(instance, baseKlass, ...) \
    GBL_CLASS_DERIVE_2(instance, baseKlass) \
    GBL_MAP(GBL_CLASS_IMPL_INTERFACE, __VA_ARGS__)

#define GBL_CLASS_DERIVE_2(instance, baseInstance)  \
    struct instance##Class;                         \
    typedef struct instance##Class instance##Class; \
    struct instance##Class {                        \
        baseInstance##Class   base;

#define GBL_CLASS_DERIVE_1(instance) \
    GBL_CLASS_DERIVE_2(instance, Gbl)

#define GBL_CLASS_DERIVE(...)   \
    GBL_VA_OVERLOAD_CALL(GBL_CLASS_DERIVE, GBL_VA_OVERLOAD_SUFFIXER_2_N, __VA_ARGS__)

#define GBL_CLASS_BASE_N(instance, ...)             \
    GBL_CLASS_DERIVE_N(instance, Gbl, __VA_ARGS__)

#define GBL_CLASS_BASE_1(instance) \
    GBL_CLASS_DERIVE_2(instance, Gbl)

#define GBL_CLASS_BASE(...) \
    GBL_VA_OVERLOAD_CALL(GBL_CLASS_BASE, GBL_VA_OVERLOAD_SUFFIXER_1_N, __VA_ARGS__)

#define GBL_CLASS_BASE_EMPTY(...) \
    GBL_CLASS_BASE(__VA_ARGS__)   \
    GBL_CLASS_END

#define GBL_CLASS_END };

#define GBL_CLASS_DERIVE_EMPTY(...) \
    GBL_CLASS_DERIVE(__VA_ARGS__)   \
    GBL_CLASS_END

#define GBL_INTERFACE_DERIVE_N(instance, klass, baseClass, ...) \
    GBL_INTERFACE_DERIVE_3(instance, klass, baseClass)          \
    GBL_MAP(GBL_CLASS_IMPL_INTERFACE, __VA_ARGS__)

#define GBL_INTERFACE_DERIVE_3(instance, klass, baseKlass)  \
    struct instance;                                        \
    typedef struct instance instance;                       \
    struct klass;                                           \
    typedef struct klass klass;                             \
    struct klass {                                          \
        baseKlass base;

#define GBL_INTERFACE_DERIVE_2(instance, klass) \
    GBL_INTERFACE_DERIVE_3(instance, klass, GblInterface)

#define GBL_INTERFACE_DERIVE_1(instance)    \
    GBL_INTERFACE_DERIVE_2(instance, instance##Class)

#define GBL_INTERFACE_DERIVE(...)   \
    GBL_VA_OVERLOAD_CALL(GBL_INTERFACE_DERIVE, GBL_VA_OVERLOAD_SUFFIXER_3_N, __VA_ARGS__)

#define GBL_INTERFACE_END };

#define GBL_INSTANCE_DERIVE(derivedInstance, baseInstance)  \
    struct derivedInstance;                                 \
    typedef struct derivedInstance derivedInstance;         \
    struct derivedInstance {                                \
        union {                                             \
            derivedInstance##Class* pClass;                 \
            baseInstance            base;                   \
        };

#define GBL_INSTANCE_BASE(instance)     \
    struct instance;                    \
    typedef struct instance instance;   \
    struct instance {                   \
        union {                         \
            instance##Class* pClass;    \
            GblInstance      base;      \
        };

#define GBL_INSTANCE_BASE_EMPTY(instance)   \
    GBL_INSTANCE_BASE(instance)             \
    GBL_INSTANCE_END

#define GBL_INSTANCE_END };

#define GBL_INSTANCE_DERIVE_EMPTY(derived, base) \
    GBL_INSTANCE_DERIVE(derived, base)           \
    GBL_INSTANCE_END

#define GBL_TYPEOF(instanceStruct)  \
    GBL_GLUE(instanceStruct, _type())

#define GBL_DECLARE_UNION(S)   \
    union S;                   \
    typedef union S S;         \
    union S

#define GBL_DECLARE_STRUCT(S)   \
    struct S;                   \
    typedef struct S S;         \
    struct S

#define GBL_DECLARE_STRUCT_PRIVATE(S)   \
    struct S##_;                        \
    typedef struct S##_ S##_;           \
    struct S##_

#define GBL_FORWARD_DECLARE_STRUCT(S)  \
    struct S;                          \
    typedef struct S S

#define GBL_FORWARD_DECLARE_STRUCT_PRIVATE(S)   \
    struct S##_;                                \
    typedef struct S##_ S

#define GBL_FORWARD_DECLARE_TYPE(t) \
    GBL_TYPEOF(t)

#ifndef __cplusplus
#   define GBL_DECLARE_ENUM(E)    \
        typedef GblEnum E;        \
        enum E
#else
#   define GBL_DECLARE_ENUM(E)     \
        enum E : GblEnum
#endif

#ifndef __cplusplus
#   define GBL_DECLARE_FLAGS(F)    \
        typedef GblFlags F;        \
        enum F
#else
#   define GBL_DECLARE_FLAGS(F)    \
        enum F: GblFlags
#endif

#define GBL_FORWARD_DECLARE_ENUM(E) \
    typedef GBL_ENUM E

#define GBL_FORWARD_DECLARE_FLAGS(F) \
    typedef GBL_FLAGS F

#define GBL_DECLARE_OPAQUE(S)   \
    struct S##_;                \
    typedef struct S##_* S

#define GBL_PRIVATE()       struct {

#define GBL_PRIVATE_END }   private_;

#define GBL_PRIV(a)         (a).private_
#define GBL_PRIV_REF(a)     (a)->private_

#define GBL_VSELF           GBL_SELF_TYPE self

#define GBL_KLASS           GBL_GLUE(GBL_SELF_TYPE, Class)* pSelf
#define GBL_CKLASS          const GBL_KLASS

#define GBL_SELF            GBL_SELF_TYPE* pSelf
#define GBL_CSELF           const GBL_SELF

// sizeof(emptyStruct) is 1 in Cpp, 0 in C!
#define GBL_DEFINE_EMPTY_STRUCT(S)  \
    typedef struct S {              \
        char cppCompat;             \
    } S

#define GBL_UNUSED_(a) \
    (void)(a)

#define GBL_UNUSED(...) GBL_MAP_LIST(GBL_UNUSED_, __VA_ARGS__)

#ifdef GBL_CTX_SHARED_LIB
#   ifdef GBL_CTX_EXPORTS
#       define GBL_EXPORT GBL_EXPORT_SHARED
#   else
#       define GBL_EXPORT GBL_IMPORT_SHARED
#   endif
#else
#   define GBL_EXPORT
#endif

// ===== MISCELLANEOUS META TUPLES FOR MACRO GENERATORS ====
#define GBL_DECL_VAR_TYPE(type, ...) \
    type
#define GBL_DECL_VAR_NAME(type, name) \
    name

#define GBL_DECL_VAR(type, name) \
  GBL_DECL_VAR_TYPE(type, name) GBL_DECL_VAR_NAME(type, name)

#define GBL_DECL_VAR_PAIR_TYPE(pair) \
    GBL_DECL_VAR_TYPE pair

#define GBL_DECL_VAR_PAIR_NAME(pair) \
    GBL_DECL_VAR_NAME pair

#define GBL_DECL_VAR_PAIR(pair) \
  GBL_DECL_VAR pair


// === ENUMERATION AUTOGENERATION ===========

#define GBL_ENUM_TABLE_DECL_ENUM(cName, value, name, string) \
    cName = value,


#define GBL_ENUM_TABLE_DECLARE(table) \
    GBL_DECLARE_ENUM(GBL_META_ENUM_TYPE_PROPERTY(table, CNAME)) { \
        GBL_MAP_TUPLES(GBL_ENUM_TABLE_DECL_ENUM,  GBL_MAP_TUPLES(GBL_EVAL, GBL_META_ENUM_TUPLE_VALUE_ARRAY table)) \
    }; \
    GBL_ENUM_TABLE_DECLARE_STRINGIFIER(table)

#define GBL_ENUM_TABLE_RETURN_STRING(cName, value, name, string) \
    case value: return string;

#define GBL_ENUM_TABLE_TO_STRING(table, value) \
    switch(value) { \
        GBL_MAP_TUPLES(GBL_ENUM_TABLE_RETURN_STRING, GBL_MAP_TUPLES(GBL_EVAL, GBL_META_ENUM_TUPLE_VALUE_ARRAY table)) \
    }

#define GBL_ENUM_TABLE_DECLARE_STRINGIFIER(table) \
     GBL_INLINE GBL_CONSTEXPR const char* GBL_EVAL(GBL_META_ENUM_TYPE_PROPERTY(table, STRINGIFIER))(GBL_EVAL(GBL_META_ENUM_TYPE_PROPERTY(table, CNAME)) value) { \
        GBL_EVAL(GBL_ENUM_TABLE_TO_STRING(table, value)); \
        return ""; \
    }

#define GBL_META_ENUM_TUPLE(tuple)

#define GBL_META_ENUM_TUPLE_TYPE_INFO(typeInfo, valueTable) typeInfo

#define GBL_META_ENUM_TUPLE_TYPE_INFO_PROPERTY_CNAME(cName, name, description, stringifier) cName
#define GBL_META_ENUM_TUPLE_TYPE_INFO_PROPERTY_NAME(cName, name, description, stringifier) name
#define GBL_META_ENUM_TUPLE_TYPE_INFO_PROPERTY_DESCRIPTION(cName, name, description, stringifier) description
#define GBL_META_ENUM_TUPLE_TYPE_INFO_PROPERTY_STRINGIFIER(cName, name, description, stringifier) stringifier

#define GBL_META_ENUM_TUPLE_VALUE_ARRAY(typeInfo, valueArray) valueArray

#define GBL_META_ENUM_TUPLE_VALUE_ARRAY_TUPLE_VALUE(...) GBL_EVAL __VA_ARGS__

#define GBL_META_ENUM_TUPLE_VALUE_PROPERTY_CNAME(cName, value, name, string) cName
#define GBL_META_ENUM_TUPLE_VALUE_PROPERTY_NAME(cName, value, name, string) name
#define GBL_META_ENUM_TUPLE_VALUE_PROPERTY_VALUE(cName, value, name, string) value
#define GBL_META_ENUM_TUPLE_VALUE_PROPERTY_STRING(cName, value, name, string) string

#define GBL_META_ENUM_TYPE_PROPERTY(table, postfix) GBL_META_ENUM_TUPLE_TYPE_INFO_PROPERTY_##postfix GBL_META_ENUM_TUPLE_TYPE_INFO table


#ifdef __cplusplus
}
#endif

#endif // GIMBAL_DECLS_H
