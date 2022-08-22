/*! \file
 *  \brief Macro primitives for metaprogramming
 *  \ingroup preprocessor
 *  \sa gimbal_macro_composition.h
 */

#ifndef GIMBAL_MACRO_SEQUENCES_H
#define GIMBAL_MACRO_SEQUENCES_H

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




#define GLUE(A,B) GLUE_I(A,B)
#define GLUE_I(A,B) A##B
#define COUNT(...) COUNT_I(__VA_ARGS__, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1,)
#define COUNT_I(V, _20, _19, _18, _17, _16, _15, _14, _13, _12, _11, _10, _9,_8,_7,_6,_5,_4,_3,_2,X,...) X

#define FIRST(...) FIRST_I(__VA_ARGS__,)
#define FIRST_I(X,...) X
#define REST(X,...) __VA_ARGS__
#define EXPAND(...) __VA_ARGS__

#define FOREACH(MACRO_,DATA_,TUPLE_) GLUE(FOREACH_I_,COUNT TUPLE_)(MACRO_,DATA_,TUPLE_)

#define FOREACH_I_1(MACRO_,DATA_,TUPLE_) MACRO_(DATA_,FIRST TUPLE_)
#define FOREACH_I_2(MACRO_,DATA_,TUPLE_) MACRO_(DATA_,FIRST TUPLE_) FOREACH_I_1(MACRO_,DATA_,(REST TUPLE_))
#define FOREACH_I_3(MACRO_,DATA_,TUPLE_) MACRO_(DATA_,FIRST TUPLE_) FOREACH_I_2(MACRO_,DATA_,(REST TUPLE_))
#define FOREACH_I_4(MACRO_,DATA_,TUPLE_) MACRO_(DATA_,FIRST TUPLE_) FOREACH_I_3(MACRO_,DATA_,(REST TUPLE_))
#define FOREACH_I_5(MACRO_,DATA_,TUPLE_) MACRO_(DATA_,FIRST TUPLE_) FOREACH_I_4(MACRO_,DATA_,(REST TUPLE_))
#define FOREACH_I_6(MACRO_,DATA_,TUPLE_) MACRO_(DATA_,FIRST TUPLE_) FOREACH_I_5(MACRO_,DATA_,(REST TUPLE_))
#define FOREACH_I_7(MACRO_,DATA_,TUPLE_) MACRO_(DATA_,FIRST TUPLE_) FOREACH_I_6(MACRO_,DATA_,(REST TUPLE_))
#define FOREACH_I_8(MACRO_,DATA_,TUPLE_) MACRO_(DATA_,FIRST TUPLE_) FOREACH_I_7(MACRO_,DATA_,(REST TUPLE_))
#define FOREACH_I_9(MACRO_,DATA_,TUPLE_) MACRO_(DATA_,FIRST TUPLE_) FOREACH_I_8(MACRO_,DATA_,(REST TUPLE_))
#define FOREACH_I_10(MACRO_,DATA_,TUPLE_) MACRO_(DATA_,FIRST TUPLE_) FOREACH_I_9(MACRO_,DATA_,(REST TUPLE_))
#define FOREACH_I_11(MACRO_,DATA_,TUPLE_) MACRO_(DATA_,FIRST TUPLE_) FOREACH_I_10(MACRO_,DATA_,(REST TUPLE_))
#define FOREACH_I_12(MACRO_,DATA_,TUPLE_) MACRO_(DATA_,FIRST TUPLE_) FOREACH_I_11(MACRO_,DATA_,(REST TUPLE_))
#define FOREACH_I_13(MACRO_,DATA_,TUPLE_) MACRO_(DATA_,FIRST TUPLE_) FOREACH_I_12(MACRO_,DATA_,(REST TUPLE_))
#define FOREACH_I_14(MACRO_,DATA_,TUPLE_) MACRO_(DATA_,FIRST TUPLE_) FOREACH_I_13(MACRO_,DATA_,(REST TUPLE_))
#define FOREACH_I_15(MACRO_,DATA_,TUPLE_) MACRO_(DATA_,FIRST TUPLE_) FOREACH_I_14(MACRO_,DATA_,(REST TUPLE_))
#define FOREACH_I_16(MACRO_,DATA_,TUPLE_) MACRO_(DATA_,FIRST TUPLE_) FOREACH_I_15(MACRO_,DATA_,(REST TUPLE_))
#define FOREACH_I_17(MACRO_,DATA_,TUPLE_) MACRO_(DATA_,FIRST TUPLE_) FOREACH_I_16(MACRO_,DATA_,(REST TUPLE_))
#define FOREACH_I_18(MACRO_,DATA_,TUPLE_) MACRO_(DATA_,FIRST TUPLE_) FOREACH_I_17(MACRO_,DATA_,(REST TUPLE_))
#define FOREACH_I_19(MACRO_,DATA_,TUPLE_) MACRO_(DATA_,FIRST TUPLE_) FOREACH_I_18(MACRO_,DATA_,(REST TUPLE_))
#define FOREACH_I_20(MACRO_,DATA_,TUPLE_) MACRO_(DATA_,FIRST TUPLE_) FOREACH_I_19(MACRO_,DATA_,(REST TUPLE_))

#define PP_NARG(...) \
         PP_NARG_(__VA_ARGS__,PP_RSEQ_N())
#define PP_NARG_(...) \
         PP_ARG_N(__VA_ARGS__)
#define PP_ARG_N( \
          _1, _2, _3, _4, _5, _6, _7, _8, _9,_10, \
         _11,_12,_13,_14,_15,_16,_17,_18,_19,_20, \
         _21,_22,_23,_24,_25,_26,_27,_28,_29,_30, \
         _31,_32,_33,_34,_35,_36,_37,_38,_39,_40, \
         _41,_42,_43,_44,_45,_46,_47,_48,_49,_50, \
         _51,_52,_53,_54,_55,_56,_57,_58,_59,_60, \
         _61,_62,_63,N,...) N
#define PP_RSEQ_N() \
         63,62,61,60,                   \
         59,58,57,56,55,54,53,52,51,50, \
         49,48,47,46,45,44,43,42,41,40, \
         39,38,37,36,35,34,33,32,31,30, \
         29,28,27,26,25,24,23,22,21,20, \
         19,18,17,16,15,14,13,12,11,10, \
         9,8,7,6,5,4,3,2,1,0

#endif // GIMBAL_MACRO_SEQUENCES_H
