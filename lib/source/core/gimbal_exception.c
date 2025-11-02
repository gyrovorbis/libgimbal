#include <gimbal/core/gimbal_exception.h>
#include <gimbal/core/gimbal_tls.h>

#include <tinycthread.h>

static GBL_TLS(GblException*, pCurrent_, NULL);

GBL_EXPORT GblException* GblException_current(void) {
    return *GBL_TLS_LOAD(pCurrent_);
}

GBL_EXPORT void GblException_throw(GblException* pSelf) {
    GBL_ASSERT(pSelf);

    GblException_clear();
    *GBL_TLS_LOAD(pCurrent_) = pSelf;
}

GBL_EXPORT GblException* GblException_catch(GblType type) {
    GblException* pReturn = NULL;

    if(GblType_check(GBL_TYPEOF(*GBL_TLS_LOAD(pCurrent_)), type)) {
        pReturn = *GBL_TLS_LOAD(pCurrent_);
        *GBL_TLS_LOAD(pCurrent_) = NULL;
    }

    return pReturn;
}

GBL_EXPORT GblBool GblException_clear(void) {
    GblBool retVal = GBL_FALSE;

    GBL_UNREF(*GBL_TLS_LOAD(pCurrent_));

    if(*GBL_TLS_LOAD(pCurrent_))
        retVal = GBL_TRUE;

    *GBL_TLS_LOAD(pCurrent_) = NULL;

    return retVal;
}

GBL_EXPORT GblException* (GblException_create)(GblType     derived,
                                               const char* pFile,
                                               const char* pFunc,
                                               size_t      line,
                                               GblType     resultType,
                                               GblEnum     result,
                                               const char* pFmt,
                                               ...)
{
    GBL_ASSERT(GblType_check(derived, GBL_EXCEPTION_TYPE));

    GblException* pSelf = GBL_EXCEPTION(GblBox_create(derived));

    pSelf->pFile      = pFile;
    pSelf->pFunction  = pFunc;
    pSelf->line       = line;
    pSelf->pThread    = GblThread_current();
    pSelf->resultType = resultType;
    pSelf->result     = result;

    va_list varArgs;
    va_start(varArgs, pFmt);
    //GblStringRef_createFormatVaList(pFmt, &varArgs);
    pSelf->pMessage = GblStringRef_create(pFmt);
    va_end(varArgs);

    return pSelf;
}

GBL_EXPORT GblException* GblException_ref(const GblException* pSelf) {
    return GBL_EXCEPTION(GBL_REF(pSelf));
}

GBL_EXPORT GblRefCount GblException_unref(GblException* pSelf) {
    return GBL_UNREF(pSelf);
}

GBL_EXPORT GblType GblException_resultType(const GblException* pError) {
    return GBL_EXCEPTION_GET_CLASS(pError)->resultType;
}

GBL_EXPORT const char* GblException_resultString(const GblException* pError) {
    const char* pStr = NULL;

    GBL_EXCEPTION_GET_CLASS(pError)->pFnResultString(pError, pError->result, &pStr);
    return pStr? pStr : "?";
}

GBL_EXPORT GblBool GblException_hasSource(const GblException* pError) {
    return pError->pFile || pError->pFunction;
}

GBL_EXPORT const char* GblException_file(const GblException* pError) {
    return pError->pFile? pError->pFile : "?";
}

GBL_EXPORT const char* GblException_function(const GblException* pError) {
    return pError->pFunction? pError->pFunction : "?";
}

GBL_EXPORT size_t GblException_line(const GblException* pError) {
    return pError->line;
}

static GBL_RESULT GblException_resultString_(const GblException* pSelf, GblEnum result, const char** ppString) {
    GBL_UNUSED(pSelf);
    GBL_CTX_BEGIN(NULL);
    *ppString = gblResultString(result);
    GBL_CTX_END();
}

static GBL_RESULT GblException_GblObject_property_(const GblObject* pObject, const GblProperty* pProp, GblVariant* pValue) {
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

static GBL_RESULT GblException_GblObject_setProperty_(GblObject* pObject, const GblProperty* pProp, GblVariant* pValue) {
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

static GBL_RESULT GblException_GblBox_destructor_(GblBox* pBox) {
    GBL_CTX_BEGIN(NULL);

    GblException* pSelf = GBL_EXCEPTION(pBox);

    GblStringRef_unref(pSelf->pMessage);

    GBL_CTX_END();
}

static GBL_RESULT GblExceptionClass_init_(GblClass* pClass, const void* pUd) {
    GBL_UNUSED(pUd);

    GBL_BOX_CLASS(pClass)      ->pFnDestructor   = GblException_GblBox_destructor_;
    GBL_OBJECT_CLASS(pClass)   ->pFnProperty     = GblException_GblObject_property_;
    GBL_OBJECT_CLASS(pClass)   ->pFnSetProperty  = GblException_GblObject_setProperty_;
    GBL_EXCEPTION_CLASS(pClass)->pFnResultString = GblException_resultString_;

    return GBL_RESULT_SUCCESS;
}

GBL_EXPORT GblType GblException_type(void) {
    static GblType type = GBL_INVALID_TYPE;

    static GblTypeInfo info = {
        .pFnClassInit    = GblExceptionClass_init_,
        .classSize       = sizeof(GblExceptionClass),
        .instanceSize    = sizeof(GblException)
    };

    if GBL_UNLIKELY(type == GBL_INVALID_TYPE) {
        type = GblType_register(GblQuark_internStatic("GblException"),
                                GBL_OBJECT_TYPE,
                                &info,
                                GBL_TYPE_FLAG_TYPEINFO_STATIC |
                                GBL_TYPE_FLAG_CLASS_PINNED);
}

    return type;
}
