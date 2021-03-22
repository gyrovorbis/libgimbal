#ifndef GIMBAL_CONTAINER_H
#define GIMBAL_CONTAINER_H

#ifdef __cplusplus
extern "C" {
#endif

#include "gimbal_types.h"
#include "gimbal_variant.h"


GBL_DECLARE_ENUM(GBL_CONTAINER_TYPE) {
    GBL_CONTAINER_TYPE_ARRAY,
    GBL_CONTAINER_TYPE_MAP,
    GBL_CONTAINER_TYPE_LIST
};

///CONTAINER TYPES
/// 1) random access (array, map, etc)
/// 2) forward iterable (pretty much everything)
/// 3)


struct GblContainer;
struct GblContainerIterator;

typedef GBL_RESULT (*GblContainerVTableSize)(const struct GblContainer* pCont, GblSize* pSize);
//typedef GBL_RESULT (*GblContainerVTableNext)(const GblContainer* pCont, GblContainerIterator* pIterator);
typedef GBL_RESULT (*GblContainerVTableGet)(const struct GblContainer* pCont, const void* pKey, void** pValue);
typedef GBL_RESULT (*GblContainerVTableSet)(const struct GblContainer* pCont, const void* pKey, const void* pValue);

typedef GBL_RESULT (*GblContainerVTableIteratorBegin)(const struct GblContainer* pCont, struct GblContainerIterator* pIterator);
typedef GBL_RESULT (*GblContainerVTableIteratorEnd)(const struct GblContainer* pCont, struct GblContainerIterator* pIterator);
typedef GBL_RESULT (*GblContainerVTableIteratorIncrement)(const struct GblContainer* pCont, struct GblContainerIterator* pIterator);
typedef GBL_RESULT (*GblContainerVTableIteratorCompare)(const struct GblContainer* pCont, const struct GblContainerIterator* pIterator1, const struct GblContainerIterator* pIterator2);

typedef GBL_RESULT (*GblContainerVTableIteratorData)(const struct GblContainer* pCont, const struct GblContainerIterator* pIterator,
                                                     void** pKey, void** pVal);

typedef GBL_RESULT (*GblContainerVTableAppend)(const struct GblContainer* pCont, const void* pValue);
typedef GBL_RESULT (*GblContainerVTableRemove)(const struct GblContainer* pCont, const void* pKey);


//typedef GBL_RESULT (*GblContainer)

typedef struct GblContainer {
    GblContext*         pContext;
    GBL_VARIANT_TYPE    keyType;
    GBL_VARIANT_TYPE    valueType;
    GblSize             keySize;
    GblSize             valueSize;
} GblContainer;

typedef struct GblContainerIterator {
    GblContainer*   pCont;
    void*           pPayload;
} GblContainerIterator;


GBL_RESULT gblContainerSize(GblContainer* pCont, GblSize* pSize);
GBL_RESULT gblContainerClear(GblContainer* pCont);
GBL_RESULT gblContainerNext(GblContainer* pCont, GblContainerIterator* pPrev);


typedef struct GblVector {
    GblContainer    container;
    GblSize         size;
    GblSize         capacity;
    void*           pData;
} GblVector;

#define GBL_VECTOR_INDEX_INVALID GBL_SIZE_MAX

GBL_RESULT gblVectorInit(GblVector* pVector, GblContext* pCtx, GblSize valueSize, const void* pInitialData, GblSize initialSize, GblSize initialCapacity);
// set comparator for for find?
GBL_RESULT gblVectorUninit(GblVector* pVector);

GBL_RESULT gblVectorSize(const GblVector* pVector, GblSize* pSize);

GBL_RESULT gblVectorAt(const GblVector* pVector, GblSize index, void** ppEntry);
GBL_RESULT gblVectorFront(const GblVector* pVector, void** ppEntry);
GBL_RESULT gblVectorBack(const GblVector* pVector, void** ppEntry);
GBL_RESULT gblVectorData(const GblVector* pVector, void** ppData);

GBL_RESULT gblVectorFind(const GblVector* pVector, const void* pEntryValue, GblSize* pIndex);

GBL_RESULT gblVectorPushBack(GblVector* pVector, const void* pData);
GBL_RESULT gblVectorPushFront(GblVector* pVector, const void* pData);
GBL_RESULT gblVectorErase(GblVector* pVector, GblSize index);




#ifdef __cplusplus
}
#endif

#endif // GIMBAL_CONTAINER_H
