#include <gimbal/core/gimbal_library.h>

typedef struct GblLibrary {
    GblStringRef* pName;
    GblStringRef* pPath;
    GblBool       resident;
} GblLibrary;


