#include <gimbal/core/gimbal_application.h>
#include <gimbal/strings/gimbal_string_ref.h>
#include <gimbal/utils/gimbal_version.h>
#include <gimbal/strings/gimbal_string_list.h>

GBL_DECLARE_STRUCT_PRIVATE(GblApplication) {
    GblStringRef   name;
    GblStringRef   organization;
    GblStringRef   domain;
    GblVersion     version;
    GblStringList* pArgs;
};

static GblBool         startingUp_  = GBL_TRUE;
static GblBool         closingDown_ = GBL_FALSE;
static GblApplication* pInstance_   = NULL;
static GblStringList*  pLibPaths_   = NULL;

GBL_EXPORT GblBool GblApplication_startingUp(void) {
    return startingUp_;
}

GBL_EXPORT GblBool GblApplication_closingDown(void) {
    return closingDown_;
}

GBL_EXPORT GblApplication* GblApplication_instance(void) {
    return pInstance_;
}

GBL_EXPORT const GblStringList* GblApplication_libraryPaths(void) {
    if(!pLibPaths_) {
        pLibPaths_ = GblStringList_createEmpty();
    }
    return pLibPaths_;
}

GBL_EXPORT void GblApplication_addLibraryPath(const char* pPath) {
    if(!pLibPaths_) {
        pLibPaths_ = GblStringList_createEmpty();
    }
    GblStringList_pushBack(pLibPaths_, pPath);
}




