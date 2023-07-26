/*! \file
 *  \brief Grouping of parsable command-line options
 *  \ingroup utils
 *
 *  This file contains the GblOptionGroup type and its
 *  associated API. It is used to create a standalone
 *  group of options that correspond to a single module
 *  or unit of code which can be added and parsed off of
 *  a larger, application-wide group of options.
 *
 *  \author     2023 Falco Girgis
 *  \copyright  MIT License
 */
#ifndef GIMBAL_OPTION_GROUP_H
#define GIMBAL_OPTION_GROUP_H

#include "../meta/instances/gimbal_object.h"
#include "../meta/signals/gimbal_signal.h"
#include "../strings/gimbal_string_list.h"
#include "gimbal_version.h"

/*! \name Type System
 *  \brief Type UUID and cast operators
 *  @{
 */
#define GBL_OPTION_GROUP_TYPE               (GBL_TYPEID(GblOptionGroup))            //!< Type UUID for GblOptionGroup
#define GBL_OPTION_GROUP(self)              (GBL_CAST(GblOptionGroup, self))        //!< Casts a GblInstance to GblOptionGroup
#define GBL_OPTION_GROUP_CLASS(klass)       (GBL_CLASS_CAST(GblOptionGroup, klass)) //!< Casts a GblClass to GblOptionGroupClass
#define GBL_OPTION_GROUP_GET_CLASS(self)    (GBL_CLASSOF(GblOptionGroup, self))     //!< Gets a GblOptionGroupClass from a GblInstance
//! @}

#define GBL_SELF_TYPE GblOptionGroup

GBL_DECLS_BEGIN

GBL_FORWARD_DECLARE_STRUCT(GblOptionGroup);
GBL_FORWARD_DECLARE_STRUCT(GblOption);

//! Callback function signature to be used with a GBL_OPTION_TYPE_CALLBACK GblOption
typedef GBL_RESULT (*GblOptionCallbackFn)(GblOptionGroup*  pGroup,
                                          const GblOption* pOption,
                                          GblStringView    value,
                                          GblBool*         pConsumed);

/*! \enum GBL_OPTION_FLAGS
 *  \brief GblOption flags for controlling special option behaviors
 */
GBL_DECLARE_FLAGS(GBL_OPTION_FLAGS) {
    GBL_OPTION_FLAG_NONE            = 0,    //!< None
    GBL_OPTION_FLAG_HIDDEN          = 0x1,  //!< Hidden (doesn't display in the --help text)
    GBL_OPTION_FLAG_BOOL_INVERTED   = 0x2,  //!< Underlying boolean value is swapped from user input
    GBL_OPTION_FLAG_BOOL_NO_VALUE   = 0x4   //!< Bool option is enabled just by being present, without value
};

/*! \enum GBL_OPTION_TYPE
 *  \brief Represents the list of every supported type of GblOption value
 */
GBL_DECLARE_ENUM(GBL_OPTION_TYPE) {
    GBL_OPTION_TYPE_STRING,     //!< const char*
    GBL_OPTION_TYPE_BOOL,       //!< GblBool
    GBL_OPTION_TYPE_UINT8,      //!< uint8_t
    GBL_OPTION_TYPE_CHAR,       //!< char
    GBL_OPTION_TYPE_UINT16,     //!< uint16_t
    GBL_OPTION_TYPE_INT16,      //!< int16_t
    GBL_OPTION_TYPE_UINT32,     //!< uint32_t
    GBL_OPTION_TYPE_INT32,      //!< int32_t
    GBL_OPTION_TYPE_UINT64,     //!< uint64_t
    GBL_OPTION_TYPE_INT64,      //!< int64_t
    GBL_OPTION_TYPE_FLOAT,      //!< float
    GBL_OPTION_TYPE_DOUBLE,     //!< double
    GBL_OPTION_TYPE_CALLBACK    //!< GblOptionCallbackFn
};

//! Union for SAFELY storing either a data or callback pointer
typedef union GblOptionPtr {
    void*               pData;  //!< Data pointer of union
    GblOptionCallbackFn pFn;    //!< Callback pointer of union
} GblOptionPtr;

//! Describes a single command-line option along with handler info
typedef struct GblOption {
    const char*     pLongName;      //!< Long, hyphenated name of option
    char            shortName;      //!< Shorthanded, single-character name of option (optional)
    GBL_OPTION_TYPE type;           //!< Data type of the option handler
    GblOptionPtr    pOutput;        //!< Union for either data or callback pointer
    const char*     pDescription;   //!< Help description of option
    const char*     pValueName;     //!< Name of the value associated with the option
    GblFlags        flags;          //!< Additional flags for the option
} GblOption;

