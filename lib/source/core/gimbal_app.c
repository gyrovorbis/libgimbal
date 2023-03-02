#include <gimbal/core/gimbal_app.h>
#include <gimbal/strings/gimbal_string_ref.h>
#include <gimbal/utils/gimbal_version.h>
#include <gimbal/strings/gimbal_string_list.h>

#define GBL_APP_(self)  ((GblApp_*)GBL_INSTANCE_PRIVATE(self, GBL_APP_TYPE))

GBL_DECLARE_STRUCT_PRIVATE(GblApp) {
    GblStringRef*  pOrganization;
    GblStringRef*  pDomain;
    GblVersion     version;
    GblStringList* pArgs;
};

static GblBool        startingUp_  = GBL_TRUE;
static GblBool        closingDown_ = GBL_FALSE;
static GblApp*        pInstance_   = NULL;
static GblStringList* pLibPaths_   = NULL;

static void GblApp_logInfo_(const GblApp* pApp) {
    GBL_UNUSED(pApp);
    GBL_CTX_BEGIN(NULL);
    GBL_CTX_END_BLOCK();
}

GBL_EXPORT GblBool GblApp_startingUp(void) {
    return startingUp_;
}

GBL_EXPORT GblBool GblApp_closingDown(void) {
    return closingDown_;
}

GBL_EXPORT GblApp* GblApp_instance(void) {
    return pInstance_;
}

GBL_EXPORT const GblStringList* GblApp_libraryPaths(void) {
    if(!pLibPaths_) {
        pLibPaths_ = GblStringList_createEmpty();
    }
    return pLibPaths_;
}

GBL_EXPORT void GblApp_addLibraryPath(const char* pPath) {
    if(!pLibPaths_) {
        pLibPaths_ = GblStringList_createEmpty();
    }
    GblStringList_pushBack(pLibPaths_, pPath);
}

GBL_EXPORT GblApp* GblApp_create(int argc, const char** ppArgv) {
    GblApp* pApp = pInstance_;

    GBL_CTX_BEGIN(NULL);
    GBL_CTX_VERIFY(!pApp,
                   GBL_RESULT_ERROR_INVALID_OPERATION,
                   "Attempted create a GblApp with one already active!");

    pApp = GBL_OBJECT_NEW(GblApp,
                          "argc", argc,
                          "argv", ppArgv);
    GBL_CTX_END_BLOCK();
    pInstance_ = pApp;
    return pApp;
}

GBL_EXPORT GblRefCount GblApp_unref(GblApp* pSelf) {
    return GBL_BOX_UNREF(pSelf);
}

GBL_EXPORT GBL_RESULT GblApp_exec(GblApp* pSelf) {
    GBL_CTX_BEGIN(NULL);
    GBL_CTX_END();
}

GBL_EXPORT GBL_RESULT GblApp_exit(GblApp* pSelf, int retVal) {
    GBL_CTX_BEGIN(NULL);
    GBL_CTX_END();
}

GBL_EXPORT GBL_RESULT GblApp_quit(GblApp* pSelf, int retVal) {
    GBL_CTX_BEGIN(NULL);
    GBL_CTX_END();
}

GBL_EXPORT GblVersion GblApp_version(const GblApp* pSelf) {
    return GBL_APP_(pSelf)->version;
}

GBL_EXPORT const GblStringRef* GblApp_organization(const GblApp* pSelf) {
    return GBL_APP_(pSelf)->pOrganization;
}

GBL_EXPORT const GblStringRef* GblApp_domain(const GblApp* pSelf) {
    return GBL_APP_(pSelf)->pDomain;
}

static GBL_RESULT GblApp_Object_property_(const GblObject* pObject, const GblProperty* pProp, GblVariant* pValue) {
    GBL_CTX_BEGIN(NULL);
    GblApp* pSelf = GBL_APP(pObject);
    GBL_UNUSED(pSelf, pValue);
    switch(pProp->id) {
    //case GblOptionGroup_Property_Id_name:
    //    GBL_CTX_VERIFY_CALL(GblVariant_setValueMove(pValue, pProp->valueType, GblStringRef_acquire(GblObject_name(pObject))));
    //    break;
    default: GBL_CTX_RECORD_SET(GBL_RESULT_ERROR_INVALID_PROPERTY, "Reading unhandled property: %s", GblProperty_nameString(pProp));
    }
    GBL_CTX_END();
}

