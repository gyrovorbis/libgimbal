#include <gimbal/meta/gimbal_type.h>

typedef union GblTypeInfo {
    GblTypeBaseInfo             base;
    GblTypeFundamentalInfo      fundamental;
    GblTypeClassInfo            klass;
} GblTypeInfo;

typedef struct GblMetaType_ {
    GblRefCount         refCount;
#ifdef GBL_TYPE_DEBUG
    GblRefCount         instanceRefCount;
#endif
    GblTypeInfo         info;
    GblString           name;
    void*               pModule; //module
    void*               pClass;
} GblMetaType_;


typedef struct GblTypeState {
    GblVector       staticTypes;
    GblVector       fundamentalTypes;
    uint32_t        lastFundamentalType;
} GblTypeState;

static GblTypeState     typeStateDefault_;
static GblTypeState*    pTypeState_  = &typeStateDefault_;


#define GBL_TYPE_ID_MASK    ((GblType)((1 << GBL_TYPE_FUNDAMENTAL_SHIFT) - 1))

static inline GblMetaType_* gblMetaFromType_(GblType typeId) {
    void* pEntry = NULL;
    GBL_API_BEGIN(NULL);
    if(typeId > GBL_TYPE_FUNDAMENTAL_MAX) {
        pEntry = (GblMetaType*)(typeId & GBL_TYPE_ID_MASK);
    } else {
        GBL_API_CALL(gblVectorAt(&pTypeState_->fundamentalTypes, typeId >> GBL_TYPE_FUNDAMENTAL_SHIFT, &pEntry));
    }
    GBL_API_END();
    return pEntry;
}


static GBL_RESULT gblTypeStateInit_(void) {
    GBL_API_BEGIN_ONCE(NULL);
    memset(pTypeState_, 0, sizeof(GblTypeState));
    GBL_API_CALL(gblVectorConstruct(&pTypeState_->staticTypes, NULL, sizeof(GblMetaType)));
    GBL_API_CALL(gblVectorConstruct(&pTypeState_->fundamentalTypes, NULL, sizeof(GblMetaType)));
    GBL_API_END_ONCE();
}

static GBL_RESULT gblTypeStateDeinit_(void) {
#ifdef GBL_TYPE_DEBUG
        // print everything that still has reference counts!!!
#endif
}

static GBL_RESULT gblTypeRegisterBase_(const char*                            pTypeName,
                                       GblType*                               pNewType,
                                       GblVector*                             pVector)
{
    GBL_API_BEGIN(NULL);
    GBL_API_VERIFY_POINTER(pTypeName);

    {
        GblMetaType_ metaType = {
            .refCount           = 0,
#ifdef GBL_TYPE_DEBUG
            .instanceRefCount   = 0,
#endif
            .pModule            = NULL,
            .pClass             = NULL
        };

        GBL_API_CALL(gblStringConstruct(&metaType.name, 0, NULL, &(const GblStringView){
                                            pTypeName,
                                            0
                                        }));

        GBL_API_VERIFY_POINTER(memset(&metaType.info, 0, sizeof(GblTypeInfo)));

        GBL_API_CALL(gblVectorPushBack(pVector, &metaType));
    }

    GBL_API_END();
}


GBL_API gblTypeRegisterClass(const char*                            pTypeName,
                             const GblTypeClassInfo*                pClassInfo,
                             GblType*                               pNewType)
{
    GBL_API_BEGIN(NULL);
 //   gblTypeRegisterBase_(pTypeName,
    GBL_API_END();
}


GBL_API gblTypeRegisterFundamental(const char*                      pTypeName,
                                   const GblTypeFundamentalInfo*    pFundamentalInfo,
                                   GblType*                         pNewType) {


}


