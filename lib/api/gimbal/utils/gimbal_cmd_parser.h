#ifndef GIMBAL_CMD_PARSER_H
#define GIMBAL_CMD_PARSER_H

#include "gimbal_option_group.h"

#define GBL_CMD_PARSER_TYPE                 (GBL_TYPEOF(GblCmdParser))
#define GBL_CMD_PARSER(instance)            (GBL_INSTANCE_CAST(instance, GblCmdParser))
#define GBL_CMD_PARSER_CLASS(klass)         (GBL_CLASS_CAST(klass, GblCmdParser))
#define GBL_CMD_PARSER_GET_CLASS(instance)  (GBL_INSTANCE_GET_CLASS(instance, GblCmdParser))

#define GBL_CMD_ERROR(result)               (result >= GBL_CMD_STATUS_ERROR)

#define GBL_SELF_TYPE GblCmdParser

GBL_DECLS_BEGIN

GBL_FORWARD_DECLARE_STRUCT(GblCmdParser);

GBL_DECLARE_FLAGS(GBL_CMD_FLAGS) {
    GBL_CMD_FLAGS_NONE    = 0x0,
    GBL_CMD_FLAG_HIDDEN   = 0x1,
};

GBL_DECLARE_STRUCT(GblCmdArg) {
    const char*  pName;
    const char*  pDesc;
    GBL_ARG_TYPE type;
    void*        pData;
};

GBL_DECLARE_ENUM(GBL_CMD_STATUS) {
    GBL_CMD_STATUS_PENDING,
    GBL_CMD_STATUS_PARSING,
    GBL_CMD_STATUS_SUCCESS,
    GBL_CMD_STATUS_ERROR,
    GBL_CMD_STATUS_MISSING_POSITIONAL,
    GBL_CMD_STATUS_EXTRA_POSITIONAL,
    GBL_CMD_STATUS_INVALID_ARG_TYPE,
    GBL_CMD_STATUS_UNKNOWN_OPTION,
    GBL_CMD_STATUS_INVALID_OPTION_TYPE,
};

GBL_CLASS_DERIVE(GblCmdParser, GblObject)
    GBL_RESULT (*pFnParseCmd)(GBL_SELF, GblStringView cmd);
GBL_CLASS_END

GBL_INSTANCE_DERIVE(GblCmdParser, GblObject)

GBL_INSTANCE_END

GBL_PROPERTIES(GblCmdParser,
    (args,   GBL_GENERIC, (READ, CONSTRUCT), GBL_POINTER_TYPE),
    (info,  GBL_GENERIC, (READ, WRITE), GBL_STRING_TYPE),
    (status, GBL_GENERIC, (READ),        GBL_ENUM_TYPE)
)

GBL_EXPORT GblType        GblCmdParser_type             (void)                                   GBL_NOEXCEPT;

GBL_EXPORT GblCmdParser*  GblCmdParser_create           (void)                                   GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT     GblCmdParser_destroy          (GBL_SELF)                               GBL_NOEXCEPT;
GBL_EXPORT GblBool        GblCmdParser_parse            (GBL_SELF, int argc, const char** pArgv) GBL_NOEXCEPT;

GBL_EXPORT void           GblCmdParser_setArgs          (GBL_SELF,
                                                         const GblCmdArg* pArgs)                 GBL_NOEXCEPT;

GBL_EXPORT GblSize        GblCmdParser_argCount         (GBL_CSELF)                              GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT     GblCmdParser_arg              (GBL_CSELF,
                                                         GblSize      index,
                                                         GBL_ARG_TYPE toType,
                                                         void*        pData)                     GBL_NOEXCEPT;

GBL_EXPORT GBL_CMD_STATUS GblCmdParser_status           (GBL_CSELF)                              GBL_NOEXCEPT;
GBL_EXPORT const char*    GblCmdParser_errorMsg         (GBL_CSELF)                              GBL_NOEXCEPT;

GBL_DECLS_END

#undef GBL_SELF_TYPE

#endif // GIMBAL_CMD_PARSER_H



#if 0
GBL_OBJECT_NEW(GblCmdParser,
                "description", "Converts some shit",
                "enableHelp", GBL_TRUE,
                "enableVersion", GBL_TRUE,
                "allowExtraArgs", GBL_TRUE,
                "allowUnknownOptions", GBL_TRUE,
                "skipFirstArg", GBL_FALSE,
                "argInfo", argInfo_);

GBL_EXPORT GblBool        GblCmdParser_process          (GBL_SELF, int argc, char* pArgv)        GBL_NOEXCEPT;
GBL_EXPORT void           GblCmdParser_showHelp         (GBL_CSELF, int retCode)                 GBL_NOEXCEPT;
GBL_EXPORT void           GblCmdParser_showVersion      (GBL_CSELF)                              GBL_NOEXCEPT;

GBL_EXPORT void           GblCmdParser_addHelpOption    (GBL_SELF)                               GBL_NOEXCEPT;
GBL_EXPORT void           GblCmdParser_addVersionOption (GBL_SELF)                               GBL_NOEXCEPT;
#endif
