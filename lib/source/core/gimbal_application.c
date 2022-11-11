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



