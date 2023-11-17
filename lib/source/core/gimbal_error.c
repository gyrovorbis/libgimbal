#include <gimbal/core/gimbal_error.h>
#include <tinycthread.h>

static GBL_THREAD_LOCAL GblError* pPending_ = NULL;

GBL_EXPORT GblError* GblError_pending(void) {
    return pPending_;
}

GBL_EXPORT GblBool GblError_throw(GblError* pSelf) {
    GBL_ASSERT(pSelf);

    const GblBool retValue = GblError_clear();
    pPending_ = pSelf;

    return retValue;
}

GBL_EXPORT GblError* GblError_catch(GblType type) {
    GblError* pReturn = NULL;

    if(GblType_check(GBL_TYPEOF(pPending_), type)) {
        pReturn   = pPending_;
        pPending_ = NULL;
    }

    return pReturn;
}

GBL_EXPORT GblBool GblError_clear(void) {
    GblBool retVal = GBL_FALSE;

    GBL_UNREF(pPending_);

    if(pPending_) retVal = GBL_TRUE;

    pPending_ = NULL;
    return retVal;
}

GBL_EXPORT GblError* (GblError_create)(GblType     derived,
                                       const char* pFile,
                                       const char* pFunc,
                                       size_t      line,
                                       GblType     resultType,
                                       GblEnum     result,
                                       const char* pFmt,
                                       ...)
{
    GBL_ASSERT(GblType_check(derived, GBL_ERROR_TYPE));

    GblError* pSelf = NULL;
#if 1
    if((pPending_ && GBL_TYPEOF(pPending_) == derived)) {
        pSelf = pPending_;
        pPending_ = NULL;
       // GblStringRef_unref(pSelf->pMessage);
    } else {
#endif
        pSelf = (GblError*)GblObject_create(derived, NULL);
#if 1
    }
#endif

    pSelf->pFile      = pFile;
    pSelf->pFunction  = pFunc;
    pSelf->line       = line;
    pSelf->pThread    = GblThread_current();
    pSelf->resultType = resultType;
    pSelf->result     = result;

    va_list varArgs;
    va_start(varArgs, pFmt);
    vsnprintf(pSelf->message, sizeof(pSelf->message), pFmt, varArgs);
    //GblStringRef_createFormatVaList(pFmt, &varArgs);
    //pSelf->pMessage = GblStringRef_create(pFmt);
    va_end(varArgs);

    return pSelf;
}

GBL_EXPORT GblError* GblError_ref(const GblError* pSelf) {
    return GBL_ERROR(GBL_REF(pSelf));
}

GBL_EXPORT GblRefCount GblError_unref(GblError* pSelf) {
    return GBL_UNREF(pSelf);
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

//    GblStringRef_unref(pSelf->pMessage);

    GBL_CTX_END();
}

static GBL_RESULT GblErrorClass_init_(GblClass* pClass, const void* pUd) {
    GBL_UNUSED(pUd);

    GBL_BOX_CLASS(pClass)   ->pFnDestructor   = GblError_GblBox_destructor_;
    //GBL_OBJECT_CLASS(pClass)->pFnProperty     = GblError_GblObject_property_;
    //GBL_OBJECT_CLASS(pClass)->pFnSetProperty  = GblError_GblObject_setProperty_;
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
        type = GblType_register(GblQuark_internStatic("GblError"),
                                GBL_OBJECT_TYPE,
                                &info,
                                GBL_TYPE_FLAG_TYPEINFO_STATIC |
                                GBL_TYPE_FLAG_CLASS_PINNED);
}

    return type;
}
