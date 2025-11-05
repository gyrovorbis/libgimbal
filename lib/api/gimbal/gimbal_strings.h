/*! \file
 *  \brief   Aggregate header including entire string API.
 *  \ingroup strings
 *
 *  This file is a single-file aggregate header which includes
 *  the header files for all strings contained within libGimbal's
 *  string API, so you do not have to include them individually.
 *
 *  \author    2023, 2025 Falco Girgis
 *  \copyright MIT License
 */
#ifndef GIMBAL_STRINGS_H
#define GIMBAL_STRINGS_H

#include "strings/gimbal_pattern.h"
#include "strings/gimbal_quark.h"
#include "strings/gimbal_string.h"
#include "strings/gimbal_string_buffer.h"
#include "strings/gimbal_string_ref.h"
#include "strings/gimbal_string_view.h"
#include "strings/gimbal_string_list.h"

/*! \defgroup strings Strings
    \ingroup  dataTypes
    \brief    Collection of specialized string types.

    LibGimbal provides a rich set of string manipulation functions with API functionality taken
    from various languages such as C++ and Java.

    Rather than create a single string type and API that is general-purpose and only "decent"
    at everything (such as C++'s std::string and most equivalents), libGimbal has taken the approach
    of creating various different string types, with each optimized for a particular use-case.

    For a little bit of extra complexity, this has the added benefits of allowing for custom-tailored
    APIs that are well suited for such individual use-cases in addition to the performance benefit.
    If you're using libGimbal with a higher-level langauge such as C++ or Rust, you also gain more
    than just another generic string type which is already builtin to your language of choice.

## Choosing the Right String
    The following table can be referenced when deciding which string type to use:

    Type            | Use-cases                                            | Header
    ----------------|------------------------------------------------------|-------
    GblStringView   | Reading or immutable data processing                 | gimbal_string_view.h
    GblStringBuffer | Writing or mutable data processing and building      | gimbal_string_buffer.h
    ::GblStringRef  | Storing or managing ownership                        | gimbal_string_ref.h
    ::GblQuark      | Hashing or uniquely identifying                      | gimbal_quark.h
    GblStringList   | Storing or operating on multiple strings             | gimbal_string_list.h
    GblPattern      | Searching or applying regular expressions to strings | gimbal_pattern.h
*/


#endif // GIMBAL_STRINGS_H
