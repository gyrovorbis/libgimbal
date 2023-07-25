/*! \file
 *  \brief   Utilities module-wide include
 *  \ingroup utils
 *
 *  This file includes all headers for within the
 *  libGimbal utilities API, so that you don't have
 *  to include them individually.
 *
 *  \author     2023 Falco Girgis
 *  \copyright  MIT License
 */
#ifndef GIMBAL_UTILS_H
#define GIMBAL_UTILS_H

#include "utils/gimbal_ref.h"
#include "utils/gimbal_timer.h"
#include "utils/gimbal_uuid.h"
#include "utils/gimbal_version.h"
#include "utils/gimbal_option_group.h"
#include "utils/gimbal_cmd_parser.h"
#include "utils/gimbal_date_time.h"
#include "utils/gimbal_bit_view.h"
#include "utils/gimbal_byte_array.h"
#include "utils/gimbal_scanner.h"
#include "utils/gimbal_uri.h"
#include "utils/gimbal_settings.h"

/*! \defgroup utils Utilities
    \brief    Auxiliary utility types

    The utilities module provides various supplementary
    structures and APIs for representing commonly-used
    data and performing common tasks within an application
    or framework.
    - Reference-counted pointers
    - Timers and DateTime management
    - Version and UUID management
    - Command-line argument and option parsing
    - User settings management
    - URI decoding
    - Byte array and bit view containers
*/

#endif // GIMBAL_UTILS_H
