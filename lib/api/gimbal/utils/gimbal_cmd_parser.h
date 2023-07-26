/*! \file
 *  \brief   Modular command-line argument parser
 *  \ingroup utils
 *
 *  \todo More descriptive errors, help and version options,
 *        use virtual methods for customization points,
 *        implement signals for events (probably),
 *        process(..) top-level method for handling exiting
 *        upon failure, version, or help.
 *
 *  \author     2023 Falco Girgis
 *  \copyright  MIT License
 */
#ifndef GIMBAL_CMD_PARSER_H
#define GIMBAL_CMD_PARSER_H

#include "gimbal_option_group.h"

/*! \name  Type System
 *  \brief Type UUID and cast operators
 *  @{
 */
#define GBL_CMD_PARSER_TYPE             (GBL_TYPEID(GblCmdParser))            //!< Type UUID of GblCmdParser
#define GBL_CMD_PARSER(self)            (GBL_CAST(GblCmdParser, self))        //!< Function-style GblInstance cast
#define GBL_CMD_PARSER_CLASS(klass)     (GBL_CLASS_CAST(GblCmdParser, klass)) //!< Function-style GblClass cast
#define GBL_CMD_PARSER_GET_CLASS(self)  (GBL_CLASSOF(GblCmdParser, self))     //!< Get a GblCmdParserClass from GblInstance
//! @}

#define GBL_SELF_TYPE GblCmdParser

GBL_DECLS_BEGIN

GBL_FORWARD_DECLARE_STRUCT(GblCmdParser);

//! Attributes of a single command-line argument
typedef struct GblCmdArg {
    GblStringRef* pName;    //!< Name of the command-line argument
    GblStringRef* pDesc;    //!< Description of the command-line argument
} GblCmdArg;

/*! \struct  GblCmdParserClass
 *  \extends GblObjectClass
 *  \brief   GblClass for GblCmdParser
 *
 *  GblCmdParserClass offers a polymorphic virtual table
 *  for processing command-line arguments.
 *
 *  \sa GblCmdParser
 */
GBL_CLASS_DERIVE(GblCmdParser, GblObject)
    //! Parses the command given by \p cmd and returns a GBL_RESULT code
    GBL_RESULT (*pFnParse)           (GBL_SELF,
                                      GblStringView cmd);
    //! Checks to see if unknown command-line option values can be used, returning number used
    GBL_RESULT (*pFnTryUnknownOption)(GBL_SELF,
                                      GblStringView key,
                                      GblStringView value,
                                      size_t*       pUsed);
GBL_CLASS_END

/*! \struct  GblCmdParser
 *  \extends GblObject
 *  \ingroup utils
 *  \brief   General-purpose command-line argument parser
 *
 *  GblCmdParser is a configurable, modular, generalized
 *  parser for processing command-line arguments. A
 *  parser may contain a single "main" option group and
 *  a collection of secondary option groups.
 *  This modularity allows each library or framework
 *  within an application to add its own individual option
 *  group to the main command parser.
 *
 *  \sa GblOptionGroup
 */
GBL_INSTANCE_DERIVE(GblCmdParser, GblObject)
    GblStringRef* pErrorMsg;                //!< Current, pending error message
    GBL_RESULT    parseResult;              //!< Result of last parse operation
    uint8_t       allowExtraArgs       : 1; //!< Toggle for allowing extra positional arguments beyond the ones added
    uint8_t       allowUnknownOptions  : 1; //!< Toggle for allowing unknown options not handled by a GblCmdOptionGroup
    uint8_t       firstArgAsExecutable : 1; //!< Toggle for determining whether to treat the first argument as the executable name
    uint8_t       enableVersionOption  : 1; //!< Toggle for optionally enabling a version command-line option
    uint8_t       enableHelpOption     : 1; //!< Toggle for optionally enabling an autogenerated help option
GBL_INSTANCE_END

//! \cond
GBL_PROPERTIES(GblCmdParser,
    (allowExtraArgs,       GBL_GENERIC, (READ, WRITE), GBL_BOOL_TYPE),
    (allowUnknownOptions,  GBL_GENERIC, (READ, WRITE), GBL_BOOL_TYPE),
    (firstArgAsExecutable, GBL_GENERIC, (READ, WRITE), GBL_BOOL_TYPE),
    (enableVersionOption,  GBL_GENERIC, (READ, WRITE), GBL_BOOL_TYPE),
    (enableHelpOption,     GBL_GENERIC, (READ, WRITE), GBL_BOOL_TYPE),
    (mainOptionGroup,      GBL_GENERIC, (READ, WRITE), GBL_POINTER_TYPE),
    (optionGroups,         GBL_GENERIC, (READ, WRITE), GBL_POINTER_TYPE),
    (positionalArgs,       GBL_GENERIC, (READ, WRITE), GBL_POINTER_TYPE)
)

GBL_SIGNALS(GblCmdParser,
    (parsePrePass,        (GBL_INSTANCE_TYPE, pReceiver), (GBL_POINTER_TYPE, pInitialArgs)),
    (parsePostPass,       (GBL_INSTANCE_TYPE, pReceiver), (GBL_POINTER_TYPE, pRemainingArgs)),
    (parseExtraArgs,      (GBL_INSTANCE_TYPE, pReceiver), (GBL_POINTER_TYPE, pExtraArgs)),
    (parseUnknownOptions, (GBL_INSTANCE_TYPE, pReceiver), (GBL_POINTER_TYPE, pRemaininArgs)),
    (parseError,          (GBL_INSTANCE_TYPE, pReceiver), (GBL_ENUM_TYPE,    errorCode))
)
//! \endcond

