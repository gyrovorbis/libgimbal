/*! \file
 *  \brief   GblSettings user configuration management and persistence
 *  \ingroup utils
 *  \todo
 *      - marshal values to signals
 *      - array handling
 *      - swap GblHashSet out for GblTreeSet
 *      - rig up properties
 *
 *  \author    2023 Falco Girgis
 *  \copyright MIT License
 */
#ifndef GIMBAL_SETTINGS_H
#define GIMBAL_SETTINGS_H

#include "../meta/instances/gimbal_object.h"
#include "../meta/signals/gimbal_signal.h"

/*! \name Type System
 *  \brief Type UUID and cast operators
 *  @{
 */
#define GBL_SETTINGS_TYPE            (GBL_TYPEOF(GblSettings))
#define GBL_SETTINGS(self)           (GBL_INSTANCE_CAST(self, GblSettings))
#define GBL_SETTINGS_CLASS(klass)    (GBL_CLASS_CAST(klass, GblSettings))
#define GBL_SETTINGS_GET_CLASS(self) (GBL_INSTANCE_GET_CLASS(self, GblSettings))
//! @}

#define GBL_SELF_TYPE GblSettings

GBL_FORWARD_DECLARE_STRUCT(GblSettings);

/* \enum  GBL_SETTINGS_STATUS
 * \brief Status code for GblSettings state (\ref GblSettings::status)
 */
GBL_DECLARE_ENUM(GBL_SETTINGS_STATUS) {
    GBL_SETTINGS_STATUS_OK,             //!< OK
    GBL_SETTINGS_STATUS_ERROR_FILE,     //!< File Error
    GBL_SETTINGS_STATUS_ERROR_FORMAT    //!< Format Error
};

/*! \struct  GblSettingsClass
 *  \extends GblObjectClass
 *  \brief   GblClass VTable structure for GblSettings
 *
 *  Provides a polymorphic virtual table allowing the
 *  top-level save and load routines to be reimplemented,
 *  as well as reading and writing values from and to the
 *  internal data structure.
 *
 *  \sa  GblSettings
 */
GBL_CLASS_DERIVE(GblSettings, GblObject)
    GBL_RESULT (*pFnSave) (GBL_SELF);
    GBL_RESULT (*pFnLoad) (GBL_SELF);
    GBL_RESULT (*pFnWrite)(GBL_SELF, const char* pKey, GblVariant* pValue);
    GBL_RESULT (*pFnRead) (GBL_SELF, const char* pKey, GblVariant* pValue);
GBL_CLASS_END

/*! \struct  GblSettings
 *  \extends GblObject
 *  \ingroup utils
 *  \brief   User-settings data management and persistence
 *
 *  GblSettings provides a mechanism through which user
 *  and application settings can be accessed uniformly
 *  and serialized and deserialized from some common
 *  format.
 *
 *  \sa GblSettingsClass
 */
GBL_INSTANCE_DERIVE(GblSettings, GblObject)
    GblStringRef*       pPath;
    GblStringRef*       pAppName;
    GblStringRef*       pOrgName;
    GBL_SETTINGS_STATUS status;
    GblBool             dirty;
GBL_INSTANCE_END

//! \cond
GBL_PROPERTIES(GblSettings,
   (appName, GBL_GENERIC, (READ, WRITE), GBL_STRING_TYPE),
   (orgName, GBL_GENERIC, (READ, WRITE), GBL_STRING_TYPE),
   (path,    GBL_GENERIC, (READ, WRITE), GBL_STRING_TYPE),
   (scope,   GBL_GENERIC, (READ, WRITE), GBL_STRING_TYPE),
   (status,  GBL_GENERIC, (READ),        GBL_ENUM_TYPE),
   (dirty,   GBL_GENERIC, (READ),        GBL_BOOL_TYPE)
)

GBL_SIGNALS(GblSettings,
    (added,   (receiver, GBL_INSTANCE_TYPE), (key,    GBL_STRING_TYPE)),
    (removed, (receiver, GBL_INSTANCE_TYPE), (key,    GBL_STRING_TYPE)),
    (changed, (receiver, GBL_INSTANCE_TYPE), (key,    GBL_STRING_TYPE)),
    (saved,   (receiver, GBL_INSTANCE_TYPE), (result, GBL_ENUM_TYPE)),
    (loaded,  (receiver, GBL_INSTANCE_TYPE), (result, GBL_ENUM_TYPE))
)
//! \endcond

GBL_EXPORT GblType      GblSettings_type          (void)                                                   GBL_NOEXCEPT;

GBL_EXPORT GblSettings* GblSettings_create        (void)                                                   GBL_NOEXCEPT;
GBL_EXPORT GblRefCount  GblSettings_unref         (GBL_SELF)                                               GBL_NOEXCEPT;

