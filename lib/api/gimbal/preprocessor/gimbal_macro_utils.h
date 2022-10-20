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
#include <concepts>
extern "C" {
#endif

#define GBL_MIN(a,b)                        (((a)<(b))?(a):(b))
#define GBL_MAX(a,b)                        (((a)>(b))?(a):(b))
#define GBL_CEIL(numerator, denominator)    ((numerator / denominator + (numerator % denominator != 0)))
#define GBL_CLAMP(n, min, max)              (((n) > (max)) ? (max) : (((n) < (min)) ? (min) : (n)))

#define GBL_CONTAINER_OF(ptr, type, member) ((type*)((char*)(ptr) - offsetof(type, member)))
#define GBL_COUNT_OF(array)                 (sizeof(array)/sizeof(array[0]))

#define GBL_NPOS                            ((GblSize)-1)

#define GBL_SWITCH_CASE_STRINGIFY(s) \
    case s: return #s

#define GBL_LABEL_EMPTY(name) \
    name: {;}

#define GBL_VA_SNPRINTF(pFormat)                            \
    char buffer[GBL_VA_SNPRINTF_BUFFER_SIZE] = { '\0' };    \
    va_list argptr;                                         \
    va_start(argptr, pFormat);                              \
    const int vsnprintfBytes =                              \
        vsnprintf(buffer, sizeof(buffer), pFormat, argptr); \
    va_end(argptr)

#define GBL_SWAP(x,y)                                       \
    GBL_STMT_START {                                        \
        unsigned char swap_temp[sizeof(x) == sizeof(y) ?    \
                               (signed)sizeof(x) : -1];     \
        memcpy(swap_temp,&y,sizeof(x));                     \
        memcpy(&y,&x,       sizeof(x));                     \
        memcpy(&x,swap_temp,sizeof(x));                     \
    } GBL_STMT_END

// ===== FLAGS ======

/* A mask with x least-significant bits set, possibly 0 or >=32, PPC and endian-safe!
https://stackoverflow.com/questions/8774567/c-macro-to-create-a-bit-mask-possible-and-have-i-found-a-gcc-bug
*/


/*
 * #define FLAGS_NOMENCLATURE
 *
 * #define GBL_FLAGS_FIELD(flagName, fieldName) (flagName##fieldName##POS, flagName##SIZE, flagName##fieldName##MASK)
 *
 * EVMU_SFR_PSW_FLAGS_CY_FIELD_POS
 * EVMU_SFR_PSW_FLAGS_CY_FIELD_SIZE
 * EVMU_SFR_PSW_FLAGS_CY_FIELD_MASK
 *
 */
#if 0
// HIGHEST TOP LEVEL SHIT:
GBL_FLAGS_FIELD_SET(flags, EVMU_SFR_PSW, CY, 0x20);
EVMU_SFR_PSW_FLAGS_FIELD_SET(flags, AC, 110);
s
EVMU_SFR_PSW_FLAGS_SET(flags,
                      (
                        (AC, 110),
                        (CY, 0x20),
                        (OV, GBL_FALSE)
                      ));


#endif
//=== BITFIELDS ====== (pos, count) pair
#if 0
#define EVMU_SFR_PSW_FLAGS_TABLE (                                                                                                      \
        ( EVMU_SFR_PSW_FLAGS, SfrPswFlags, "SFR PSW Flags", gblSfrPswFlagsString),                                                      \
        (                                                                                                                               \
            (C_FIELD_NAME,  BITFIELD_PAIR(BitPos, BitCount), CPP_FIELD_NAME, "Field Description", TYPE(bool/uint/enum), defaultValue),  \
            (CY,            GBL_BITFIELD(0),               Carry,       "Carry Bit", GBL_BOOL, 0),                                      \
            (AC,            GBL_BITFIELD(1, 1),            AuxCarry,    "Aux Carry Flag", GBL_BOOL, 0),                                 \
            (IRBK,          GBL_BITFIELD_FROM_MASK(0x10),  RegIndBank,  "Indirect Address Register Bank", GBL_UINT, 2)                  \
        )                                                                                                                               \
)

// NEED INNER TABLE FOR VALUE DESCRIPTIONS AND/OR TYPES FOR EACH FIELD ENTRY
// - allow them to be bools, integers, enums? Each field has to have a stringifier.

GBL_FLAGS_TABLE_DECLARE(EVMU_SFR_PSW_FLAGS_TABLE);

