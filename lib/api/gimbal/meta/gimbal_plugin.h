#ifndef GIMBAL_PLUGIN_H
#define GIMBAL_PLUGIN_H

#include "gimbal_interface.h"

#define SELF    GblPlugin* pSelf
#define CSELF   const GblPlugin* pSelf

GBL_DECLS_BEGIN

struct GblPlugin;

typedef GBL_RESULT (*GblPluginUseFn)        (SELF);
typedef GBL_RESULT (*GblPluginUnuseFn)      (SELF);
typedef GBL_RESULT (*GblPluginTypeInfoFn)   (CSELF, GblType, GblTypeInfo*);


typedef struct GblPluginClass {
    GblInterface            base;
    GblPluginUseFn          pFnUse;
    GblPluginUnuseFn        pFnUnuse;
    GblPluginTypeInfoFn     pFnTypeInfo;
} GblPluginClass;


typedef struct GblPlugin {
    union {
        GblPluginClass*     pVTable;
        GblInterface        base;
    };
} GblPlugin;

GBL_INLINE GBL_RESULT GblPlugin_use         (SELF);
GBL_INLINE GBL_RESULT GblPlugin_unuse       (SELF);
GBL_INLINE GBL_RESULT GblPlugin_typeInfo    (CSELF, GblType requestedType, GblTypeInfo* pInfo);






GBL_INLINE GBL_RESULT GblPlugin_use(SELF) {
    if(!pSelf)                  return GBL_ERROR_INVALID_INSTANCE;
    if(!pSelf->pVTable)         return GBL_ERROR_INVALID_CLASS;
    if(!pSelf->pVTable->pFnUse) return GBL_ERROR_INVALID_VIRTUAL_CALL
    return pSelf->pVTable->pFnUse(pSelf);
}
GBL_INLINE GBL_RESULT GblPlugin_unuse(SELF) {
    if(!pSelf)                      return GBL_ERROR_INVALID_INSTANCE;
    if(!pSelf->pVTable)             return GBL_ERROR_INVALID_CLASS;
    if(!pSelf->pVTable->pFnUnuse)   return GBL_ERROR_INVALID_VIRTUAL_CALL;
    return pSelf->pVTable->pFnUnuse(pSelf);
}
GBL_INLINE GBL_RESULT GblPlugin_typeInfo(CSELF, GblType requestedType, GblTypeInfo* pInfo) {
    if(!pSelf)                          return GBL_ERROR_INVALID_INSTANCE;
    if(!pSelf->pVTable)                 return GBL_ERROR_INVALID_CLASS;
    if(!pSelf->pVTable->pFntypeInfo)    return GBL_ERROR_INVALID_VIRTUAL_CALL;
    return pSelf->pVTable->pFTypeInfo(pSelf, requesedType, pInfo);
}


GBL_DECLS_END

#undef SELF
#undef CSELF


#endif // GIMBAL_PLUGIN_H
