/*! \file
 *  \brief Thread-local variable management
 *  \ingroup core
 *
 *  This file provides a pair of macros serving as an abstraction layer
 *  between a platform's preferred thread-local storage mechanism, and
 *  the application.
 *
 *  Where supported, GBL_TLS() will use real, compiler-enabled static
 *  TLS by simply declaring the variable with the "thread_local" keyword
 *  and proceeding to access it normally.
 *
 *  If this preferred path is not available, using the same semantics, the
 *  back-end emulate this behavior by creating OS-level TLS storage using
 *  TinyCThread's C11 TLS API, which uses dynamically allocated storage
 *  and key-based lookups.
 *
 *   \author    2023 Falco Girgis
 *   \copyright MIT License
 */

#ifndef GIMBAL_TLS_H
#define GIMBAL_TLS_H

#include <tinycthread.h>
#include "../preprocessor/gimbal_compiler.h"

#if defined(GBL_GAMECUBE) || defined(GBL_PSP)
#   define GBL_TLS_EMULATED 1
#endif

/*! \def GBL_TLS(type, name, init)
 *
 *  Defines a thread-local variable using the given information,
 *  and either using compiler or OS-level TLS depending on the
 *  platform.
 *
 *  \param type     variable type
 *  \param name     variable name
 *  \param init     variable initializer
 *
 *  \sa GBL_TLS_LOAD()
 */
#if !GBL_TLS_EMULATED
#   define GBL_TLS(type, name, ...) GBL_THREAD_LOCAL type name = __VA_ARGS__
#else
#   define GBL_TLS(type, name, ...) \
        tss_t name; \
        static void tls_##name##_init_(void) { \
            int res = tss_create(&name, free); \
            GBL_ASSERT(res == thrd_success, \
                       "Failed to create "#type" TLS for "#name); \
        } \
        static type* tls_##name##_load_(void) { \
            static once_flag once = ONCE_FLAG_INIT; \
            call_once(&once, tls_##name##_init_); \
            type* pPtr = tss_get(name); \
            if(!pPtr) { \
                pPtr = malloc(sizeof(type)); \
                type temp = __VA_ARGS__; \
                memcpy(pPtr, &temp, sizeof(type)); \
                const int res = tss_set(name, pPtr); \
                GBL_ASSERT(res == thrd_success, \
                           "Failed to set "#type" TLS for "#name); \
            } \
            return pPtr; \
        }
#endif

/*! \def GBL_TLS_LOAD(name)
 *
 *  Fetches a pointer to a thread-local variable that was
 *  previously declared with GBL_TLS()
 *
 *  \param  name        variable name
 *  \return pointer     address of the given TLS variable
 *
 *  \sa GBL_TLS()
 */
#if !GBL_TLS_EMULATED
#   define GBL_TLS_LOAD(name)   &name
#else
#   define GBL_TLS_LOAD(name)   tls_##name##_load_()
#endif

#endif // GIMBAL_TLS_H
