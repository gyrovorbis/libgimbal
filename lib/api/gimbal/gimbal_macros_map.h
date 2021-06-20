#ifndef GIMBAL_MACROS_MAP_H
#define GIMBAL_MACROS_MAP_H

/*
 * Copyright (C) 2012 William Swanson
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
 * CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * Except as contained in this notice, the names of the authors or
 * their institutions shall not be used in advertising or otherwise to
 * promote the sale, use or other dealings in this Software without
 * prior written authorization from the authors.
 */

#define GBL_EVAL0(...) __VA_ARGS__
#define GBL_EVAL1(...) GBL_EVAL0(GBL_EVAL0(GBL_EVAL0(__VA_ARGS__)))
#define GBL_EVAL2(...) GBL_EVAL1(GBL_EVAL1(GBL_EVAL1(__VA_ARGS__)))
#define GBL_EVAL3(...) GBL_EVAL2(GBL_EVAL2(GBL_EVAL2(__VA_ARGS__)))
#define GBL_EVAL4(...) GBL_EVAL3(GBL_EVAL3(GBL_EVAL3(__VA_ARGS__)))
#define GBL_EVAL(...)  GBL_EVAL4(GBL_EVAL4(GBL_EVAL4(__VA_ARGS__)))

#define GBL_MAP_END(...)
#define GBL_MAP_OUT
#define GBL_MAP_COMMA ,

#define GBL_MAP_GET_END2() 0, GBL_MAP_END
#define GBL_MAP_GET_END1(...) GBL_MAP_GET_END2
#define GBL_MAP_GET_END(...) GBL_MAP_GET_END1
#define GBL_MAP_NEXT0(test, next, ...) next GBL_MAP_OUT
#define GBL_MAP_NEXT1(test, next) GBL_MAP_NEXT0(test, next, 0)
#define GBL_MAP_NEXT(test, next)  GBL_MAP_NEXT1(GBL_MAP_GET_END test, next)


#define GBL_MAP0(f, x, peek, ...) f(x) GBL_MAP_NEXT(peek, GBL_MAP1)(f, peek, __VA_ARGS__)
#define GBL_MAP1(f, x, peek, ...) f(x) GBL_MAP_NEXT(peek, GBL_MAP0)(f, peek, __VA_ARGS__)

/**
 * Applies the function macro `f` to each of the remaining parameters.
 */
#define GBL_MAP(f, ...) GBL_EVAL(GBL_MAP1(f, __VA_ARGS__, ()()(), ()()(), ()()(), 0))


/**
 * Applies the function macro `f` to each of the remaining parameters and
 * inserts commas between the results.
 */

#define GBL_MAP_LIST_NEXT1(test, next) GBL_MAP_NEXT0(test, GBL_MAP_COMMA next, 0)
#define GBL_MAP_LIST_NEXT(test, next)  GBL_MAP_LIST_NEXT1(GBL_MAP_GET_END test, next)

#define GBL_MAP_LIST0(f, x, peek, ...) f(x) GBL_MAP_LIST_NEXT(peek, GBL_MAP_LIST1)(f, peek, __VA_ARGS__)
#define GBL_MAP_LIST1(f, x, peek, ...) f(x) GBL_MAP_LIST_NEXT(peek, GBL_MAP_LIST0)(f, peek, __VA_ARGS__)
#define GBL_MAP_LIST(f, ...) GBL_EVAL(GBL_MAP_LIST1(f, __VA_ARGS__, ()()(), ()()(), ()()(), 0))

#define GBL_MAP_TUPLES0(f, x, peek, ...) f x GBL_MAP_NEXT(peek, GBL_MAP_TUPLES1)(f, peek, __VA_ARGS__)
#define GBL_MAP_TUPLES1(f, x, peek, ...) f x GBL_MAP_NEXT(peek, GBL_MAP_TUPLES0)(f, peek, __VA_ARGS__)
#define GBL_MAP_TUPLES(f, ...) GBL_EVAL(GBL_MAP_TUPLES1(f, __VA_ARGS__, ()()(), ()()(), ()()(), 0))



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

#define GBL_META_GENERIC_MACRO_TRAIT_PROPERTY_DEFAULT(defaultFunc, overloads)    defaultFunc
#define GBL_META_GENERIC_MACRO_TRAIT_PROPERTY_OVERLOADS(defaultFunc, overloads)  overloads
#define GBL_META_GENERIC_MACRO_TRAIT_OVERLOADS_PROPERTY_TYPE(type, function)        type
#define GBL_META_GENERIC_MACRO_TRAIT_OVERLOADS_PROPERTY_FUNCTION(type, function)    function

#define GBL_META_GENERIC_MACRO_TRAIT_OVERLOADS_DECLARE_CASE_C(type, function) \
    type: function,

#define GBL_META_GENERIC_MACRO_TRAIT_OVERLOADS_DECLARE_CASE_CPP(type, function) \
    else if constexpr(std::is_same_v<GenericType, type>) return function(std::forward<decltype(args)>(args)...);


#ifndef __cplusplus
#   define GBL_META_GENERIC_MACRO_GENERATE(traits, X)   \
    _Generic((X),                                                                               \
        GBL_MAP_TUPLES(GBL_META_GENERIC_MACRO_TRAIT_OVERLOADS_DECLARE_CASE_C, GBL_MAP_TUPLES(GBL_EVAL, GBL_META_GENERIC_MACRO_TRAIT_PROPERTY_OVERLOADS traits))   \
        default:  GBL_EVAL(GBL_META_GENERIC_MACRO_TRAIT_PROPERTY_DEFAULT traits)                \
    )
#else
#   define GBL_META_GENERIC_MACRO_GENERATE(traits, X) \
     [](auto&&... args) {    \
        using GenericType = decltype(X); \
        using DefaultFuncType = decltype(GBL_EVAL(GBL_META_GENERIC_MACRO_TRAIT_PROPERTY_DEFAULT traits)); \
        if constexpr(false); \
        GBL_MAP_TUPLES(GBL_META_GENERIC_MACRO_TRAIT_OVERLOADS_DECLARE_CASE_CPP, GBL_MAP_TUPLES(GBL_EVAL, GBL_META_GENERIC_MACRO_TRAIT_PROPERTY_OVERLOADS traits))               \
        else if constexpr(std::is_invocable_v<DefaultFuncType, decltype(args)...>) return GBL_EVAL(GBL_META_GENERIC_MACRO_TRAIT_PROPERTY_DEFAULT traits)(std::forward<decltype(args)>(args)...); \
    }
#endif


#endif // GIMBAL_MACROS_MAP_H
