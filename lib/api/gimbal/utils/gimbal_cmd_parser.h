#ifndef GIMBAL_CMD_PARSER_H
#define GIMBAL_CMD_PARSER_H

#include "../meta/instances/gimbal_object.h"
#include "../strings/gimbal_string_list.h"

#define GBL_CMD_PARSER_TYPE                 (GBL_TYPEOF(GblCmdParser))

#define GBL_CMD_PARSER(instance)            (GBL_INSTANCE_CAST(instance, GblCmdParser))
#define GBL_CMD_PARSER_CLASS(klass)         (GBL_CLASS_CAST(klass, GblCmdParser))
#define GBL_CMD_PARSER_GET_CLASS(instance)  (GBL_INSTANCE_GET_CLASS(instance, GblCmdParser))

#define GBL_SELF_TYPE GblCmdParser

GBL_DECLS_BEGIN

GBL_FORWARD_DECLARE_STRUCT(GblCmdParser);

GBL_DECLARE_FLAGS(GBL_CMD_FLAGS) {
    GBL_CMD_FLAGS_NONE    = 0x0,
    GBL_CMD_FLAG_HIDDEN   = 0x1,
};

GBL_DECLARE_STRUCT(GblCmdOption) {
    const char* pShort;
    const char* pLong;
    GblFlags    flags;
    const char* pDesc;
};

GBL_DECLARE_ENUM(GBL_CMD_PARSER_STATUS) {
    GBL_CMD_PARSER_STATUS_PENDING,
    GBL_CMD_PARSER_STATUS_SUCCESS,
    GBL_CMD_PARSER_STATUS_MISSING_POSITIONAL,
    GBL_CMD_PARSER_STATUS_EXTRA_POSITIONAL,
    GBL_CMD_PARSER_STATUS_INVALID_ARG,
    GBL_CMD_PARSER_STATUS_UNKNOWN_OPTION,
    GBL_CMD_PARSER_STATUS_UNKNOWN_ERROR
};

GBL_CLASS_DERIVE(GblCmdParser, GblObject)
    GBL_RESULT (*pFnParseCmd)(GBL_SELF, GblStringView cmd);
GBL_CLASS_END

GBL_INSTANCE_DERIVE(GblCmdParser, GblObject)
GBL_INSTANCE_END

GBL_PROPERTIES(GblCmdParser,
    (argc,        GBL_GENERIC, (READ, WRITE), GBL_UINT32_TYPE),
    (argv,        GBL_GENERIC, (READ, WRITE), GBL_POINTER_TYPE),
    (description, GBL_GENERIC, (READ, WRITE), GBL_STRING_TYPE),
    (status,      GBL_GENERIC, (READ),        GBL_ENUM_TYPE)
)

GBL_EXPORT GblType        GblCmdParser_type               (void)                                   GBL_NOEXCEPT;

GBL_EXPORT GblCmdParser*  GblCmdParser_create             (void)                                   GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT     GblCmdParser_destroy            (GBL_SELF)                               GBL_NOEXCEPT;
GBL_EXPORT GblBool        GblCmdParser_parse              (GBL_SELF, GblStringList* pArgList)      GBL_NOEXCEPT;
GBL_EXPORT GblBool        GblCmdParser_process            (GBL_SELF, GblStringList* pArgList)      GBL_NOEXCEPT;
GBL_EXPORT void           GblCmdParser_showHelp           (GBL_CSELF, int retCode)                 GBL_NOEXCEPT;
GBL_EXPORT void           GblCmdParser_showVersion        (GBL_CSELF)                              GBL_NOEXCEPT;

GBL_EXPORT void           GblCmdParser_addHelpOption      (GBL_SELF)                               GBL_NOEXCEPT;
GBL_EXPORT void           GblCmdParser_addVersionOption   (GBL_SELF)                               GBL_NOEXCEPT;

GBL_EXPORT void           GblCmdParser_addPositionalArg   (GBL_SELF,
                                                           const char* pName,
                                                           const char* pDesc)                      GBL_NOEXCEPT;

GBL_EXPORT GblBool        GblCmdParser_addOption          (GBL_SELF, const GblCmdOption* pOption)  GBL_NOEXCEPT;
GBL_EXPORT GblBool        GblCmdParser_addOptions         (GBL_SELF, const GblCmdOption* pOptions) GBL_NOEXCEPT;

GBL_EXPORT GblBool        GblCmdParser_isPositionalArgSet (GBL_CSELF, GblSize index)               GBL_NOEXCEPT;
GBL_EXPORT GblBool        GblCmdParser_isOptionSet        (GBL_CSELF, const char* pOption)         GBL_NOEXCEPT;


GBL_EXPORT GBL_RESULT     GblCmdParser_positionalArgValue (GBL_CSELF,
                                                           GblSize index,
                                                           GblType toType,
                                                           ...)                                    GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT     GblCmdParser_optionValue        (GBL_CSELF,
                                                           const char* pOption,
                                                           GblType     toType,
                                                           ...)                                    GBL_NOEXCEPT;

GBL_EXPORT GblStringList* GblCmdParser_unknownOptions     (GBL_CSELF)                              GBL_NOEXCEPT;
GBL_EXPORT void           GblCmdParser_setDescription     (GBL_SELF)                               GBL_NOEXCEPT;

GBL_EXPORT const char*    GblCmdParser_description        (GBL_CSELF)                              GBL_NOEXCEPT;
GBL_EXPORT GBL_CMD_PARSER_STATUS
                          GblCmdParser_status             (GBL_CSELF)                              GBL_NOEXCEPT;
GBL_EXPORT const char*    GblCmdParser_errorText          (GBL_CSELF)                              GBL_NOEXCEPT;
GBL_EXPORT const char*    GblCmdParser_helpText           (GBL_CSELF)                              GBL_NOEXCEPT;

GBL_DECLS_END

#undef GBL_SELF_TYPE

#endif // GIMBAL_CMD_PARSER_H
