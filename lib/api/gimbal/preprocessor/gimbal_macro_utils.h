/*! \file
 *  \brief Miscellaneous macro utilities and meta generators
 *  \ingroup preprocessor
 *  \sa gimbal_macro_composition.h, gimbal_macro_sequences.h
 */

/*! \defgroup preprocessor Preprocessor
 *  \brief Platform defines, utility macros, preprocessor generators and primitives
 *
 */

/*! \page Preprocessor Preprocessor
 *  \copydoc preprocessor
 */
#ifndef GIMBAL_MACRO_UTILS_H
#define GIMBAL_MACRO_UTILS_H

#include "gimbal_compiler.h"
#include "gimbal_macro_composition.h"
#include <gimbal/core/gimbal_config.h>
#include <stdint.h>

#ifdef __cplusplus
#include <functional>
extern "C" {
#endif

#define GBL_MIN(a,b)                        (((a) < (b))? (a) : (b))
#define GBL_MAX(a,b)                        (((a) > (b))? (a) : (b))
#define GBL_CEIL(numerator, denominator)    ((numerator / denominator + (numerator % denominator != 0)))
#define GBL_CLAMP(n, min, max)              (((n) > (max)) ? (max) : (((n) < (min)) ? (min) : (n)))

#define GBL_BIT_MASK_2(bits, offset)        (((1u << bits) - 1u) << offset)
#define GBL_BIT_MASK_1(bits)                GBL_BIT_MASK_2(bits, 0)
#define GBL_BIT_MASK(...)                   GBL_VA_OVERLOAD_CALL_ARGC(GBL_BIT_MASK, __VA_ARGS__)

#define GBL_BCD_BYTE_PACK(n)                (((n/10)<<4)|(n%10))            // 0-99
#define GBL_BCD_BYTE_UNPACK(n)              (unsigned)(((n>>4)*10)+(n&0xf))

#define GBL_CONTAINER_OF(ptr, type, member) ((type*)((char*)(ptr) - offsetof(type, member)))
#define GBL_COUNT_OF(array)                 (sizeof(array)/sizeof(array[0]))

#define GBL_FLOAT_NAN(f)                    (f != f)

#define GBL_SWITCH_CASE_STRINGIFY(s)        case s: return #s

#define GBL_LABEL_EMPTY(name)               name: {;}

#define GBL_SWAP(x,y)                                       \
    GBL_STMT_START {                                        \
        unsigned char swap_temp[sizeof(x) == sizeof(y) ?    \
                               (signed)sizeof(x) : -1];     \
        memcpy(swap_temp,&y,sizeof(x));                     \
        memcpy(&y,&x,       sizeof(x));                     \
        memcpy(&x,swap_temp,sizeof(x));                     \
    } GBL_STMT_END

#define GBL_ASSERT(...)  \
    GBL_VA_OVERLOAD_SELECT(GBL_ASSERT, GBL_VA_OVERLOAD_SUFFIXER_ARGC, __VA_ARGS__)(__VA_ARGS__)

#ifndef __cplusplus
#   define GBL_META_GENERIC_MACRO_GENERATE(traits, X)                                                       \
    _Generic((X),                                                                                           \
        GBL_MAP_TUPLES(GBL_META_GENERIC_MACRO_TRAIT_OVERLOADS_DECLARE_CASE_C_,                              \
                       GBL_MAP_TUPLES(GBL_EVAL, GBL_META_GENERIC_MACRO_TRAIT_PROPERTY_OVERLOADS_ traits))   \
        default:  GBL_EVAL(GBL_META_GENERIC_MACRO_TRAIT_PROPERTY_DEFAULT traits)                            \
    )
#else
#   define GBL_META_GENERIC_MACRO_GENERATE(traits, X)                                                                       \
     [](auto&&... args) {                                                                                                   \
        using GenericType = decltype(X);                                                                                    \
        if constexpr(false);                                                                                                \
        GBL_MAP_TUPLES(GBL_META_GENERIC_MACRO_TRAIT_OVERLOADS_DECLARE_CASE_CPP_,                                            \
                       GBL_MAP_TUPLES(GBL_EVAL, GBL_META_GENERIC_MACRO_TRAIT_PROPERTY_OVERLOADS_ traits))                   \
        else if constexpr(requires { GBL_EVAL(GBL_META_GENERIC_MACRO_TRAIT_PROPERTY_DEFAULT_ traits)                        \
                                             (std::forward<decltype(args)>(args)...); })                                    \
            return GBL_EVAL(GBL_META_GENERIC_MACRO_TRAIT_PROPERTY_DEFAULT_ traits)(std::forward<decltype(args)>(args)...);  \
    }
#endif

#define GBL_META_GENERIC_MACRO_TRAIT_PROPERTY_DEFAULT(defaultFunc, overloads)        defaultFunc
#define GBL_META_GENERIC_MACRO_NO_DEFAULT                                            GBL_NULL

// ===== Implementation ======
///\cond
#define GBL_META_GENERIC_MACRO_TRAIT_PROPERTY_OVERLOADS_(defaultFunc, overloads)     overloads
#define GBL_META_GENERIC_MACRO_TRAIT_OVERLOADS_PROPERTY_TYPE_(type, function)        type
#define GBL_META_GENERIC_MACRO_TRAIT_OVERLOADS_PROPERTY_FUNCTION_(type, function)    function

#define GBL_META_GENERIC_MACRO_TRAIT_OVERLOADS_DECLARE_CASE_C_(type, function) \
    type: function,

#define GBL_META_GENERIC_MACRO_TRAIT_OVERLOADS_DECLARE_CASE_CPP_(type, function) \
    else if constexpr(std::same_as<GenericType, type>) return function(std::forward<decltype(args)>(args)...);
///\endcond
#ifdef __cplusplus
}
#endif

#endif // GIMBAL_MACRO_UTILS_H
