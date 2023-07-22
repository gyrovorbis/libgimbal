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
    GblStringRef_unref(pUri->pError);
    GblStringRef_unref(pUri->pScheme);
    GblStringRef_unref(pUri->pUserName);
    GblStringRef_unref(pUri->pPassword);
    GblStringRef_unref(pUri->pHost);
    GblStringRef_unref(pUri->pPath);
    GblStringRef_unref(pUri->pQuery);
    GblStringRef_unref(pUri->pFragment);
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

GBL_EXPORT GblUri* GblUri_copy(const GblUri* pSelf, GblUri* pDest) {
    if(!pDest)
        pDest = GblUri_create();
    else
        GblUri_dtor_(pDest);

    pDest->pError    = GblStringRef_ref(pSelf->pError);
    pDest->pScheme   = GblStringRef_ref(pSelf->pScheme);
    pDest->pUserName = GblStringRef_ref(pSelf->pUserName);
    pDest->pPassword = GblStringRef_ref(pSelf->pPassword);
    pDest->pHost     = GblStringRef_ref(pSelf->pHost);
    pDest->pPath     = GblStringRef_ref(pSelf->pPath);
    pDest->pQuery    = GblStringRef_ref(pSelf->pQuery);
    pDest->pFragment = GblStringRef_ref(pSelf->pFragment);
    pDest->port      = pSelf->port;

    return pDest;
}

GBL_EXPORT GBL_RESULT GblUri_parse(GblUri* pUri, const char* pStr, GblBool relaxed) {
    // TODO: Unimplemented.
    return GBL_RESULT_UNIMPLEMENTED;
}

GBL_EXPORT const char* GblUri_format(const GblUri*    pSelf,
                                     GBL_URI_FLAGS    flags,
                                     GblStringBuffer* pStrBuff)
{
    return NULL;
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

GBL_EXPORT void GblUri_setPasswordRef(GblUri* pSelf, GblStringRef* pRef) {
    GblStringRef_unref(pSelf->pPassword);
    pSelf->pPassword = pRef;
    GblUri_validate_(pSelf);
}

GBL_EXPORT GblStringRef* GblUri_password(const GblUri* pSelf) {
    return pSelf->pPassword;
}

GBL_EXPORT void GblUri_setHost(GblUri* pSelf, const char* pStr) {
    GblUri_setHostRef(pSelf, pStr? GblStringRef_create(pStr) : NULL);
}

GBL_EXPORT void GblHost_setHostRef(GblUri* pSelf, GblStringRef* pRef) {
    GblStringRef_unref(pSelf->pHost);
    pSelf->pHost = pRef;
    GblUri_validate_(pSelf);
}

GBL_EXPORT GblStringRef* GblUri_host(const GblUri* pSelf) {
    return pSelf->pHost;
}

GBL_EXPORT void GblHost_setPort(GblUri* pSelf, int port) {
    pSelf->port = port;
}

GBL_EXPORT int GblHost_port(const GblUri* pSelf) {
    return pSelf->port;
}

GBL_EXPORT void GblUri_setPath(GblUri* pSelf, const char* pStr) {
    GblUri_setPathRef(pSelf, pStr? GblStringRef_create(pStr) : NULL);
}

GBL_EXPORT void GblUri_setPathRef(GblUri* pSelf, GblStringRef* pRef) {
    GblStringRef_unref(pSelf->pPath);
    pSelf->pPath = pRef;
    GblUri_validate_(pSelf);
}

GBL_EXPORT GblStringRef* GblUri_path(const GblUri* pSelf) {
    return pSelf->pPath;
}

GBL_EXPORT GblStringView GblUri_directory(const GblUri* pSelf) {
    const size_t startPos =
        GblStringView_findLastOf(GBL_STRV(pSelf->pPath),
                                 GBL_STRV("/"),
                                 GBL_STRING_VIEW_NPOS);

    if(startPos == GBL_STRING_VIEW_NPOS)
        return GblStringView_fromEmpty();
    else if(startPos == 0)
        return GblStringView_fromString("/");
    else
        return GblStringView_fromStringSized(pSelf->pPath, startPos - 1);
}

GBL_EXPORT GblStringView GblUri_fileName(const GblUri* pSelf) {
    const size_t startPos =
        GblStringView_findLastOf(GBL_STRV(pSelf->pPath),
                                 GBL_STRV("/"),
                                 GBL_STRING_VIEW_NPOS);

    if(startPos == GBL_STRING_VIEW_NPOS)
        return GblStringRef_view(pSelf->pPath);
    else if(startPos + 1 >= GblStringRef_length(pSelf->pPath))
        return GblStringView_fromEmpty();
    else
        return GblStringView_fromString(pSelf->pPath + startPos + 1);
}

GBL_EXPORT GblStringView GblUri_extension(const GblUri* pSelf) {
    const GblStringView fileName = GblUri_fileName(pSelf);

    const size_t startPos =
        GblStringView_findLastOf(fileName,
                                 GBL_STRV("."),
                                 GBL_STRING_VIEW_NPOS);

    if(startPos == GBL_STRING_VIEW_NPOS)
        return GblStringView_fromEmpty();
    else if(startPos + 1 >= GblStringRef_length(pSelf->pPath))
        return GblStringView_fromEmpty();
    else
        return GblStringView_removePrefix(fileName, startPos + 1);
}

GBL_EXPORT void GblUri_setQuery(GblUri* pSelf, const char* pStr) {
    GblUri_setQueryRef(pSelf, pStr? GblStringRef_create(pStr) : NULL);
}

GBL_EXPORT void GblUri_setQueryRef(GblUri* pSelf, GblStringRef* pRef) {
    GblStringRef_unref(pSelf->pQuery);
    pSelf->pQuery = pRef;
    GblUri_validate_(pSelf);
}

GBL_EXPORT GblStringRef* GblUri_query(const GblUri* pSelf) {
    return pSelf->pQuery;
}

GBL_EXPORT void GblUri_setFragment(GblUri* pSelf, const char* pStr) {
    GblUri_setFragmentRef(pSelf, pStr? GblStringRef_create(pStr) : NULL);
}

GBL_EXPORT void GblUri_setFragmentRef(GblUri* pSelf, GblStringRef* pRef) {
    GblStringRef_unref(pSelf->pFragment);
    pSelf->pFragment = pRef;
    GblUri_validate_(pSelf);
}

GBL_EXPORT GblStringRef* GblUri_fragment(const GblUri* pSelf) {
    return pSelf->pFragment;
}

GBL_EXPORT GblBool GblUri_isRelative(const GblUri* pSelf) {
    return GBL_FALSE;
}

GBL_EXPORT GblBool GblUri_isParentOf(const GblUri* pSelf, const GblUri* pUri) {
    return GBL_FALSE;
}

GBL_EXPORT int GblUri_compare(const GblUri* pSelf, const GblUri* pUri) {
    return 1;
}

GBL_EXPORT GblBool GblUri_equals(const GblUri* pSelf, const GblUri* pUri, GBL_URI_FLAGS flags) {
    return GblUri_compare(pSelf, pUri) == 0;
}
