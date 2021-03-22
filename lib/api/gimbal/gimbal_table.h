#ifndef GIMBAL_TABLE_H
#define GIMBAL_TABLE_H

#include "gimbal_variant.h"

typedef GIM_RESULT (GimTableSerializer)(const GimTable* pTable, const char** ppPtr, GimSize* pSize);
typedef GIM_RESULT (GimTableDeserializer)(const GimTable* pTable, const char* pString);

//invalid key, METAMETHODS
typedef GIM_RESULT (*GimMetaMethodIndex)(const GimTable* pTable, const GimVariant* pKey, GimVariant** pValue);
typedef GIM_RESULT (*GimMetaMethodNewIndex)(const GimTable* pTable, const GimVariant* pKey, const GimVariant* pValue);
typedef GIM_RESULT (*GimMetaMethodDestroy)(const GimTable* pTable);
typedef GIM_RESULT (*GimMetaMethodLength)(const GimTable* pTable, GimSize* pSize);
typedef GIM_RESULT (*GimMetaMethodNext)(const GimTable* pTable, GimVariant* pKey, const GimVariant* pValue);
typedef GIM_RESULT (*GimMetaMethodCall)(const GimTable* pTable, GimVariant** pArgs); // Generic call, linked list vararg, what about return? Luastack semantics
typedef GIM_RESULT (*GimMetaMethodConvert)(const GimTable* pTable, GIM_VARIANT_TYPE toType, GimVariant* pVariant);
//comparison (eq, lt, le, etc)
//toString (and/or generic conversions to/from LuaVariant)
//Shit to make whatever Table intero with LuaVariant

GIM_DEFINE_ENUM(TABLE_TYPE) {
    GIM_TABLE_TYPE_NIL,
    GIM_TABLE_TYPE_TABLE,
    GIM_TABLE_TYPE_ARRAY,
    GIM_TABLE_TYPE_USERDATA
};

};

typedef struct GimMetaMethods {
    GimMetaMethodIndex      pIndex;
    GimMetaMethodNewIndex   pNewIndex;
    GimMetaMethodCall       pCall;
    GimMetaMethodDestroy    pDestroy;
    GimMetaMethodLength     pLength;
    GimMetaMethodNext       pNext;
    GimMetaMethodConvert    pConvert;
    //GimMetaMethodOp       pOp;
} GimMetaMethods;

typedef struct GimMetatable {
    const char*         pName;
    const char*         pDesc;
    GIM_TABLE_TYPE      tableType;
    GimMetaTable*       pParent;
    GimTableIndexCb     pIndexFn;
    GimTableNewIndexCb  pNewIndexFn;
    GimTableNextCb      pNextCb;
    GimTableDestroyCb   pDestroyFn;
    GimTableLengthCb    pLengthFn;
    GimCallMetaMethod   pCall;
} GimMetaTable;

typedef struct GimMetaMethods {
    GimMetaMethodIndex      pIndex;
    GimMetaMethodNewIndex   pNewIndex;
    GimMetaMethodCall       pCall;
    GimMetaMethodDestroy    pDestroy;
    GimMetaMethodLength     pLength;
    GimMetaMethodNext       pNext;
    GimMetaMethodConvert    pConvert;
    //GimMetaMethodOp       pOp;
} GimMetaMethods;

typedef struct GimMetatable {
    const char*         pName;
    const char*         pDesc;
    GIM_TABLE_TYPE      tableType;
    GimMetaTable*       pParent;
    GimTableIndexCb     pIndexFn;
    GimTableNewIndexCb  pNewIndexFn;
    GimTableNextCb      pNextCb;
    GimTableDestroyCb   pDestroyFn;
    GimTableLengthCb    pLengthFn;
    GimCallMetaMethod   pCall;
} GimMetaTable;

typedef struct GimTable {
    GimMetaTable* pMetaTable;
    GimTable*     pParent;
    void*         pData;
} GimTable;

GIM_API gimContextMetatableRegister(GimContext* pCtx, const GimMetaTable* pTable);

//GIM_API gimTableCreateArray();
//GIM_API gimTableCreateCopy();
//GIM_API gimTableCreateMetatable();

GIM_API gimTableCreate(GimContext* pCtx, GimTable** ppTable);
GIM_API gimTableCreateUserdata(GimContext* pCtx, void* pUserdata, GimMetaTable* pMeta, GimTable** ppTable);
// could also set initial values...
GIM_API gimTableDestroy(GimTable** ppTable);

GIM_API gimTableTypeName(const GimTable* pTable, char* pName, GimSize* pSize);

GIM_API gimTablePrint(GimTable* pTable);
GIM_API gimTableSerialize(GimTable* pTable, char* pData, GimSize* pSize);
GIM_API gimTableDeserialize(GimTable* pTable, const char* pData, GimSize pSize);
GIM_API gimTableApply(GimTable* pTable, const GimTable* pOther);
    //creates entries if doesn't exist, overwrites if does exist

GIM_API gimTableParent(const GimTable* pTable, GimTable** ppParent);
GIM_API gimTableParentSet(const GimTable* pTable, GimTable* pParent);

GIM_API gimTableMetatable(const GimTable* pTable, const GimMetaTable** ppMeta);
GIM_API gimTableMetatableSet(const GimTable* pTable, const GimMetaTable* pMeta);

GIM_API gimTableRaw(const GimTable* pTable, const GimVariant* pKey, GimVariant* pValue);
GIM_API gimTableRawSet(GimTable* pTable, const GimVariant* pKey, const GimVariant* pValue);

GIM_API gimTable(const GimTable* pTable, const GimVariant* pKey, GimVariant* pValue);
GIM_API gimTableSet(GimTable* pTable, const GimVariant* pKey, const GimVariant* pValue);

GIM_API gimTableField(const GimTable* pTable, const char* pFieldName, GimVariant* pValue);
GIM_API gimTableFieldSet(const GimTable* pTable, const char* pFieldName, const char* pValue);

GIM_API gimTableIndex(const GimTable* pTable, uint32_t index, GimVariant* pValue);
GIM_API gimTableIndexSet(const GimTable* pTable, uint32_t index, const GimVariant* pValue);

GIM_API gimTableNext(const GimTable* pTable, GimVariant* pKey, GimVariant* pValue);
GIM_API gimTableLength(const GimTable* pTable, GimSize* pSize);

GIM_API gimTableCall(const GimTable* pTable, GimVariant** pArgs, GimSize argCount);
GIM_API gimTableConvert(const GimTable* pTable, GIM_VARIANT_TYPE toType, GimVariant* pVariant);


//verify no leaks based on malloc and shit
class GimTableTestSet: public TestSet {



};



// compare, concat, deep copy, etc



#endif // GIMBAL_TABLE_H
