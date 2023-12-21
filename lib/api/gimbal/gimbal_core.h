/*! \file
 *  \brief Core module-wide include
 *  \ingroup core
 *
 *  \author Falco Girgis
 */

#ifndef GIMBAL_CORE_H
#define GIMBAL_CORE_H

#include "core/gimbal_ctx.h"
#include "core/gimbal_decls.h"
#include "core/gimbal_result.h"
#include "core/gimbal_stack_frame.h"
#include "core/gimbal_thd.h"
#include "core/gimbal_typedefs.h"
#include "core/gimbal_app.h"
#include "core/gimbal_module.h"
#include "core/gimbal_logger.h"
#include "core/gimbal_exception.h"
#include "core/gimbal_error.h"

/*! \defgroup core Core
    \brief Core framework module

    The core module provides:
    - platform-specific macros and utilities
    - error and exception handling
    - debug and verification utilities
    - structured logging with filtering
    - object-oriented threading and concurrency model

    Typically this is pulled in by just using `#include <gimbal/core.h>`.

    The following table provides an index of some of the core APIs:

    |Type        | Purpose                  |
    |------------|--------------------------|
    |GblException|Unified error handling    |
    |GblLogger   |Generic logging mechanism |
    |GblThread   |Low-level concurrency     |
    |GblModule   |Module system             |
    |GblApp      |Main application object   |
*/

#endif // GIMBAL_CORE_H
