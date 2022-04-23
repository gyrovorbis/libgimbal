#ifndef GIMBAL_PLUGIN_H
#define GIMBAL_PLUGIN_H

#include "gimbal_interface.h"

GBL_DECLS_BEGIN

#define GBL_IPLUGIN_TYPE (GblIPlugin_type())
#define GBL_IPLUGIN(instance)                GBL_TYPE_CAST_INSTANCE(instance, GBL_IPLUGIN_TYPE, GblIPlugin)
#define GBL_IPLUGIN_CHECK(instance)    GBL_TYPE_CHECK_INSTANCE(instance, GBL_IPLUGIN_TYPE)
#define GBL_IPLUGIN_IFACE(klass)             GBL_TYPE_CAST_CLASS(klass, GBL_IPLUGIN_TYPE, GblIPluginIFace)
#define GBL_IPLUGIN_IFACE_CHECK(klass)  GBL_TYPE_CHECK_CLASS(klass, GBL_IPLUGIN_TYPE)
#define GBL_IPLUGIN_GET_IFACE(instance)      GBL_TYPE_CAST_GET_CLASS(instance, GBL_IPLUGIN_TYPE, GblIPluginIFace)

#define SELF    GblIPlugin* pSelf
#define CSELF   const GblIPlugin* pSelf


typedef GBL_RESULT (*GblIPluginUseFn)        (SELF);
typedef GBL_RESULT (*GblIPluginUnuseFn)      (SELF);
typedef GBL_RESULT (*GblIPluginTypeInfoFn)   (CSELF, GblType, GblTypeInfo*);


typedef struct GblIPluginIface {
    GblInterface            base;
    GblIPluginUseFn         pFnUse;
    GblIPluginUnuseFn       pFnUnuse;
    GblIPluginTypeInfoFn    pFnTypeInfo;
} GblIPluginIFace;



GBL_EXPORT GblType    GblIPlugin_type        (void);
GBL_EXPORT GBL_RESULT GblIPlugin_use         (SELF);
GBL_EXPORT GBL_RESULT GblIPlugin_unuse       (SELF);
GBL_EXPORT GBL_RESULT GblIPlugin_typeInfo    (CSELF, GblType requestedType, GblTypeInfo* pInfo);


GBL_EXPORT GblType GblIPlugin_type(void) {
    static GblType type = GBL_TYPE_INVALID;
    if(type == GBL_TYPE_INVALID) {
        type = gblTypeRegisterStatic(GBL_TYPE_INTERFACE,
                                     "IPlugin",
                                     &((const GblTypeInfo) {
                                         .classSize    = sizeof(GblIPluginIFace),
                                         .classAlign   = GBL_ALIGNOF(GblIPluginIFace),
                                     }),
                                     GBL_TYPE_FUNDAMENTAL_FLAG_CLASSED | GBL_TYPE_FLAG_ABSTRACT);
    }
    return type;
}



GBL_INLINE GBL_RESULT GblIPlugin_use(SELF) {
    GBL_API_BEGIN(NULL); {
        GblIPluginIFace* pIFace = GBL_IPLUGIN_GET_IFACE(pSelf);
        GBL_API_VERIFY_POINTER(pIFace->pFnUse);
        GBL_API_CALL(pIFace->pFnUse(pSelf));

    } GBL_API_END();
}
GBL_INLINE GBL_RESULT GblIPlugin_unuse(SELF) {
    GBL_API_BEGIN(NULL); {
        GblIPluginIFace* pIFace = GBL_IPLUGIN_GET_IFACE(pSelf);
        GBL_API_VERIFY_POINTER(pIFace->pFnUnuse);
        GBL_API_CALL(pIFace->pFnUnuse(pSelf));

    } GBL_API_END();
}
GBL_INLINE GBL_RESULT GblIPlugin_typeInfo(CSELF, GblType requestedType, GblTypeInfo* pInfo) {
    GBL_API_BEGIN(NULL); {
        GblIPluginIFace* pIFace = GBL_IPLUGIN_GET_IFACE(pSelf);
        GBL_API_VERIFY_POINTER(pIFace->pFnTypeInfo);
        GBL_API_CALL(pIFace->pFnTypeInfo(pSelf, requestedType, pInfo));

    } GBL_API_END();
}


GBL_DECLS_END

#undef SELF
#undef CSELF


#endif // GIMBAL_PLUGIN_H
