#ifndef GIMBAL_API_GENERATORS_H
#define GIMBAL_API_GENERATORS_H

#include <stdarg.h>
#include <stdio.h>
#include <stdint.h>
#include <limits.h>
#include <gimbal/core/gimbal_config.h>
#include "../preprocessor/gimbal_macro_utils.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
#   define GBL_BEGIN_DECLS  extern "C" {
#   define GBL_END_DECLS    }
#else
#   define GBL_BEGIN_DECLS
#   define GBL_END_DECLS
#endif

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

//sizeof(emptyStruct) is 1 in Cpp, 0 in C!
#define GBL_DEFINE_EMPTY_STRUCT(S)  \
    typedef struct S {              \
        char cppCompat;             \
    } S

#define GBL_UNUSED(a) \
    (void)(a)


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
    GBL_DECLARE_ENUM(GBL_EVAL(GBL_META_ENUM_TYPE_PROPERTY(table, CNAME))) { \
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
    GBL_MAYBE_UNUSED GBL_CONSTEXPR GBL_INLINE const char* GBL_EVAL(GBL_META_ENUM_TYPE_PROPERTY(table, STRINGIFIER))(GBL_EVAL(GBL_META_ENUM_TYPE_PROPERTY(table, CNAME)) value) { \
        GBL_ENUM_TABLE_TO_STRING(table, value); \
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