GBL_EXPORT const char*  GblSettings_scope         (GBL_CSELF)                                              GBL_NOEXCEPT;
GBL_EXPORT const char*  GblSettings_pushScope     (GBL_SELF, const char* pPrefix)                          GBL_NOEXCEPT;
GBL_EXPORT void         GblSettings_pushArray     (GBL_SELF, const char* pName, size_t size)               GBL_NOEXCEPT;
GBL_EXPORT const char*  GblSettings_pop           (GBL_SELF)                                               GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT   GblSettings_setArrayIndex (GBL_SELF, size_t index)                                 GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT   GblSettings_sync          (GBL_SELF)                                               GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT   GblSettings_variant       (GBL_CSELF, const char* pKey, GblVariant* pVariant)      GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT   GblSettings_toValue       (GBL_CSELF, const char* pKey, GblType type, ...)         GBL_NOEXCEPT;
GBL_EXPORT GblType      GblSettings_typeOf        (GBL_CSELF, const char* pKey)                            GBL_NOEXCEPT;

GBL_EXPORT GblBool      GblSettings_isNil         (GBL_CSELF, const char* pKey)                            GBL_NOEXCEPT;
GBL_EXPORT GblBool      GblSettings_toBool        (GBL_CSELF, const char* pKey, GblBool defaultValue)      GBL_NOEXCEPT;
GBL_EXPORT char         GblSettings_toChar        (GBL_CSELF, const char* pKey, char defaultValue)         GBL_NOEXCEPT;
GBL_EXPORT uint8_t      GblSettings_toUint8       (GBL_CSELF, const char* pKey, uint8_t defaultValue)      GBL_NOEXCEPT;
GBL_EXPORT uint16_t     GblSettings_toUint16      (GBL_CSELF, const char* pKey, uint16_t defaultValue)     GBL_NOEXCEPT;
GBL_EXPORT int16_t      GblSettings_toInt16       (GBL_CSELF, const char* pKey, int16_t defaultValue)      GBL_NOEXCEPT;
GBL_EXPORT uint32_t     GblSettings_toUint32      (GBL_CSELF, const char* pKey, uint32_t defaultValue)     GBL_NOEXCEPT;
GBL_EXPORT int32_t      GblSettings_toInt32       (GBL_CSELF, const char* pKey, int32_t defaultValue)      GBL_NOEXCEPT;
GBL_EXPORT uint32_t     GblSettings_toUint64      (GBL_CSELF, const char* pKey, uint32_t defaultValue)     GBL_NOEXCEPT;
GBL_EXPORT int32_t      GblSettings_toInt64       (GBL_CSELF, const char* pKey, int32_t defaultValue)      GBL_NOEXCEPT;
GBL_EXPORT size_t       GblSettings_toSize        (GBL_CSELF, const char* pkey, size_t defaultValue)       GBL_NOEXCEPT;
GBL_EXPORT GblEnum      GblSettings_toEnum        (GBL_CSELF, const char* pKey, GblEnum defaultValue)      GBL_NOEXCEPT;
GBL_EXPORT GblFlags     GblSettings_toFlags       (GBL_CSELF, const char* pKey, GblFlags defaultValue)     GBL_NOEXCEPT;
GBL_EXPORT float        GblSettings_toFloat       (GBL_CSELF, const char* pKey, float defaultValue)        GBL_NOEXCEPT;
GBL_EXPORT double       GblSettings_toDouble      (GBL_CSELF, const char* pKey, double defaultValue)       GBL_NOEXCEPT;
GBL_EXPORT void*        GblSettings_toPointer     (GBL_CSELF, const char* pKey, void* pDefaultValue)       GBL_NOEXCEPT;
GBL_EXPORT const char*  GblSettings_toString      (GBL_CSELF, const char* pKey, const char* pDefValue)     GBL_NOEXCEPT;
GBL_EXPORT GblType      GblSettings_toType        (GBL_CSELF, const char* pKey, GblType defaultValue)      GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT   GblSettings_setNil        (GBL_SELF, const char* pKey)                             GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT   GblSettings_setBool       (GBL_SELF, const char* pKey, GblBool value)              GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT   GblSettings_setChar       (GBL_SELF, const char* pKey, char value)                 GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT   GblSettings_setUint8      (GBL_SELF, const char* pKey, uint8_t value)              GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT   GblSettings_setUint16     (GBL_SELF, const char* pKey, uint16_t value)             GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT   GblSettings_setInt16      (GBL_SELF, const char* pKey, int16_t value)              GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT   GblSettings_setUint32     (GBL_SELF, const char* pKey, uint32_t value)             GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT   GblSettings_setInt32      (GBL_SELF, const char* pKey, int32_t value)              GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT   GblSettings_setUint64     (GBL_SELF, const char* pKey, uint64_t value)             GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT   GblSettings_setInt64      (GBL_SELF, const char* pKey, int64_t value)              GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT   GblSettings_setSize       (GBL_SELF, const char* pKey, size_t value)               GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT   GblSettings_setEnum       (GBL_SELF, const char* pKey, GblType type, GblEnum val)  GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT   GblSettings_setFlags      (GBL_SELF, const char* pKey, GblType type, GblFlags val) GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT   GblSettings_setFloat      (GBL_SELF, const char* pKey, float value)                GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT   GblSettings_setDouble     (GBL_SELF, const char* pKey, double value)               GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT   GblSettings_setPointer    (GBL_SELF, const char* pKey, void* pValue)               GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT   GblSettings_setString     (GBL_SELF, const char* pKey, const char* pValue)         GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT   GblSettings_setType       (GBL_SELF, const char* pKey, GblType value)              GBL_NOEXCEPT;

#undef GBL_SELF_TYPE

#endif // GIMBAL_SETTINGS_H
