#ifndef GIMBAL_TYPE_H
#define GIMBAL_TYPE_H

#include <stdint.h>

#include "../types/gimbal_result.h"
#include "../core/gimbal_api_frame.h"

#ifdef __cplusplus
extern "C" {
#endif

#define GBL_VFUNC_NAME(Suffix) \
    GBL_CAT(pFnV, Suffix)


#define GBL_DECLARE_VFUNC_0(Suffix) \
    GBL_RESULT (*GBL_VFUNC_NAME(Suffix))(void)

#define GBL_DECLARE_VFUNC(Suffix, ...) \
    GBL_RESULT (*GBL_VFUNC_NAME(Suffix))(void* pSelf GBL_VA_ARGS(__VAR_ARGS__))


#define GBL_TYPE(Name)                                              \
    GBL_API_INLINE_HELPER((GblType, 0), Name##_TYPE, (void), {      \
        GBL_API_CALL(Name##_typeGet(&GBL_API_INLINE_RETVAL()));     \
    });

#define GBL_INVALID_TYPE            0

struct GblClass;

typedef size_t      GblSize;
typedef uintptr_t   GblType;
typedef GblFlags    GblTypeFlags;

typedef enum GBL_TYPE_CATEGORY {
    GBL_TYPE_CATEGORY_NONE,
    GBL_TYPE_CATEGORY_CLASS,
    GBL_TYPE_CATEGORY_INTERFACE,
    GBL_TYPE_CATEGORY_BOXED,
    GBL_TYPE_CATEGORY_PRIMITIVE,
    GBL_TYPE_CATEGORY_MAX
} GBL_TYPE_CATEGORY;

typedef struct GblTypeInfo {
    GBL_TYPE_CATEGORY   category; //identifier for union of other shit
    /* union {
           GblClassTypeInfo         classInfo;
           GblInterfaceTypeInfo     interfaceInfo;
           GblBoxedTypeInfo         boxedInfo;
           GblPrimitiveTypeInfo     primitiveInfo;
    };*/
    const char*         pName;
    GblType             typeId;
    GblTypeFlags        flags;
    GblSize             instanceSize;
    GblSize             instanceAlign;
} GblTypeInfo;


typedef struct GblMethod {
    const char*         pTag;
    GblFunction         pFnSelector;
    GblFunction         pFnMemberFunc;
} GblMethod;

struct GblTypeClass;

typedef struct GblInstance {
    struct GblTypeClass* pClass;
} GblInstance;

typedef struct GblTypeClass {
    GblInstance         base_;
    GBL_RESULT          (*pFnTypeInfoGet)   (const GblTypeInfo** pInfo);
    GBL_RESULT          (*pFnClassSuperGet) (uint32_t index, GblTypeClass* pClass);
} GblTypeClass;

typedef struct GblInstanceClass {
    GblTypeClass        base_;
    GBL_RESULT          (*pFnAsRefType)     (GblInstance* pSelf, GblType type, GblInstance** ppOut);
} GblInstanceClass;

typedef struct GblValueClass {
    GblInstanceClass    base_;
    GBL_RESULT          (*pFnAcquire)       (GblInstance* pSelf, GblInstance** ppOut);
    GBL_RESULT          (*pFnRelease)       (GblInstance* pSelf);
} GblValueClass;

typedef struct GblObjectClass {
    GblValueClass       base_;
    GBL_RESULT          (*pFnCtor)          (GblInstance* pSelf, va_list* pArgs);
    GBL_RESULT          (*pFnDtor)          (GblInstance* pSelf);
};





GBL_API GblType_nameGet (GblType type, const char** ppName);
GBL_API GblType_classGet(GblType type, const struct GblClass** ppClass);

#ifdef __cplusplus
}
#endif



#endif // GIMBAL_TYPE_H
