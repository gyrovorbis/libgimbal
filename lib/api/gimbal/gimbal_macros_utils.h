#ifndef GIMBAL_MACROS_UTILS_H
#define GIMBAL_MACROS_UTILS_H

#include "gimbal_macros_map.h"


#define GBL_VA_ARGS(...) ,##__VA_ARGS__
#define _GBL_STRINGIFY(a) #a
#define GBL_STRINGIFY(a) _GBL_STRINGIFY(a)


#define GBL_CAT(A, B) A##B

#define GBL_MACRO_CALL(macro, ...)                  macro(__VA_ARGS__)
#define GBL_MACRO_CALL_NULL(...)

#define GBL_MACRO_CONDITIONAL_CALL_0(macro, ...) GBL_MACRO_CALL_NULL()
#define GBL_MACRO_CONDITIONAL_CALL_1(macro, ...) GBL_MACRO_CALL(macro, __VA_ARGS__)

#define GBL_MACRO_CONDITIONAL_CALL(enabled, ...) \
    GBL_CAT(GBL_MACRO_CONDITIONAL_CALL_,enabled)(__VA_ARGS__)

#define GBL_IDENTITY(A) A

#define GBL_APPEND_SUFFIX(NAME, NUM) GBL_CAT(NAME##_, NUM)
#define GBL_COMPOSE(NAME, ARGS) NAME ARGS

#define GBL_VA_SLIDING_ARG(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, COUNT, ...) COUNT
#define GBL_COMMA  ,
#define GBL_EXPAND() ,,,,,,,,,, // 10 commas (or 11 empty tokens)

#define GBL_VA_OVERLOAD_SUFFIXER(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, ...) \
    GBL_COMPOSE(GBL_VA_SLIDING_ARG, (EXPAND __VA_ARGS__ (), _0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10) )

#define GBL_VA_OVERLOAD_SUFFIXER_ARGC(...) \
    GBL_VA_OVERLOAD_SUFFIXER(0, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, ##__VA_ARGS__)

#define GBL_VA_OVERLOAD_SUFFIXER_1_N(...) \
    GBL_VA_OVERLOAD_SUFFIXER(0, N, N, N, N, N, N, N, N, N, 1, ##__VA_ARGS__)

#define GBL_VA_OVERLOAD_SUFFIXER_0_N(...) \
    GBL_VA_OVERLOAD_SUFFIXER(N, N, N, N, N, N, N, N, N, N, 0, ##__VA_ARGS__)

#define GBL_VA_OVERLOAD_SUFFIXER_2_N(...) \
    GBL_VA_OVERLOAD_SUFFIXER(0, N, N, N, N, N, N, N, N, 2, 1, ##__VA_ARGS__)

#define GBL_VA_OVERLOAD_SUFFIXER_5_N(...) \
    GBL_VA_OVERLOAD_SUFFIXER(0, N, N, N, N, N, 5, 4, 3, 2, 1, ##__VA_ARGS__)


#define GBL_VA_OVERLOAD_SELECT(BASE, SUFFIXER, ...) \
    GBL_APPEND_SUFFIX(BASE, SUFFIXER(__VA_ARGS__))

#define GBL_VA_OVERLOAD_CALL(BASE, SUFFIXER, ...) \
    GBL_VA_OVERLOAD_SELECT(BASE, SUFFIXER, ## __VA_ARGS__)(__VA_ARGS__)




#endif // GIMBAL_MACROS_UTILS_H