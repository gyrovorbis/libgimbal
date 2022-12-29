/*! \file
 *  \brief Modular command-line option parser
 *  \ingroup utils
 */

#ifndef GIMBAL_CMD_PARSER_H
#define GIMBAL_CMD_PARSER_H

#include "gimbal_option_group.h"

#define GBL_CMD_PARSER_TYPE                 (GBL_TYPEOF(GblCmdParser))
#define GBL_CMD_PARSER(instance)            (GBL_INSTANCE_CAST(instance, GblCmdParser))
#define GBL_CMD_PARSER_CLASS(klass)         (GBL_CLASS_CAST(klass, GblCmdParser))
#define GBL_CMD_PARSER_GET_CLASS(instance)  (GBL_INSTANCE_GET_CLASS(instance, GblCmdParser))

#define GBL_SELF_TYPE GblCmdParser

GBL_DECLS_BEGIN

GBL_FORWARD_DECLARE_STRUCT(GblCmdParser);

GBL_CLASS_DERIVE(GblCmdParser, GblObject)
    GBL_RESULT (*pFnParse)           (GBL_SELF, GblStringView cmd);
    GBL_RESULT (*pFnTryUnknownOption)(GBL_SELF, GblStringView key, GblStringView value, GblSize* pUsed);
GBL_CLASS_END

/*! \brief General-purpose command-line argument parser
 *  \ingroup utils
 *  GblCmdParser is a configurable, modular, generalized
 *  parser for processing command-line arguments. A
 *  parser may contain a single "main" option group and
 *  a collection of secondary option groups.
 *  This modularity allows each library or framework
 *  within an application to add its own individual option
 *  group to the main command parser.
 *  \sa GblOptionGroup
 */
GBL_INSTANCE_DERIVE(GblCmdParser, GblObject)
    GblStringRef* pErrorMsg;
    GBL_RESULT    parseResult;
    uint8_t       allowExtraArgs       : 1;
    uint8_t       allowUnknownOptions  : 1;
    uint8_t       firstArgAsExecutable : 1;
    uint8_t       addVersionOption     : 1;
    uint8_t       addHelpOption        : 1;
GBL_INSTANCE_END

#if 0
GBL_PROPERTIES(GblCmdParser,
    (errorText, GBL_GENERIC, (READ, WRITE), GBL_STRING_TYPE),
    (status,    GBL_GENERIC, (READ, WRITE), GBL_ENUM_TYPE)
)
#endif

#if 0
GBL_SIGNALS(GblCmdParser,
    (parsePrePass,        (GBL_INSTANCE_TYPE, pReceiver), (GBL_POINTER_TYPE, pInitialArgs)),
    (parsePostPass,       (GBL_INSTANCE_TYPE, pReceiver), (GBL_POINTER_TYPE, pRemainingArgs)),
    (parseExtraArgs,      (GBL_INSTANCE_TYPE, pReceiver), (GBL_POINTER_TYPE, pExtraArgs)),
    (parseUnknownOptions, (GBL_INSTANCE_TYPE, pReceiver), (GBL_POINTER_TYPE, pRemaininArgs)),
    (parseError,          (GBL_INSTANCE_TYPE, pReceiver), (GBL_ENUM_TYPE,    errorCode))
)
#endif

GBL_EXPORT GblType     GblCmdParser_type                (void)                   GBL_NOEXCEPT;

GBL_EXPORT GblCmdParser*
                       GblCmdParser_create              (void)                   GBL_NOEXCEPT;
GBL_EXPORT GblRefCount GblCmdParser_unref               (GBL_SELF)               GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT  GblCmdParser_parse               (GBL_SELF,
                                                         GblStringList* pArgs)   GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT  GblCmdParser_addOptionGroup      (GBL_SELF,
                                                         GblOptionGroup* pGroup) GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT  GblCmdParser_setMainOptionGroup  (GBL_SELF,
                                                         GblOptionGroup* pGroup) GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT  GblCmdParser_addPositionalArg    (GBL_SELF,
                                                         const char* pName,
                                                         const char* pDesc)      GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT  GblCmdParser_clearPositionalArgs (GBL_SELF)               GBL_NOEXCEPT;

GBL_EXPORT GblSize     GblCmdParser_positionalArgValueCount
                                                        (GBL_CSELF)              GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT  GblCmdParser_positionalArgValue  (GBL_CSELF,
                                                         GblSize    index,
                                                         GblType    toType,
                                                         void*      pData)       GBL_NOEXCEPT;
GBL_EXPORT const GblStringList*
                       GblCmdParser_positionalArgValues (GBL_CSELF)              GBL_NOEXCEPT;

GBL_EXPORT const GblStringRef*
                       GblCmdParser_executable          (GBL_CSELF)              GBL_NOEXCEPT;
GBL_EXPORT const GblStringList*
                       GblCmdParser_unknownOptions      (GBL_CSELF)              GBL_NOEXCEPT;

GBL_DECLS_END

#undef GBL_SELF_TYPE

#endif // GIMBAL_CMD_PARSER_H
