#include <gimbal/meta/properties/gimbal_bool_property.h>
#if 0
GBL_EXPORT GblBoolProperty* GblBoolProperty_create(GblType     ownerType,
                                                   const char* pName,
                                                   GblFlags    flags,
                                                   GblBool     defaultValue)
{
    GblBoolProperty* pProperty = GBL_BOOL_PROPERTY(GblProperty_create(GBL_BOOL_PROPERTY_TYPE,
                                                                      ownerType,
                                                                      pName,
                                                                      flags));
    if(pProperty) {
        pProperty->defaultValue = defaultValue;
    }
    return pProperty;
}

GBL_EXPORT GBL_RESULT GblBoolProperty_construct(GblBoolProperty* pSelf,
                                                GblType     ownerType,
                                                const char* pName,
                                                GblFlags    flags,
                                                GblBool     defaultValue)
{
    GBL_API_BEGIN(NULL);
    GBL_API_VERIFY_CALL(GblProperty_construct(GBL_PROPERTY(pSelf),
                                              GBL_BOOL_PROPERTY_TYPE,
                                              ownerType,
                                              pName,
                                              flags));
    pSelf->defaultValue = defaultValue;
    GBL_API_END();
}

GBL_EXPORT GblType GblBoolProperty_type(void) GBL_NOEXCEPT {
    return GBL_INVALID_TYPE;
}
#endif
