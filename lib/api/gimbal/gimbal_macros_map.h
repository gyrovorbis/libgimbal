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



#define GBL_ENUM_TABLE_DECL_ENUM(cName, value, name, string) \
    cName = value,


#define GBL_ENUM_TABLE_DECLARE(table) \
    GBL_DECLARE_ENUM(GBL_META_ENUM_CNAME(table)) { \
        GBL_MAP_TUPLES(GBL_ENUM_TABLE_DECL_ENUM,  GBL_MAP_TUPLES(GBL_EVAL, GBL_META_ENUM_TUPLE_VALUE_ARRAY table)) \
    }

#define GBL_ENUM_TABLE_RETURN_STRING(cName, value, name, string) \
    case value: return string;

#define GBL_ENUM_TABLE_TO_STRING(table, value) \
    switch(value) { \
        GBL_MAP_TUPLES(GBL_ENUM_TABLE_RETURN_STRING,  GBL_MAP_TUPLES(GBL_EVAL, GBL_META_ENUM_TUPLE_VALUE_ARRAY table)) \
    }


// === CLEAN ME LATER, PART OF TUPLE/TABLE META SHIT ===========

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

#define GBL_META_ENUM_TUPLE(tuple)

#define GBL_META_ENUM_TUPLE_TYPE_INFO(typeInfo, valueTable) typeInfo

#define GBL_META_ENUM_TUPLE_TYPE_INFO_PROPERTY_CNAME(cName, name, description) cName
#define GBL_META_ENUM_TUPLE_TYPE_INFO_PROPERTY_NAME(cName, name, description) name
#define GBL_META_ENUM_TUPLE_TYPE_INFO_PROPERTY_DESCRIPTION(cName, name, description) description

#define GBL_META_ENUM_TUPLE_VALUE_ARRAY(typeInfo, valueArray) valueArray

#define GBL_META_ENUM_TUPLE_VALUE_ARRAY_TUPLE_VALUE(...) GBL_EVAL __VA_ARGS__

#define GBL_META_ENUM_TUPLE_VALUE_PROPERTY_CNAME(cName, value, name, string) cName
#define GBL_META_ENUM_TUPLE_VALUE_PROPERTY_NAME(cName, value, name, string) name
#define GBL_META_ENUM_TUPLE_VALUE_PROPERTY_VALUE(cName, value, name, string) value
#define GBL_META_ENUM_TUPLE_VALUE_PROPERTY_STRING(cName, value, name, string) string

#define GBL_META_ENUM_TYPE_PROPERTY(table, postfix) GBL_META_ENUM_TUPLE_TYPE_INFO_PROPERTY_##postfix GBL_META_ENUM_TUPLE_TYPE_INFO table
#define GBL_META_ENUM_CNAME(table) GBL_META_ENUM_TYPE_PROPERTY(table, CNAME)

#define GBL_META_GENERIC_MACRO_TRAIT_PROPERTY_DEFAULT(defaultFunc, overloads)    defaultFunc
#define GBL_META_GENERIC_MACRO_TRAIT_PROPERTY_OVERLOADS(defaultFunc, overloads)  overloads
#define GBL_META_GENERIC_MACRO_TRAIT_OVERLOADS_PROPERTY_TYPE(type, function)        type
#define GBL_META_GENERIC_MACRO_TRAIT_OVERLOADS_PROPERTY_FUNCTION(type, function)    function

#define GBL_META_GENERIC_MACRO_TRAIT_OVERLOADS_DECLARE_CASE_C(type, function) \
    type: function,

#define GBL_META_GENERIC_MACRO_TRAIT_OVERLOADS_DECLARE_CASE_CPP(type, function) \
    if constexpr(std::is_same_v<decltype(arg), type>) return function(std::forward<decltype(arg)>(arg));


#ifndef __cplusplus
#   define GBL_META_GENERIC_MACRO_GENERATE(traits)   \
    _Generic((X),                                                                               \
        GBL_MAP_TUPLES(GBL_META_GENERIC_MACRO_TRAIT_OVERLOADS_DECLARE_CASE_C, GBL_MAP_TUPLES(GBL_EVAL, GBL_META_GENERIC_MACRO_TRAIT_PROPERTY_OVERLOADS traits))   \
        default:  GBL_EVAL(GBL_META_GENERIC_MACRO_TRAIT_PROPERTY_DEFAULT traits)                \
    )
#else
#   define GBL_META_GENERIC_MACRO_GENERATE(traits) \
     [](auto arg) -> decltype(auto) {    \
        GBL_MAP_TUPLES(GBL_META_GENERIC_MACRO_TRAIT_OVERLOADS_DECLARE_CASE_CPP,                                         \
                        GBL_MAP_TUPLES(GBL_EVAL, GBL_META_GENERIC_MACRO_TRAIT_PROPERTY_OVERLOADS traits))               \
        else return GBL_EVAL(GBL_META_GENERIC_MACRO_TRAIT_PROPERTY_DEFAULT traits)(std::forward<decltype(arg)>(arg));   \
    }
#endif


#if 0

//declare macro "trait" table
#define GBL_POW2_NEXT
    GBL_GENERIC_MACRO(
        GBL_POW2_NEXT_UINT64,               // default
        (uint8_t,  GBL_POW2_NEXT_UINT8),    // per-type overloads
        (uint16_t, GBL_POW2_NEXT_UINT16),
        (uint32_t, GBL_POW2_NEXT_UINT32),
        (uint64_t, GBL_POW2_NEXT_UINT64)
    )

//shit out respective code from the "trait" table
GBL_GENERIC_MACRO_GENERATE_C(GBL_POW2_NEXT);
GBL_GENERIC_MACRO_GENERATE_CPP(GBL_POW2_NEXT);
GBL_GENERIC_MACRO_GENERATE(GBL_POW2_NEXT); //call into the correct one depending on build flags



inline constexpr decltype(auto) GBL_POW2_NEXT(auto arg) {
    if constexpr(std::is_same_v<decltype(arg), uint8_t>) return GBL_POW2_NEXT_UINT8(arg);
    if constexpr(std::is_same_v<decltype(arg), uint16_t>) return GBL_POW2_NEXT_UINT16(arg);
    if constexpr(std::is_same_v<decltype(arg), uint32_t>) return GBL_POW2_NEXT_UINT32(arg);
    if constexpr(std::is_same_v<decltype(arg), uint64_t>) return GBL_POW2_NEXT_UINT64(arg);
    else return GBL_POW2_NEXT_UINT64(arg);
}

#define GBL_POW2_NEXT(X)                    \
    _Generic((X),                           \
        uint8_t:    GBL_POW2_NEXT_UINT8,    \
        uint16_t:   GBL_POW2_NEXT_UINT16,   \
        uint32_t:   GBL_POW2_NEXT_UINT32,   \
        uint64_t:   GBL_POW2_NEXT_UINT64,   \
        default:    GBL_POW2_NEXT_UINT64    \
    )(X)


#endif





#endif // GIMBAL_MACROS_MAP_H
