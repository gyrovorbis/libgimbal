#ifndef GIMBAL_TABLE_H
#define GIMBAL_TABLE_H

#include "../gimbal_hashmap.h"
#include "../gimbal_variant.h"

#ifdef __cplusplus
extern "C" {
#endif

#define GBL_API_VERIFY_TABLE(table) GBL_API_VERIFY_HASH_MAP(table)

typedef struct GblTable {
    GblHashMap hashBase;
} GblTable;

typedef struct GblTableField {
    GblVariant key;
    GblVariant value;
} GblTableField;

GBL_INLINE uint64_t gblTableHash_(const void* pItem, uint64_t seed0, uint64_t seed1) {
    uint64_t hash = 0;
    GblTableField* pField = pItem;
    GBL_API_BEGIN(NULL);

    if(pField) hash = hashmap_murmur(&pField->key, sizeof(GblVariant), seed0, seed1);

    GBL_API_VERIFY(hash, GBL_RESULT_ERROR_UNHANDLED_EXCEPTION, "Hash could not be generated for variant!");
    GBL_API_END_BLOCK();
    return hash;
}

GBL_INLINE uint64_t gblTableCompare_(const void* pA, const void* pB, void* pUdata) {
    GblBool equal = GBL_FALSE;
    uint64_t diff = UINTPTR_MAX;
    GblTableField* pFieldA = pA;
    GblTableField* pFieldB = pB;
    GBL_API_BEGIN(NULL);
    if(pFieldA && pFieldB) {
        GBL_API_CALL(gblVariantCompare(pFieldA.key, pFieldB->key, GBL_VARIANT_OP_CMP_EQUAL, &equal));
    }
    if(equal) diff = 0;
    GBL_API_END_LABEL();
    return 0;
}

GBL_INLINE void gblTableElementFree_(void* pItem) {
    GblTableField* pField = pItem;
    GBL_API_BEGIN(NULL);
    if(pField) {
        if(pField->key)     GBL_API_CALL(gblVariantSetNil(&pField->key));
        if(pField->value)   GBL_API_CALL(gblVariantSetNil(&pField->value));
    }
    GBL_API_END();
}

GBL_INLINE GBL_RESULT gblTableInit(GblTable* pTable,
                                   GblContext hCtx,
                                   GblSize capacity)
{
    GBL_API_BEGIN(hCtx);
    GBL_API_CALL(gblHashMapInit(pTable, hCtx, sizeof(GblTableEntry), capacity,
                   &gblTableHash_, &gblTableCompare_, &gblTableElementFree_));
    GBL_API_END();
}

GBL_INLINE GBL_RESULT gblTableFinalize(GblTable* pTable) {
    return gblHashMapFinalize(pTable);
}

GBL_INLINE GBL_RESULT gblTableValueGet(GblTable* pTable, const GblVariant* pKey, GblVariant* pValue) {
    GblTableField* pField = NULL;
    GBL_ASSERT(pTable);
    GBL_API_BEGIN(pTable->hashBase.hCtx);
    GBL_API_VERIFY_POINTER(pKey);
    GBL_API_VERIFY_POINTER(pValue);
    GBL_API_CALL(gblHashMapGet(pTable, pKey, &pField));
    if(pField) {
        GBL_API_CALL(gblVariantCopy(&pField->value, pValue));
    } else {
        GBL_API_CALL(gblVariantSetNil(pValue));
    }
    GBL_API_END();
}

// CHANGE ME TO MOVE SEMANTICS!!!
GBL_INLINE GBL_RESULT gblTableValueSet(GblTable* pTable, const GblVariant* pKey, const GblVariant* pValue) {
    GblTableField field;
    GBL_VARIANT_TYPE valueType = GBL_VARIANT_TYPE_NIL;
    void* pData = NULL;
    GBL_ASSERT(pTable);
    GBL_API_BEGIN(pTable->hashBase.hCtx);
    GBL_API_VERIFY_POINTER(pKey);
    GBL_API_CALL(gblVariantCopy(&field.key, pKey));
    if(pValue) {
        GBL_API_CALL(gblVariantTypeGet(pValue, &valueType));
    }
    if(valueType != GBL_VARIANT_TYPE_NIL) {
        GBL_API_CALL(gblVariantCopy(&field.value, pValue));
        GBL_API_CALL(gblHashMapSet(pTable, &field, pData));
    } else {
        GBL_API_CALL(gblTableKeyRemove(pTable, pKey));
    }
    GBL_API_END();
}

GBL_INLINE GBL_RESULT gblTableFieldRemove(GblTable* pTable, GblVariant* pKey) {
    void* pData = NULL;
    GBL_ASSERT(pTable);
    GBL_API_BEGIN(pTable->hashBase.hCtx);
    GBL_API_VERIFY_POINTER(pKey);
    GBL_API_CALL(gblHashMapDelete(pTable, pKey, &pData));
    GBL_API_END();
}

GBL_INLINE GBL_RESULT gblTableFieldAt(GblTable* pTable, uint64_t index, GblTableField** ppField) {
    return gblHashMapProbe(pTable, index, ppField);
}

GBL_INLINE GBL_RESULT gblTableClear(GblTable* pTable, GblBool updateCapacity) {
    return gblHashMapClear(pTable, updateCapacity);
}

GBL_INLINE GBL_RESULT gblTableFieldCount(const GblTable* pTable, GblSize* pSize) {
    return gblHashMapCount(pTable, pSize);
}

GBL_INLINE GBL_RESULT gblTableOom(GblTable* pTable, GblBool* pValue) {
    return gblHashMapOom(pTable, pValue);
}

GBL_API GBL_RESULT gblTableIterate(GblTable* pTable, bool (*pFnIter)(const TableField *pField, void *pUData), void *pUdata, GblBool* pResult) {
    return gblHashMapScan(pTable, pFnIter, pUdata, pResult);
}


#ifdef __cplusplus
}
#endif

#endif // GIMBAL_TABLE_H