/*! \struct GblOptionGroupClass
 *  \extends GblObjectClass
 *  \brief GblClass structure for GblOptionGroup
 *
 *  GblOptionGroupClass provides polymorphic virtual
 *  functions for customizing the overall behavior of the
 *  parser as well as the behavior for parsing individual
 *  options.
 *
 *  \sa GblOptionGroup
 */
GBL_CLASS_DERIVE(GblOptionGroup, GblObject)
    //! Top-level virtual function for parsing out options and processing from a list
    GBL_RESULT (*pFnParse)(GBL_SELF, GblStringList* pList);
    //! Tries processing a single KV pair as an option, returning the # of args consumed (0-2)
    GBL_RESULT (*pFnTry)  (GBL_SELF, GblStringView key, GblStringView value, size_t *pUsed);
GBL_CLASS_END

/*! \struct  GblOptionGroup
 *  \extends GblObject
 *  \brief   Grouping of command-line options
 *  \ingroup utils
 *
 *  GblOptionGroup represents a collection of configurable
 *  command-line options, which may be parsed directly or
 *  added to a top-level GblCmdParser. Options are divided
 *  into groups so that each module within an application
 *  can manage its own options independently.
 *
 *  \sa GblCmdParser
 */
GBL_INSTANCE_DERIVE(GblOptionGroup, GblObject)
    GblOption*     pOptions;     //!< Array of GblOption entries
    size_t         optionCount;  //!< Number of options within pOptions
    GblStringRef*  pPrefix;      //!< Prefix for all options within this group
    GblVersion     version;      //!< Version number for the module/unit represented by this group
    GblStringRef*  pSummary;     //!< Summary of the module/unit represented by the option group
    GblStringRef*  pDescription; //!< Longer description of the module/unit represented by the option group
    GblStringList* pParsedArgs;  //!< List of all options/values which have been extracted and parsed
    GBL_RESULT     parseResult;  //!< Result of GblOptionGroup_parse()
GBL_INSTANCE_END

//! \cond
GBL_PROPERTIES(GblOptionGroup,
    (name,        GBL_GENERIC, (READ, CONSTRUCT, OVERRIDE), GBL_STRING_TYPE),
    (prefix,      GBL_GENERIC, (READ, CONSTRUCT),           GBL_STRING_TYPE),
    (options,     GBL_GENERIC, (READ, CONSTRUCT),           GBL_POINTER_TYPE),
    (version,     GBL_GENERIC, (READ, WRITE),               GBL_UINT32_TYPE),
    (summary,     GBL_GENERIC, (READ, WRITE),               GBL_STRING_TYPE),
    (description, GBL_GENERIC, (READ, WRITE),               GBL_STRING_TYPE)
)

GBL_SIGNALS(GblOptionGroup,
    (parsePrePass,  (GBL_INSTANCE_TYPE, pReceiver), (GBL_POINTER_TYPE, pStringList)),
    (parsePostPass, (GBL_INSTANCE_TYPE, pReceiver), (GBL_POINTER_TYPE, pStringList)),
    (parseError,    (GBL_INSTANCE_TYPE, pReceiver), (GBL_ENUM_TYPE,    errorCode))
)
//! \endcond

//! Returns the GblType UUID associated with GblOptionGroup
GBL_EXPORT GblType         GblOptionGroup_type   (void)                         GBL_NOEXCEPT;
//! Creates a GblOptionGroup with an option name, prefix, and NULL-terminated option list
GBL_EXPORT GblOptionGroup* GblOptionGroup_create (const char*      pName,
                                                  const char*      pPrefix,
                                                  const GblOption* pOptions)    GBL_NOEXCEPT;
//! Decrements the reference counter of the given GblOptionGroup by 1, destructing when it hits 0
GBL_EXPORT GblRefCount     GblOptionGroup_unref  (GBL_SELF)                     GBL_NOEXCEPT;
//! Processes the given string list, optionally requiring prefixes on all options
GBL_EXPORT GBL_RESULT      GblOptionGroup_parse  (GBL_SELF,
                                                  GblStringList* pList,
                                                  GblBool prefixOnly/*=false*/) GBL_NOEXCEPT;

GBL_DECLS_END

//! \cond
#define GblOptionGroup_parse(...) \
    GblOptionGroup_parseDefault_(__VA_ARGS__, GBL_FALSE)
#define GblOptionGroup_parseDefault_(self, list, prefix, ...) \
    (GblOptionGroup_parse)(self, list, prefix)
//! \endcond

#undef GBL_SELF_TYPE

#endif // GIMBAL_OPTION_GROUP_H