//! Returns the GblType UUID associated with GblCmdParser
GBL_EXPORT GblType GblCmdParser_type (void) GBL_NOEXCEPT;

/*! \name  Lifetime Management
 *  \brief Methods for controlling lifetime of a GblCmdParser
 *  @{
 */
//! Creates a GblCmdParser object, initializes it, and returns a pointer to it
GBL_EXPORT GblCmdParser* GblCmdParser_create (void)     GBL_NOEXCEPT;
//! Unreferences a GblCmdParser, destroying it if it's the last one, returning the remaining reference count
GBL_EXPORT GblRefCount   GblCmdParser_unref  (GBL_SELF) GBL_NOEXCEPT;
//! @}

/*! \name  Option Groups
 *  \brief Methods for managing GblCmdOptionGroup subobjects
 *  \relatesalso GblCmdParser
 *  @{
 */
//! Adds the GblOptionGroup \p pGroup to the list of option groups handled by the GblCmdParser
GBL_EXPORT GBL_RESULT      GblCmdParser_addOptionGroup     (GBL_SELF,
                                                            GblOptionGroup* pGroup)    GBL_NOEXCEPT;
//! Sets the internal array of option groups to the list of pointers given by the \p ppGroups array, NULL-terminated
GBL_EXPORT GBL_RESULT      GblCmdParser_setOptionGroups    (GBL_SELF,
                                                            GblOptionGroup** ppGroups) GBL_NOEXCEPT;
//! Returns a pointer to the GblOptionGroup at the given index
GBL_EXPORT GblOptionGroup* GblCmdParser_optionGroup        (GBL_CSELF, size_t idx)     GBL_NOEXCEPT;
//! Finds a GblOptionGroup from the internal list by its GblObject::name.
GBL_EXPORT GblOptionGroup* GblCmdParser_findOptionGroup    (GBL_CSELF,
                                                            const char* pName)         GBL_NOEXCEPT;
//! Returns the number of option groups held within the internal list of them
GBL_EXPORT size_t          GblCmdParser_optionGroupCount   (GBL_CSELF)                 GBL_NOEXCEPT;
//! Sets the main option group for the parser to the one given by \p pGroup, returning the result
GBL_EXPORT GBL_RESULT      GblCmdParser_setMainOptionGroup (GBL_SELF,
                                                            GblOptionGroup* pGroup)    GBL_NOEXCEPT;
//! Returns the GblOptionGroup previously set as the main option group for the GblCmdParser
GBL_EXPORT GblOptionGroup* GblCmdParser_mainOptionGroup    (GBL_CSELF)                 GBL_NOEXCEPT;
//! @}

/*! \name  Positional Arguments
 *  \brief Methods for managing positional arguments
 *  \relatesalso GblCmdParser
 *  @{
 */
//! Dynamically creates a GblCmdArg struct from the given args and adds it to the internal list
GBL_EXPORT GBL_RESULT           GblCmdParser_addPositionalArg        (GBL_SELF,
                                                                      const char* pName,
                                                                      const char* pDesc)       GBL_NOEXCEPT;
//! Sets the 0-terminated list of arguments pointed to by \p pArgs as the new positional argument list
GBL_EXPORT GBL_RESULT           GblCmdParser_setPositionalArgs       (GBL_SELF,
                                                                      const GblCmdArg* pArgs)  GBL_NOEXCEPT;
//! Clears the currently held internal list of GblCmdArg structures, resetting its size to 0
GBL_EXPORT GBL_RESULT           GblCmdParser_clearPositionalArgs     (GBL_SELF)                GBL_NOEXCEPT;
//! Returns the size of the internally managed list of GblCmdArg structures represengin command-line arguments
GBL_EXPORT size_t               GblCmdParser_positionalArgCount      (GBL_CSELF)               GBL_NOEXCEPT;
//! Returns a pointer to the GblCmdArg structure in the internally maintained list at the given index
GBL_EXPORT const GblCmdArg*     GblCmdParser_positionalArg           (GBL_CSELF, size_t index) GBL_NOEXCEPT;
//! After parsing, this function returns the number of positional arguments extracted
GBL_EXPORT size_t               GblCmdParser_positionalArgValueCount (GBL_CSELF)               GBL_NOEXCEPT;
//! After parsing, returns the GblStringList maintained internally, containing the extracted positional argument values
GBL_EXPORT const GblStringList* GblCmdParser_positionalArgValues     (GBL_CSELF)               GBL_NOEXCEPT;
//! After parsing, this function converts the value of the argument at the given \p index to the desired \p toType, copying its value to \p pData, and returning the result
GBL_EXPORT GBL_RESULT           GblCmdParser_positionalArgValue      (GBL_CSELF,
                                                                      size_t  index,
                                                                      GblType toType,
                                                                      void*   pData)           GBL_NOEXCEPT;
//! @}

/*! \name  Parsing
 *  \brief Methods for parsing arguments or managing parsed values
 *  @{
 */
//! After parsing, returns a GblStringRef to the name of the current executable (first positional argument)
GBL_EXPORT const GblStringRef*  GblCmdParser_executable     (GBL_CSELF)            GBL_NOEXCEPT;
//! After parsing, retuns a GblStringList to the internally maintained list of unknown command-line options
GBL_EXPORT const GblStringList* GblCmdParser_unknownOptions (GBL_CSELF)            GBL_NOEXCEPT;
//! Parses the list of arguments given by \p pArgs, populating internal structures, and returning the result
GBL_EXPORT GBL_RESULT           GblCmdParser_parse          (GBL_SELF,
                                                             GblStringList* pArgs) GBL_NOEXCEPT;
//! @}

GBL_DECLS_END

#undef GBL_SELF_TYPE

#endif // GIMBAL_CMD_PARSER_H
