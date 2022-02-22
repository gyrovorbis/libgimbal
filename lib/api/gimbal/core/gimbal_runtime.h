#ifndef GIMBAL_RUNTIME_H
#define GIMBAL_RUNTIME_H

#include "gimbal_api.h"

#include <stdatomic.h>

GBL_DECLARE_OPAQUE(GblRuntime);
GBL_DECLARE_OPAQUE(GblModule);




struct GblMetaType;

typedef struct GblRuntime_ {

    uint_fast32_t typeCount;



} GblRuntime_;


GBL_API gblRuntimeCreate(GblRuntime* phRt);
GBL_API gblRuntimeDestroy(GblRuntime);

// Module API
GBL_API gblRuntimeModuleOpen(GblRuntime hRt, GblModule hModule);
GBL_API gblRuntimeModuleClose(GblRuntime hRt, GblModule hModule);
GBL_API gblRuntimeModuleFind(GblRuntime hRt, const char* pName, GblModule* phModule);

// Child Contexts and Types
GBL_API gblRuntimeContextFind(GblRuntime hRt, const char* pName, GblContext* phCtx);
GBL_API gblRuntimeContextPush(GblRuntime hRt, GblContext hCtx);
GBL_API gblRuntimeContextPop(GblRuntime hRt);
//GBL_API gblRuntimeContextCurrent(GblRuntime hRt, GblContext* phCtx);

// Meta Type System
GBL_API gblRuntimeMetaTypeRegister(GblRuntime hRt, const GblMetaType* pMetaType);
GBL_API gblRuntimeMetaTypeUnregister(GblRuntime hRt, const GblMetaType* pMetaType);
GBL_API gblRuntimeMetaTypeFind(GblRuntime hRt, const char* pTypeName, const GblMetaType** ppMetaType);

// Virtual Stack API
//GBL_API gblRuntimePush(GblRuntime hRt);
//GBL_API gblRuntimePop(GblRuntime hRt);

GBL_API gblRuntimeGlobalFind(GblRuntime hRt, const char* pGlobal);

// Serialization
GBL_API gblRuntimeSerialize(GblRuntime hRt, GblVariant v);
GBL_API gblRuntimeDeserialize(GblRuntime hRt, GblVariant v);

// Script API
GBL_API gblRuntimeExec(GblRuntime hRt, const char* pString);
GBL_API gblRuntimeScript(GblRuntime hRt, const char* pScript);

// Reference/Memory Management API
//GBL_API gblRuntimeRefCreate(GblRuntime hRt, GblRef* phRef);
//GBL_API gblRuntimeRefDestroy(GblRuntime hRt, GblRef ref);

//GBL_API gblRuntimeStackTrace()
//GBL_API gblRuntimeConfigOptions()
//GBL_API gblRuntimeCmdLineParse(GblRuntime hRt, int argc, char* argv[]);
//GBL_API gblRuntimeCrashReporter();

// Child Thread API
//GBL_API gblRuntimeThreadCreate();
//GBL_API gblRuntimeThreadDestroy();


#endif // GIMBAL_RUNTIME_H