static GBL_RESULT GblApp_Object_setProperty_(GblObject* pObject, const GblProperty* pProp, GblVariant* pValue) {
    GBL_CTX_BEGIN(NULL);
    GBL_UNUSED(pProp);
    GblApp* pSelf = GBL_APP(pObject);
    GBL_UNUSED(pSelf, pValue);
    switch(pProp->id) {
    //case GblOptionGroup_Property_Id_name: {
    //    const char* pName = NULL;
    //   GBL_CTX_VERIFY_CALL(GblVariant_getValueCopy(pValue, &pName));
    //    GblObject_setName(pObject, pName);
    //    break;
    //}
    default: GBL_CTX_RECORD_SET(GBL_RESULT_ERROR_INVALID_PROPERTY,
                                "Writing unhandled property: %s",
                                GblProperty_nameString(pProp));
    }
    GBL_CTX_END();
}

static GBL_RESULT GblApp_Object_constructed_(GblObject* pObject) {
    GBL_CTX_BEGIN(NULL);

    GBL_INSTANCE_VCALL_DEFAULT(GblObject, pFnConstructed, pObject);

    GblApp* pSelf = GBL_APP(pObject);
    GblApp_* pSelf_ = GBL_APP_(pSelf);

    startingUp_ = GBL_FALSE;

    pSelf_->pArgs = GblStringList_createFromArray(pSelf->ppArgv, pSelf->argc);

    GblApp_logInfo_(GBL_APP(pObject));

    GBL_CTX_END();
}

static GBL_RESULT GblApp_Box_destructor_(GblBox* pBox) {
    GBL_CTX_BEGIN(NULL);

    GblApp* pSelf = GBL_APP(pBox);
    GblApp_* pSelf_ = GBL_APP_(pSelf);

    closingDown_ = GBL_TRUE;

    GBL_CTX_VERIFY_CALL(GblStringRef_release(pSelf_->pOrganization));
    GBL_CTX_VERIFY_CALL(GblStringRef_release(pSelf_->pDomain));
    GBL_CTX_VERIFY_CALL(GblStringList_destroy(pSelf_->pArgs));
    GBL_CTX_VERIFY_CALL(GblStringList_destroy(pLibPaths_));

    GBL_INSTANCE_VCALL_DEFAULT(GblObject, base.pFnDestructor, pBox);

    GBL_CTX_END();
}

static GBL_RESULT GblAppClass_init_(GblClass* pClass, const void* pUd, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GBL_UNUSED(pUd);

    GBL_BOX_CLASS(pClass)   ->pFnDestructor  = GblApp_Box_destructor_;
    GBL_OBJECT_CLASS(pClass)->pFnConstructed = GblApp_Object_constructed_;
    GBL_OBJECT_CLASS(pClass)->pFnProperty    = GblApp_Object_property_;
    GBL_OBJECT_CLASS(pClass)->pFnSetProperty = GblApp_Object_setProperty_;

    GBL_CTX_END();
}

GBL_EXPORT GblType GblApp_type(void) {
    static GblType type = GBL_INVALID_TYPE;

    if(type == GBL_INVALID_TYPE) {
        GBL_CTX_BEGIN(NULL);

        type = GblType_registerStatic(GblQuark_internStringStatic("GblApp"),
                                      GBL_OBJECT_TYPE,
                                      &(GblTypeInfo) {
                                          .pFnClassInit        = GblAppClass_init_,
                                          .classSize           = sizeof(GblAppClass),
                                          .instanceSize        = sizeof(GblApp),
                                          .instancePrivateSize = sizeof(GblApp_)
                                      },
                                      GBL_TYPE_FLAGS_NONE);

        GBL_CTX_VERIFY_LAST_RECORD();
        GBL_CTX_END_BLOCK();

    }

    return type;
}

