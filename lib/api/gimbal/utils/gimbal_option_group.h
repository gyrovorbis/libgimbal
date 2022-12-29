/*! \file
 *  \brief Grouping of parsable command-line options
 *  \ingroup utils
 */

#ifndef GIMBAL_OPTION_GROUP_H
#define GIMBAL_OPTION_GROUP_H

#include "../meta/instances/gimbal_object.h"
#include "../meta/signals/gimbal_signal.h"
#include "../strings/gimbal_string_list.h"
#include "gimbal_version.h"

#define GBL_OPTION_GROUP_TYPE                   (GBL_TYPEOF(GblOptionGroup))
#define GBL_OPTION_GROUP(instance)              (GBL_INSTANCE_CAST(instance, GblOptionGroup))
#define GBL_OPTION_GROUP_CLASS(klass)           (GBL_CLASS_CAST(klass, GblOptionGroup))
#define GBL_OPTION_GROUP_GET_CLASS(instance)    (GBL_INSTANCE_GET_CLASS(instance, GblOptionGroup))

#define GBL_SELF_TYPE GblOptionGroup

GBL_DECLS_BEGIN

GBL_FORWARD_DECLARE_STRUCT(GblOptionGroup);
GBL_FORWARD_DECLARE_STRUCT(GblOption);

typedef GBL_RESULT (*GblOptionCallbackFn)(GblOptionGroup*  pGroup,
                                          const GblOption* pOption,
                                          GblStringView    value,
                                          GblBool*         pConsumed);

GBL_DECLARE_FLAGS(GBL_OPTION_FLAGS) {
    GBL_OPTION_FLAG_NONE            = 0,
    GBL_OPTION_FLAG_HIDDEN          = 0x1,
    GBL_OPTION_FLAG_BOOL_INVERTED   = 0x2,
    GBL_OPTION_FLAG_BOOL_NO_VALUE   = 0x4
};

GBL_DECLARE_ENUM(GBL_ARG_TYPE) {
    GBL_ARG_TYPE_STRING,
    GBL_ARG_TYPE_BOOL,
    GBL_ARG_TYPE_UINT8,
    GBL_ARG_TYPE_CHAR,
    GBL_ARG_TYPE_UINT16,
    GBL_ARG_TYPE_INT16,
    GBL_ARG_TYPE_UINT32,
    GBL_ARG_TYPE_INT32,
    GBL_ARG_TYPE_UINT64,
    GBL_ARG_TYPE_INT64,
    GBL_ARG_TYPE_FLOAT,
    GBL_ARG_TYPE_DOUBLE,
    GBL_ARG_TYPE_CALLBACK
};

GBL_DECLARE_UNION(GblOptionPtr) {
    void*               pData;
    GblOptionCallbackFn pFn;
};

GBL_DECLARE_STRUCT(GblOption) {
    const char*  pLongName;
    char         shortName;
    GBL_ARG_TYPE type;
    GblOptionPtr pOutput;
    const char*  pDescription;
    const char*  pValueName;
    GblFlags     flags;
};

GBL_CLASS_DERIVE(GblOptionGroup, GblObject)
    GBL_RESULT (*pFnParse) (GBL_SELF, GblStringList* pList);
    GBL_RESULT (*pFnTry)   (GBL_SELF, GblStringView key, GblStringView value, GblSize* pUsed);
GBL_CLASS_END

/*! \brief Grouping of command-line options
 *  \ingroup utils
 *  GblOptionGroup represents a collection of configurable
 *  command-line options, which may be parsed directly or
 *  added to a top-level GblCmdParser. Options are divided
 *  into groups so that each module within an application
 *  can manage its own options independently.
 *  \sa GblCmdParser
 */
GBL_INSTANCE_DERIVE(GblOptionGroup, GblObject)
    GblOption*     pOptions;
    GblSize        optionCount;
    GblStringRef*  pPrefix;
    GblVersion     version;
    GblStringRef*  pSummary;
    GblStringRef*  pDescription;
    GblStringList* pParsedArgs;
    GBL_RESULT     parseResult;
GBL_INSTANCE_END

GBL_PROPERTIES(GblOptionGroup,
    (name,        GBL_GENERIC, (READ, CONSTRUCT, OVERRIDE), GBL_STRING_TYPE),
    (prefix,      GBL_GENERIC, (READ, CONSTRUCT),           GBL_STRING_TYPE),
    (options,     GBL_GENERIC, (READ, CONSTRUCT),           GBL_POINTER_TYPE),
    (version,     GBL_GENERIC, (READ, WRITE),               GBL_UINT32_TYPE),
    (summary,     GBL_GENERIC, (READ, WRITE),               GBL_STRING_TYPE),
    (description, GBL_GENERIC, (READ, WRITE),               GBL_STRING_TYPE)
)

GBL_SIGNALS(GblOptionGroup,
    (parsePrePass,  (GBL_INSTANCE_TYPE, pRecv), (GBL_POINTER_TYPE, pStringList)),
    (parsePostPass, (GBL_INSTANCE_TYPE, pRecv), (GBL_POINTER_TYPE, pStringList)),
    (parseError,    (GBL_INSTANCE_TYPE, pRecv), (GBL_ENUM_TYPE,    errorCode))
)

GBL_EXPORT GblType         GblOptionGroup_type   (void)                      GBL_NOEXCEPT;

GBL_EXPORT GblOptionGroup* GblOptionGroup_create (const char*      pName,
                                                  const char*      pPrefix,
                                                  const GblOption* pOptions) GBL_NOEXCEPT;

GBL_EXPORT GblRefCount     GblOptionGroup_unref  (GBL_SELF)                  GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT      GblOptionGroup_parse  (GBL_SELF,
                                                  GblStringList* pList)      GBL_NOEXCEPT;
GBL_DECLS_END

#undef GBL_SELF_TYPE

#endif // GIMBAL_OPTION_GROUP_H