#endif
#define GBL_BITFIELD_1(bitPos)              (bitPos, 1)
#define GBL_BITFIELD_2(bitPos, bitCount)    (bitPos, bitCount)
#define GBL_BITFIELD(...)  \
    GBL_VA_OVERLOAD_SELECT(GBL_BITFIELD_, GBL_VA_OVERLOAD_SUFFIXER_ARGC, __VA_ARGS__)(__VA_ARGS__)

#define GBL_BITFIELD_FROM_MASK(mask) (GBL_BITMASK_FFS(mask), GBL_BITMASK_POPCOUNT(mask))

#define GBL_BITFIELD_POS_(bitPos, bitCount)     bitPos
#define GBL_BITFIELD_POS(bitField)              GBL_BITFIELD_POS_(bitField)

#define GBL_BITFIELD_SIZE_(bitPos, bitCount)    bitCount
#define GBL_BITFIELD_SIZE(bitField)             GBL_BITFIELD_SIZE_(bitField)

#define GBL_BITFIELD_MASK_(bitPos, bitCount)                                  \
      (((GBL_FLAGS) -1 >> (31 - ((bitPos) + (bitCount)))) & ~((1u << (bitPos)) - 1))

#define GBL_BITFIELD_MASK(bitField) \
    GBL_BITFIELD_MASK_(bitField)

//=== FLAGS ====


#define GBL_FLAGS_FIELD_GET(flags, bitField)                                                \
    ((flags & GBL_BITFIELD_MASK(bitField)) >> GBL_BITFIELD_POS(bitField))

#define GBL_FLAGS_FIELD_CLEAR(flags, bitField)                                              \
    GBL_STMT_START {                                                                        \
        flags &= ~GBL_BITFIELD_MASK(bitField);                                              \
    } GBL_STMT_END

#define GBL_FLAGS_FIELD_SET_3(flags, bitField, value)                                       \
    GBL_STMT_START {                                                                        \
        GBL_FLAGS_FIELD_CLEAR(flags, bitField);                                             \
        (flags) |= (GBL_BITFIELD_MASK(bitField) & (value << GBL_BITFIELD_POS(bitField)));   \
    } GBL_STMT_END

#define GBL_FLAGS_FIELD_SET_2(flags, bitField) \
    GBL_FLAGS_FIELD_SET_3(flags, bitField, 1)

#define GBL_FLAGS_FIELD_SET(...) \
    GBL_VA_OVERLOAD_SELECT(GBL_FLAGS_FIELD_SET, GBL_VA_OVERLOAD_SUFFIXER_ARGC, __VA_ARGS__)(__VA_ARGS__)


// ====== / FLAGS ======

#define GBL_ASSERT(...)                                                     \
    GBL_VA_OVERLOAD_SELECT(GBL_ASSERT, GBL_VA_OVERLOAD_SUFFIXER_ARGC, __VA_ARGS__)(__VA_ARGS__)


#define GBL_META_GENERIC_MACRO_TRAIT_PROPERTY_DEFAULT(defaultFunc, overloads)       defaultFunc
#define GBL_META_GENERIC_MACRO_TRAIT_PROPERTY_OVERLOADS(defaultFunc, overloads)     overloads
#define GBL_META_GENERIC_MACRO_TRAIT_OVERLOADS_PROPERTY_TYPE(type, function)        type
#define GBL_META_GENERIC_MACRO_TRAIT_OVERLOADS_PROPERTY_FUNCTION(type, function)    function

#define GBL_META_GENERIC_MACRO_TRAIT_OVERLOADS_DECLARE_CASE_C(type, function) \
    type: function,

#define GBL_META_GENERIC_MACRO_TRAIT_OVERLOADS_DECLARE_CASE_CPP(type, function) \
    else if constexpr(std::same_as<GenericType, type>) return function(std::forward<decltype(args)>(args)...);

#define GBL_META_GENERIC_MACRO_NO_DEFAULT NULL


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
        if constexpr(false); \
        GBL_MAP_TUPLES(GBL_META_GENERIC_MACRO_TRAIT_OVERLOADS_DECLARE_CASE_CPP, GBL_MAP_TUPLES(GBL_EVAL, GBL_META_GENERIC_MACRO_TRAIT_PROPERTY_OVERLOADS traits))               \
        else if constexpr(requires { GBL_EVAL(GBL_META_GENERIC_MACRO_TRAIT_PROPERTY_DEFAULT traits)(std::forward<decltype(args)>(args)...); }) return GBL_EVAL(GBL_META_GENERIC_MACRO_TRAIT_PROPERTY_DEFAULT traits)(std::forward<decltype(args)>(args)...);\
    }
#endif

#ifdef __cplusplus
}
#endif

#endif // GIMBAL_MACRO_UTILS_H
