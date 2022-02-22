#ifndef GIMBAL_INAMED_H
#define GIMBAL_INAMED_H

#include "gimbal_icomponent.h"

#define INamed_IFACE                                                    \
    vfuncDefault(GBL_RESULT, nameGet, const VSelf, const char** ppStr)  \
    vfuncDefault(GBL_RESULT, nameSet, VSelf,       const char* pStr)

#define INamed_EXTENDS (IComponent)

interface(INamed);


GBL_RESULT INamed_nameGet(const VSelf, const char** ppStr) {
    return GBL_RESULT_UNIMPLEMENTED;
}

GBL_RESULT INamed_nameSet(VSelf, const char* pStr) {
    return GBL_RESULT_UNIMPLEMENTED;
}

typedef struct CNamed {
    INamed parent;
    char name[100];
} CNamed;

GBL_RESULT CNamed_nameGet(const VSelf, const char** ppStr) {
    VSELF(const CNamed);
    GBL_API_BEGIN(self);
    GBL_API_VERIFY_POINTER(ppStr);
    *ppStr = self->name;
    GBL_API_END();
}

GBL_RESULT CNamed_nameSet(VSelf, const char* pStr) {
    VSELF(CNamed);
    GBL_API_BEGIN(self);
    GBL_API_VERIFY(pStr);
    strncpy(self->name, pStr, sizeof(self->name));
    GBL_API_END();
}

impl(INamed, CNamed)




#endif // GIMBAL_INAMED_H
