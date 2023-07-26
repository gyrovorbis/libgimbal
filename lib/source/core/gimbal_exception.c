#include <gimbal/core/gimbal_exception.h>
#include <tinycthread.h>


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
    GBL_CTX_BEGIN(NULL);

    GBL_BOX_CLASS(pClass)      ->pFnDestructor   = GblException_GblBox_destructor_;
    GBL_OBJECT_CLASS(pClass)   ->pFnProperty     = GblException_GblObject_property_;
    GBL_OBJECT_CLASS(pClass)   ->pFnSetProperty  = GblException_GblObject_setProperty_;
    GBL_EXCEPTION_CLASS(pClass)->pFnResultString = GblException_resultString_;

    GBL_CTX_END();
}

GBL_EXPORT GblType GblException_type(void) {
    static GblType type = GBL_INVALID_TYPE;

    static GblTypeInfo info = {
        .pFnClassInit    = GblExceptionClass_init_,
        .classSize       = sizeof(GblExceptionClass),
        .instanceSize    = sizeof(GblException)
    };

    if(type == GBL_INVALID_TYPE) {
        type = GblType_register(GblQuark_internStringStatic("GblException"),
                                      GBL_OBJECT_TYPE,
                                      &info,
                                      GBL_TYPE_FLAG_TYPEINFO_STATIC |
                                      GBL_TYPE_FLAG_CLASS_PINNED    |
                                      GBL_TYPE_FLAG_CLASS_PREINIT);
    }

    return type;
}
