#include <gimbal/utils/gimbal_uri.h>
#include <gimbal/utils/gimbal_ref.h>

GBL_DECLARE_STRUCT(GblUri) {
    GblStringRef* pError;
    GblStringRef* pScheme;
    GblStringRef* pUserName;
    GblStringRef* pPassword;
    GblStringRef* pHost;
    GblStringRef* pPath;
    GblStringRef* pQuery;
    GblStringRef* pFragment;
    int           port;
};

static GBL_RESULT GblUri_dtor_(GblUri* pUri) {
    GblStringRef_release(pUri->pError);
    GblStringRef_release(pUri->pScheme);
    GblStringRef_release(pUri->pUserName);
    GblStringRef_release(pUri->pPassword);
    GblStringRef_release(pUri->pHost);
    GblStringRef_release(pUri->pPath);
    GblStringRef_release(pUri->pQuery);
    GblStringRef_release(pUri->pFragment);
    return GBL_RESULT_SUCCESS;
}

static void GblUri_validate_(GblUri* pUri) {

}

GBL_EXPORT GblUri* GblUri_create(void) {
    return GblRef_create(sizeof(GblUri));
}

GBL_EXPORT GblUri* GblUri_createParsed(const char* pStr, GblBool relaxed) {
    GblUri* pUri = GblUri_create();
    GblUri_parse(pUri, pStr, relaxed);
    return pUri;
}

GBL_EXPORT GBL_RESULT GblUri_parse(GblUri* pUri, const char* pStr, GblBool relaxed) {
    // TODO: Unimplemented.
    return GBL_RESULT_UNIMPLEMENTED;
}

GBL_EXPORT GblUri* GblUri_ref(GblUri* pUri) {
    return GblRef_ref(pUri);
}

GBL_EXPORT GblRefCount GblUri_refCount(const GblUri* pUri) {
    return GblRef_refCount(pUri);
}

GBL_EXPORT GblRefCount GblUri_unref(GblUri* pSelf) {
    return GblRef_unref(pSelf, (GblRefDestructFn)GblUri_dtor_);
}

GBL_EXPORT GblStringRef* GblUri_error(const GblUri* pSelf) {
    return pSelf->pError;
}

GBL_EXPORT GblBool GblUri_isValid(const GblUri* pSelf) {
    return !GblUri_error(pSelf);
}

GBL_EXPORT GblBool GblUri_isEmpty(const GblUri* pSelf) {
    return GblStringRef_empty(pSelf->pScheme)   &&
           GblStringRef_empty(pSelf->pUserName) &&
           GblStringRef_empty(pSelf->pPassword) &&
           GblStringRef_empty(pSelf->pHost)     &&
           GblStringRef_empty(pSelf->pPath)     &&
           GblStringRef_empty(pSelf->pQuery)    &&
           GblStringRef_empty(pSelf->pFragment) &&
           pSelf->port == 0;
}

GBL_EXPORT void GblUri_clear(GblUri* pSelf) {
    GblUri_dtor_(pSelf);
    memset(pSelf, 0, sizeof(GblUri));
}

GBL_EXPORT void GblUri_setSchemeRef(GblUri* pSelf, GblStringRef* pRef) {
    GblStringRef_unref(pSelf->pScheme);
    pSelf->pScheme = pRef;
    GblUri_validate_(pSelf);
}

GBL_EXPORT void GblUri_setScheme(GblUri* pSelf, const char* pStr) {
    return GblUri_setSchemeRef(pSelf, GblStringRef_create(pStr));
}

GBL_EXPORT GblStringRef* GblUri_scheme(const GblUri* pSelf) {
    return pSelf->pScheme;
}

GBL_EXPORT void GblUri_setAuthority(GblUri* pSelf, const char* pStr) {
    GblStringRef_unref(pSelf->pUserName);
    GblStringRef_unref(pSelf->pPassword);
    // parse new shit
    GblUri_validate_(pSelf);
}

GBL_EXPORT const char* GblUri_authority(const GblUri* pSelf, GblStringBuffer* pBuffer) {
    // concatenate username + password
    // return pointer to concatted buffer
    return NULL;
}

GBL_EXPORT void GblUri_setUserInfo(GblUri* pSelf, const char* pStr) {
    GblUri_setUserInfoRef(pSelf, pStr? GblStringRef_create(pStr) : NULL);
}

GBL_EXPORT void GblUri_setUserInfoRef(GblUri* pSelf, GblStringRef* pRef) {

}

GBL_EXPORT GblStringRef* GblUri_userInfo(const GblUri* pSelf) {
    return NULL;
}

GBL_EXPORT void GblUri_setUserName(GblUri* pSelf, const char* pStr) {
    GblUri_setUserNameRef(pSelf, pStr? GblStringRef_create(pStr) : NULL);
}

GBL_EXPORT void GblUri_setUserNameRef(GblUri* pSelf, GblStringRef* pRef) {
    GblStringRef_unref(pSelf->pUserName);
    pSelf->pUserName = pRef;
    GblUri_validate_(pSelf);
}

GBL_EXPORT GblStringRef* GblUri_userName(const GblUri* pSelf) {
    return pSelf->pUserName;
}

GBL_EXPORT void GblUri_setPassword(GblUri* pSelf, const char* pStr) {
    GblUri_setPasswordRef(pSelf, pStr? GblStringRef_create(pStr) : NULL);
}

GBL_EXPORT void GblUri_setPasswordRef(GblUri* pSelf, GblStringRef* pRef)  {
    GblStringRef_unref(pSelf->pPassword);
    pSelf->pPassword = pRef;
    GblUri_validate_(pSelf);
}

GBL_EXPORT GblStringRef* GblUri_password(const GblUri* pSelf) {
    return pSelf->pPassword;
}
