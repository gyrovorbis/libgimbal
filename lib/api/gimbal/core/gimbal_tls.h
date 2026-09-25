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
 *  If this preferred path is not available, using the same semantics,
 *  TinyCThread will emulate this behavior by creating OS-level TLS storage.
 *  In a single-threaded build, it will use ordinary static storage.
 *
 *   \author    2023 Falco Girgis
 *   \author    2026 Agustín Bellagamba
 *   \copyright MIT License
 */

#ifndef GIMBAL_TLS_H
#define GIMBAL_TLS_H

#include <tinycthread_tls.h>

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
#define GBL_TLS(type, name, ...) TTHREAD_TLS(type, name, __VA_ARGS__)

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
#define GBL_TLS_LOAD(name) TTHREAD_TLS_LOAD(name)

#endif // GIMBAL_TLS_H
