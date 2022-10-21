/*! \file
 *  \brief Fundamental atomic type for reference counting
 *  \ingroup preprocessor
 */

#ifndef GIMBAL_ATOMICS_H
#define GIMBAL_ATOMICS_H

#include "../preprocessor/gimbal_compiler.h"

#if 1
#   define GBL_ATOMIC_INT16                    volatile int16_t
#   define GBL_ATOMIC_INT16_INIT(a, v)         do { a = v; } while(0)
#   define GBL_ATOMIC_INT16_LOAD(a)            a
#   define GBL_ATOMIC_INT16_STORE(a, v)        do { a = v; } while(0)
#   define GBL_ATOMIC_INT16_INC(a)             a++
#   define GBL_ATOMIC_INT16_DEC(a)             a--
#else
#ifdef _WIN32
#   define NOGDI
#   include <windows.h>
#   include <winnt.h>
#   define GBL_ATOMIC_INT16                    volatile int16_t
#   define GBL_ATOMIC_INT16_INIT(a, v)         do { a = v; } while(0)
#   define GBL_ATOMIC_INT16_LOAD(a)            a
#   define GBL_ATOMIC_INT16_INC(a)             InterlockedIncrement16(&a)-1
#   define GBL_ATOMIC_INT16_DEC(a)             InterlockedDecrement16(&a)+1
#else
#   ifdef GBL_CPP_11
#       include <atomic>
#       define GBL_ATOMIC_INT16                std::atomic<int16_t>
#       define GBL_ATOMIC_INT16_INIT(a,v)      a.store(v)
#       define GBL_ATOMIC_INT16_LOAD(a)        a.load()
#       define GBL_ATOMIC_INT16_INC(a)         a.fetch_add(1)
#       define GBL_ATOMIC_INT16_DEC(a)         a.fetch_sub(1)
#   elif defined(GBL_C_11)
#       include <stdatomic.h>
#       define GBL_ATOMIC_INT16               atomic_int_fast16_t
#       define GBL_ATOMIC_INT16_INIT(a, v)    atomic_init(&a, v)
#       define GBL_ATOMIC_INT16_LOAD(a)       atomic_load(&a)
#       define GBL_ATOMIC_INT16_INC(a)        atomic_fetch_add(&a, 1)
#       define GBL_ATOMIC_INT16_DEC(a)        atomic_fetch_sub(&a, 1)
#   endif
#endif
#endif

#endif // GIMBAL_ATOMICS_H
