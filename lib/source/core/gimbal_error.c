#include <gimbal/core/gimbal_error.h>
#include <tinycthread.h>

static GBL_THREAD_LOCAL GblError* pCurrent_ = NULL;

GBL_EXPORT GblError* GblError_current(void) {
    return pCurrent_;
}

GBL_EXPORT void GblError_throw(GblError* pSelf) {
    GBL_ASSERT(pSelf);

    GblError_clear();
    pCurrent_ = pSelf;
}

GBL_EXPORT GblError* GblError_catch(GblType type) {
    GblError* pReturn = NULL;

    if(GblType_check(GBL_TYPEOF(pCurrent_), type)) {
        pReturn   = pCurrent_;
        pCurrent_ = NULL;
    }

    return pReturn;
}

GBL_EXPORT void GblError_clear(void) {
    GBL_UNREF(pCurrent_);
    pCurrent_ = NULL;
}

GBL_EXPORT GblError* (GblError_create)(GblType     derived,
                                       const char* pFile,
                                       const char* pFunc,
                                       size_t      line,
                                       GblEnum     result,
                                       const char* pFmt,
                                       ...)
{
    GBL_ASSERT(GblType_check(derived, GBL_ERROR_TYPE));

    GblError* pSelf = GBL_ERROR(GblBox_create(derived));

    pSelf->pFile     = pFile;
    pSelf->pFunction = pFunc;
    pSelf->line      = line;
    pSelf->pThread   = GblThread_current();
    pSelf->result    = result;

    va_list varArgs;
    va_start(varArgs, pFmt);
    GblStringRef_createFormatVaList(pFmt, &varArgs);
    va_end(varArgs);

    return pSelf;
}

GBL_EXPORT GblRefCount GblError_unref(GblError* pSelf) {
    return GBL_UNREF(pSelf);
}

GBL_EXPORT GblType GblError_resultType(const GblError* pError) {
    return GBL_ERROR_GET_CLASS(pError)->resultType;
}

GBL_EXPORT const char* GblError_resultString(const GblError* pError) {
    const char* pStr = NULL;

    GBL_ERROR_GET_CLASS(pError)->pFnResultString(pError, pError->result, &pStr);
    return pStr? pStr : "?";
}

GBL_EXPORT GblBool GblError_hasSource(const GblError* pError) {
    return pError->pFile || pError->pFunction;
}

GBL_EXPORT const char* GblError_file(const GblError* pError) {
    return pError->pFile? pError->pFile : "?";
}

GBL_EXPORT const char* GblError_function(const GblError* pError) {
    return pError->pFunction? pError->pFunction : "?";
}

GBL_EXPORT size_t GblError_line(const GblError* pError) {
    return pError->line;
}

static GBL_RESULT GblError_resultString_(const GblError* pSelf, GblEnum result, const char** ppString) {
    GBL_UNUSED(pSelf);
    GBL_CTX_BEGIN(NULL);
    *ppString = gblResultString(result);
    GBL_CTX_END();
}

static GBL_RESULT GblError_GblObject_property_(const GblObject* pObject, const GblProperty* pProp, GblVariant* pValue) {
    GBL_CTX_BEGIN(NULL);
    switch(pProp->id) {

    default:
        GBL_CTX_RECORD_SET(GBL_RESULT_ERROR_INVALID_PROPERTY,
                           "Attempt to read invalid property %s on %s",
                           GblProperty_nameString(pProp),
                           GblObject_name(pObject));
    }
    GBL_CTX_END();
}

static GBL_RESULT GblError_GblObject_setProperty_(GblObject* pObject, const GblProperty* pProp, GblVariant* pValue) {
    GBL_CTX_BEGIN(NULL);
    switch(pProp->id) {

    default:
        GBL_CTX_RECORD_SET(GBL_RESULT_ERROR_INVALID_PROPERTY,
                           "Attempt to write invalid property %s on %s",
                           GblProperty_nameString(pProp),
                           GblObject_name(pObject));
    }
    GBL_CTX_END();
}

static GBL_RESULT GblError_GblBox_destructor_(GblBox* pBox) {
    GBL_CTX_BEGIN(NULL);

    GblError* pSelf = GBL_ERROR(pBox);

    GblStringRef_unref(pSelf->pMessage);

    GBL_CTX_END();
}

static GBL_RESULT GblErrorClass_init_(GblClass* pClass, const void* pUd) {
    GBL_UNUSED(pUd);

    GBL_BOX_CLASS(pClass)   ->pFnDestructor   = GblError_GblBox_destructor_;
    GBL_OBJECT_CLASS(pClass)->pFnProperty     = GblError_GblObject_property_;
    GBL_OBJECT_CLASS(pClass)->pFnSetProperty  = GblError_GblObject_setProperty_;
    GBL_ERROR_CLASS(pClass) ->pFnResultString = GblError_resultString_;

    return GBL_RESULT_SUCCESS;
}

GBL_EXPORT GblType GblError_type(void) {
    static GblType type = GBL_INVALID_TYPE;

    static GblTypeInfo info = {
        .pFnClassInit    = GblErrorClass_init_,
        .classSize       = sizeof(GblErrorClass),
        .instanceSize    = sizeof(GblError)
    };

    if(type == GBL_INVALID_TYPE) GBL_UNLIKELY {
        type = GblType_register(GblQuark_internStringStatic("GblError"),
                                GBL_OBJECT_TYPE,
                                &info,
                                GBL_TYPE_FLAG_TYPEINFO_STATIC |
                                GBL_TYPE_FLAG_CLASS_PINNED    |
                                GBL_TYPE_FLAG_CLASS_PREINIT);
}

    return type;
}
