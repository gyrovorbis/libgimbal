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
    GBL_CTX_BEGIN(NULL);

    GblApp_* pSelf_ = GBL_APP_(pApp);
    char buff[GBL_VERSION_STRING_SIZE] = { '\0'};

    GBL_CTX_INFO("Application Info");
    GBL_CTX_PUSH();
    GBL_CTX_INFO("%-20s: %-100.100s", "Name",         GblObject_name(GBL_OBJECT(pApp)));
    GBL_CTX_INFO("%-20s: %-100.100s", "Version",      GblVersion_string(pSelf_->version, buff));
    GBL_CTX_INFO("%-20s: %-100.100s", "Organization", pSelf_->pOrganization? " " : pSelf_->pOrganization);
    GBL_CTX_INFO("%-20s: %-100.100s", "Domain",       pSelf_->pDomain? " " : pSelf_->pDomain);
    GBL_CTX_INFO("Command-Line Args");
    GBL_CTX_PUSH();

    GblSize a = 0;
    for(GblStringList* pIt = pSelf_->pArgs->ringNode.pNext;
        pIt != pSelf_->pArgs;
        pIt = pIt->ringNode.pNext)
    {
        GBL_CTX_INFO("%-20u: %-100.100s", a++, pIt->pData);
    }

    GBL_CTX_POP(2);

    GBL_CTX_INFO("LibGimbal Info");
    GBL_CTX_PUSH();
    GBL_CTX_INFO("%-20s: %-100.100s", "Version", GIMBAL_VERSION);
    GBL_CTX_POP(1);

    GBL_CTX_INFO("Project Info");
    GBL_CTX_PUSH();
    GBL_CTX_INFO("%-20s: %-100.100s", "Name",        GBL_PROJECT_NAME);
    GBL_CTX_INFO("%-20s: %-100.100s", "Version",     GBL_PROJECT_VERSION);
    GBL_CTX_INFO("%-20s: %-100.100s", "URL",         GBL_PROJECT_URL);
    GBL_CTX_INFO("%-20s: %-100.100s", "Description", GBL_PROJECT_DESCRIPTION);
    GBL_CTX_POP(1);

    GBL_CTX_INFO("Build Info");
    GBL_CTX_PUSH();
    GBL_CTX_INFO("%-20s: %-100.100s", "Timestamp", GBL_BUILD_TIMESTAMP);
    GBL_CTX_INFO("%-20s: %-100.100s", "Type",      GBL_BUILD_TYPE);
    GBL_CTX_INFO("%-20s: %-100.100s", "Config",    GBL_BUILD_CONFIG);
    GBL_CTX_POP(1);

    GBL_CTX_INFO("CI Info");
    GBL_CTX_PUSH();
    #if GBL_BUILD_CI
    GBL_CTX_INFO("%-20s: %-100.100s", "Project", GBL_BUILD_CI_PROJECT_TITLE);

    GBL_CTX_INFO("Source Control Commit Info");
    GBL_CTX_PUSH();
    GBL_CTX_INFO("%-20s: %-100.100s", "Title", GBL_BUILD_CI_COMMIT_TITLE);
    GBL_CTX_INFO("%-20s: %-100.100s", "Timestamp", GBL_BUILD_CI_COMMIT_TIMESTAMP);
    GBL_CTX_INFO("%-20s: %-100.100s", "SHA", GBL_BUILD_CI_COMMIT_SHA);
    GBL_CTX_INFO("%-20s: %-100.100s", "Tag", GBL_BUILD_CI_COMMIT_TAG);
    GBL_CTX_INFO("%-20s: %-100.100s", "Ref", GBL_BUILD_CI_COMMIT_BRANCH);
    GBL_CTX_POP(1);

    GBL_CTX_INFO("Job Info");
    GBL_CTX_PUSH();
    GBL_CTX_INFO("%-20s: %-100.100s", "ID", GBL_BUILD_CI_JOB_ID);
    GBL_CTX_INFO("%-20s: %-100.100s", "Name", GBL_BUILD_CI_JOB_NAME);
    GBL_CTX_INFO("%-20s: %-100.100s", "Stage", GBL_BUILD_CI_JOB_STAGE);
    GBL_CTX_POP(1);

    GBL_CTX_INFO("User Info");
    GBL_CTX_PUSH();
    GBL_CTX_INFO("%-20s: %-100.100s", "Login", GBL_BUILD_CI_USER_LOGIN);
    GBL_CTX_INFO("%-20s: %-100.100s", "Name", GBL_BUILD_CI_USER_NAME);
    GBL_CTX_INFO("%-20s: %-100.100s", "Email Address", GBL_BUILD_CI_USER_EMAIL);
    GBL_CTX_POP(1);

    GBL_CTX_INFO("Build Node Info");
    GBL_CTX_PUSH();
    GBL_CTX_INFO("%-20s: %-100.100s", "Description", GBL_BUILD_CI_RUNNER_DESCRIPTION);
    //GBL_CTX_INFO("%-20s: %-100.100s", "Tags", GBL_BUILD_CI_RUNNER_TAGS);
    GBL_CTX_INFO("%-20s: %-100.100s", "Architecture", GBL_BUILD_CI_RUNNER_ARCH);
    GBL_CTX_POP(1);
    #else
    GBL_CTX_WARN("UNOFFICIAL LOCAL BUILD !");
    #endif
    GBL_CTX_POP(1);

    GBL_CTX_INFO("Compiler Info");
    GBL_CTX_PUSH();
    GBL_CTX_INFO("C");
    GBL_CTX_PUSH();
    GBL_CTX_INFO("%-20s: %-100.100s", "ID", GBL_BUILD_C_COMPILER_ID);
    GBL_CTX_INFO("%-20s: %-100.100s", "Version", GBL_BUILD_C_COMPILER_VERSION);
    GBL_CTX_INFO("%-20s: %-100.100s", "Target", GBL_BUILD_C_COMPILER_TARGET);
    GBL_CTX_INFO("%-20s: %-100.100s", "Toolchain", GBL_BUILD_C_COMPILER_TOOLCHAIN);
    GBL_CTX_INFO("%-20s: %-100.100s", "Language Standard", GBL_BUILD_C_STANDARD);
    GBL_CTX_INFO("%-20s: %-100.100s", "Language Extensions", GBL_BUILD_C_EXTENSIONS);
    GBL_CTX_POP(1);

    GBL_CTX_INFO("C++");
    GBL_CTX_PUSH();
    GBL_CTX_INFO("%-20s: %-100.100s", "ID", GBL_BUILD_CPP_COMPILER_ID);
    GBL_CTX_INFO("%-20s: %-100.100s", "Version", GBL_BUILD_CPP_COMPILER_VERSION);
    GBL_CTX_INFO("%-20s: %-100.100s", "Target", GBL_BUILD_CPP_COMPILER_TARGET);
    GBL_CTX_INFO("%-20s: %-100.100s", "Toolchain", GBL_BUILD_CPP_COMPILER_TOOLCHAIN);
    GBL_CTX_INFO("%-20s: %-100.100s", "Language Standard", GBL_BUILD_CPP_STANDARD);
    GBL_CTX_INFO("%-20s: %-100.100s", "Language Extensions", GBL_BUILD_CPP_EXTENSIONS);
    GBL_CTX_POP(1);
    GBL_CTX_POP(1);

    GBL_CTX_INFO("Environment Info");
    GBL_CTX_PUSH();
    GBL_CTX_INFO("Host");
    GBL_CTX_PUSH();
    GBL_CTX_INFO("%-20s: %-100.100s", "Host Name", GBL_BUILD_HOST_NAME);
    //GBL_CTX_VERBOSE("%-20s: %-100.100s", "Domain Name", GBL_BUILD_HOST_DOMAIN_NAME);
    GBL_CTX_INFO("%-20s: %-100.100s", "Operating System", GBL_BUILD_HOST_OS);
    GBL_CTX_INFO("%-20s: %-100.100s", "Architecture", GBL_BUILD_HOST_ARCH);
    GBL_CTX_INFO("Processor");
    GBL_CTX_PUSH();
    GBL_CTX_INFO("%-20s: %-100.100s", "Family", GBL_BUILD_HOST_PROCESSOR_NAME);
    GBL_CTX_INFO("%-20s: %-100.100s", "Description", GBL_BUILD_HOST_PROCESSOR_DESCRIPTION);
    GBL_CTX_INFO("%-20s: %-100.u", "Physical Cores", GBL_BUILD_HOST_PROCESSOR_CORES_PHYSICAL);
    GBL_CTX_INFO("%-20s: %-100.u", "Logical Cores", GBL_BUILD_HOST_PROCESSOR_CORES_LOGICAL);
    GBL_CTX_POP(1);
    GBL_CTX_INFO("Physical Memory");
    GBL_CTX_PUSH();
    GBL_CTX_INFO("%-20s: %-100u", "Total (MB)", GBL_BUILD_HOST_MEMORY_PHYSICAL_TOTAL);
    GBL_CTX_INFO("%-20s: %-100u", "Available (MB)", GBL_BUILD_HOST_MEMORY_PHYSICAL_AVAILABLE);
    GBL_CTX_POP(1);
    GBL_CTX_POP(1);
    GBL_CTX_INFO("Target");
    GBL_CTX_PUSH();
    GBL_CTX_INFO("%-20s: %-100.100s", "Operating System", GBL_BUILD_TARGET_OS);
    GBL_CTX_INFO("%-20s: %-100.100s", "Architecture", GBL_BUILD_TARGET_ARCH);
    GBL_CTX_INFO("%-20s: %-100u", "Word Size", GBL_PTR_SIZE * 8);
    GBL_CTX_INFO("%-20s: %-100.100s", "Endianness", GBL_BIG_ENDIAN? "Big" : "Little");
    GBL_CTX_POP(2);

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

