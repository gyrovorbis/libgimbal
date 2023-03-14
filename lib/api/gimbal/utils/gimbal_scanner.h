/*! \file
 *  \brief GblScanner, generic text tokenizer and lexer
 *  \ingroup utils
 */
#ifndef GIMBAL_SCANNER_H
#define GIMBAL_SCANNER_H

#include "../meta/instances/gimbal_object.h"
#include "../strings/gimbal_string_pattern.h"

#define GBL_SELF_TYPE GblScanner

GBL_FORWARD_DECLARE_STRUCT(GblScanner);

GBL_CLASS_DERIVE_EMPTY(GblScanner, GblObject)

GBL_INSTANCE_DERIVE(GblScanner, GblObject)
    GblStringRef*    pInputString;
    GblStringView    streamBuffer;

    struct {
        GblStringRef* pDelimeters;

        uint32_t readViews    : 1;
        uint32_t readVariants : 1;
    } config;

    struct {
        GblStringView view;
        GblVariant    variant;
    } token;

    struct {
        GblStringView view;
        GblVariant    variant;
    } nextToken;

    GBL_RESULT result;
GBL_INSTANCE_END


GBL_EXPORT GblType     GblScanner_type           (void);

GBL_EXPORT GblScanner* GblScanner_create         (const char* pString, GblSize length);
GBL_EXPORT GblRefCount GblScanner_unref          (GBL_SELF);

GBL_EXPORT GBL_RESULT  GblScanner_setInputString (GBL_SELF, const char* pString, GblSize length);
GBL_EXPORT void        GblScanner_setDelimeters  (GBL_SELF, const char* pDelimeters);

GBL_EXPORT void        GblScanner_raise          (GBL_SELF, GBL_RESULT error, const char* pDetails, ...);
GBL_EXPORT GBL_RESULT  GblScanner_reset          (GBL_SELF);
GBL_EXPORT GBL_RESULT  GblScanner_seek           (GBL_SELF, intptr_t offset);
GBL_EXPORT GblSize     GblScanner_tell           (GBL_CSELF);
GBL_EXPORT GblSize     GblScanner_bytesTotal     (GBL_CSELF);
GBL_EXPORT GblSize     GblScanner_bytesLeft      (GBL_CSELF);
GBL_EXPORT GblBool     GblScanner_eof            (GBL_CSELF);

GBL_EXPORT GblBool     GblScanner_peekNext       (GBL_SELF);
GBL_EXPORT GblBool     GblScanner_next           (GBL_SELF);

#undef GBL_SELF_TYPE

#endif // GIMBAL_SCANNER_H
