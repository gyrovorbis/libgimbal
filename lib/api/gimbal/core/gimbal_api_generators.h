/*! \file
 *  \brief Helper defines for struct, enum, flags, handle delcarations
 *  \ingroup preprocessor
 *  \sa gimbal_macro_utilities.h
 */

#ifndef GIMBAL_API_GENERATORS_H
#define GIMBAL_API_GENERATORS_H

#include <stdarg.h>
#include <stdio.h>
#include <stdint.h>
#include <limits.h>
#include <gimbal/core/gimbal_config.h>
#include "../preprocessor/gimbal_macro_sequences.h"
#include "../preprocessor/gimbal_macro_utils.h"
#include "../preprocessor/gimbal_macro_composition.h"

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
    iface iface##Impl;

#define GBL_CLASS_DERIVE_N(derivedKlass, baseKlass, ...) \
    GBL_CLASS_DERIVE_2(derivedKlass, baseKlass) \
    GBL_MAP(GBL_CLASS_IMPL_INTERFACE, __VA_ARGS__)

#define GBL_CLASS_DERIVE_2(derivedKlass, baseKlass) \
    struct derivedKlass;                            \
    typedef struct derivedKlass derivedKlass;       \
    struct derivedKlass {                           \
        baseKlass   base;

#define GBL_CLASS_DERIVE_1(derivedKlass) \
    GBL_CLASS_DERIVE_2(derivedKlass, GblClass)

#define GBL_CLASS_DERIVE(...)   \
    GBL_VA_OVERLOAD_SELECT(GBL_CLASS_DERIVE, GBL_VA_OVERLOAD_SUFFIXER_2_N, __VA_ARGS__)(__VA_ARGS__)

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
    GBL_INTERFACE_DERIVE_2(instance, instance##IFace)

#define GBL_INTERFACE_DERIVE(...)   \
    GBL_VA_OVERLOAD_SELECT(GBL_INTERFACE_DERIVE, GBL_VA_OVERLOAD_SUFFIXER_3_N, __VA_ARGS__)(__VA_ARGS__)

#define GBL_INTERFACE_END };


#define GBL_INSTANCE_DERIVE_3(derivedInstance, baseInstance, klass) \
    struct derivedInstance;                                         \
    typedef struct derivedInstance derivedInstance;                 \
    struct derivedInstance {                                        \
        union {                                                     \
            klass*          pClass;                                 \
            baseInstance    base;                                   \
        };

#define GBL_INSTANCE_DERIVE_2(derivedInstance, baseInstance) \
    GBL_INSTANCE_DERIVE_3(derivedInstance, baseInstance, derivedInstance##Class)

#define GBL_INSTANCE_DERIVE_1(derivedInstance) \
    GBL_INSTANCE_DERIVE_2(derivedInstance, GblInstance, GblClass)

#define GBL_INSTANCE_DERIVE(...) \
    GBL_VA_OVERLOAD_SELECT(GBL_INSTANCE_DERIVE, GBL_VA_OVERLOAD_SUFFIXER_3_N, __VA_ARGS__)(__VA_ARGS__)

#define GBL_INSTANCE_END };

#define GBL_INSTANCE_DERIVE_EMPTY(...) \
    GBL_INSTANCE_DERIVE(__VA_ARGS__)   \
    GBL_INSTANCE_END

typedef struct GblPropertyInfo_ {
    const char* pName;
    uintptr_t     typeId;
    uint32_t    flags;
} GblPropertyInfo_;

#if 1
#if 0
#define GBL_PROPERTY_FLAGS__(flag) \
    |GBL_PROPERTY_FLAG_##flag

#define GBL_PROPERTY_FLAGS_(...) \
    GBL_MAP_TUPLES(GBL_PROPERTY_FLAGS__, __VA_ARGS__)
#endif


#define GBL_PROPERTY_FLAG__(suffix)              \
    GBL_PROPERTY_FLAG_##suffix |

#define GBL_PROPERTY_FLAX_(...)              \
    GBL_MAP(GBL_PROPERTY_FLAG__, __VA_ARGS__)

#define GBL_PROPERTY_FLAGS_MASK_(...)            \
    GBL_MAP_TUPLES(GBL_PROPERTY_FLAX_, __VA_ARGS__)0

#define GBL_PROPERTY_INFO__(name, type, flgs)   \
    { .pName = name, .typeId = type, .flags = GBL_PROPERTY_FLAGS_MASK_(flgs) },

#define GBL_PROPERTY_ITEM_(...) \
   GBL_MAP_TUPLES(GBL_PROPERTY_INFO__, __VA_ARGS__)

#define GBL_PROPERTIES(object, ...) \
   GBL_INLINE void object##_propertyInfo(GblSize size, GblPropertyInfo_*pInfoOut) {    \
        const GblPropertyInfo_ infos[] = {   \
            GBL_PROPERTY_ITEM_(__VA_ARGS__)   \
 }; memcpy(pInfoOut, &infos[size], sizeof(GblPropertyInfo_)); }

#define GBL_PROPERTIES_END GBL_LPAREN
#else
#define GBL_PROPERTY_FLAGS__(flag) \
    |GBL_PROPERTY_FLAG_##flag

#define GBL_PROPERTY_FLAGS_(...) \
    GBL_MAP_TUPLES(GBL_PROPERTY_FLAGS__, __VA_ARGS__)

#define GBL_PROPERTY_INFO__(name, type, flgs)   \
    { .pName = name, .typeId = type, .flags = flgs },

#define GBL_PROPERTY_ITEM_(...) \
    GBL_MAP(GBL_PROPERTY_INFO__, (__VA_ARGS__))

#define GBL_PROPERTY_ITEM__()   \
  GBL_PROPERTY_ITEM_

#define GBL_LPAREN (
#define GBL_RPAREN )

#define GBL_PROPERTIES(object) \
   GBL_INLINE void object##_propertyInfo(GblSize size, GblPropertyInfo_*pInfoOut) {    \
        const GblPropertyInfo_ infos[] = {   \
        GBL_PROPERTY_ITEM_

#define GBL_PROPERTIES_END GBL_RPAREN) }; memcpy(pInfoOut, &infos[size], sizeof(GblPropertyInfo_)); }
#endif

#define GBL_DECLARE_STRUCT_PUBLIC(S)    \
    struct S;                           \
    typedef struct S S;                 \
    struct S

#define GBL_FORWARD_DECLARE_STRUCT(S)  \
    struct S;                          \
    typedef struct S S

#define GBL_FORWARD_DECLARE_STRUCT_PRIVATE(S)   \
    struct S##_;                                \
    typedef struct S##_ S

#ifndef __cplusplus
#   define GBL_DECLARE_ENUM(E)     \
        typedef GBL_ENUM E;        \
        enum E
#else
#   define GBL_DECLARE_ENUM(E)      \
        enum E : GBL_ENUM
#endif

#ifndef __cplusplus
#   define GBL_DECLARE_FLAGS(F)    \
        typedef GBL_FLAGS F;       \
        enum F
#else
#   define GBL_DECLARE_FLAGS(F)    \
        enum F: GBL_FLAGS
#endif

#define GBL_FORWARD_DECLARE_ENUM(E) \
    typedef GBL_ENUM E

#define GBL_FORWARD_DECLARE_FLAGS(F) \
    typedef GBL_FLAGS F

#define GBL_HANDLE void*

#define GBL_DECLARE_HANDLE(S) \
    typedef GBL_HANDLE S;

#define GBL_DECLARE_OPAQUE(S)   \
    struct S##_;                \
    typedef struct S##_* S


#define GBL_PRIVATE()      struct {

#define GBL_PRIVATE_END }   private_;

#define GBL_PRIV(a)         (a).private_
#define GBL_PRIV_REF(a)     (a)->private_

//sizeof(emptyStruct) is 1 in Cpp, 0 in C!
#define GBL_DEFINE_EMPTY_STRUCT(S)  \
    typedef struct S {              \
        char cppCompat;             \
    } S

#define GBL_UNUSED_(a) \
    (void)(a)

#define GBL_UNUSED(...) GBL_MAP_LIST(GBL_UNUSED_, __VA_ARGS__)

#ifdef GBL_API_SHARED_LIB
#   ifdef GBL_API_EXPORTS
#       define GBL_EXPORT GBL_EXPORT_SHARED
#   else
#       define GBL_EXPORT GBL_IMPORT_SHARED
#   endif
#else
#   define GBL_EXPORT
#endif

#define GBL_API GBL_RESULT GBL_EXPORT

#define GBL_API_INLINE_HELPER(retType, suffix, args, apiCall)                                                   \
    GBL_MAYBE_UNUSED GBL_API_INLINE(suffix, GBL_EVAL(GBL_MAP_TUPLES(GBL_DECL_VAR_TYPE, retType)_, GBL_EVAL args) {        \
        GBL_API_INLINE_BEGIN(GBL_EVAL(GBL_MAP_TUPLES(GBL_DECL_VAR_NAME, retType));                                       \
        GBL_STMT_START apiCall                                                                                  \
        GBL_STMT_END;                                                                                           \
        GBL_API_INLINE_END();                                                                                   \
        GBL_API_INLINE_RETURN();                                                                                \
    }

//========== CLEAN THIS ENUM SHIT UP AND MAKE IT IN THE FORM OF ==============

//GBL_API_ENUM_GENERATE()

// ===== MISC META SHIT TO CLEAN LATER ====
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


// === ENUM SHIT CLEAN ME LATER!!!===========

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


//GBL_API_ENUM_TUPLE

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


// @TODO: ADD META TUPLE SUPPORT FOR FLAGS, INCLUDING STRINGIFICATION AND VALIDATION!!

#ifdef __cplusplus
}
#endif

#endif // GIMBAL_API_GENERATORS_H
