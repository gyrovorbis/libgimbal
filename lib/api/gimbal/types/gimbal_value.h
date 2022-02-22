#ifndef GIMBAL_VALUE_H
#define GIMBAL_VALUE_H

#include "gimbal_type.h"

#ifdef __cplusplus
extern "C" {
#endif

struct GblValue {
    GblType             gblType;
    union {
        GblType         typeValue;
        GblBool         boolean;
        GblInt          integer;
        GblUint         uinteger;
        GblEnum         enumValue;
        GblFlags        flagsValue;
        uint64_t        uinteger64;
        GblFloat        floating;
        GblFunction     function;
        void*           pVoidPtr;
        GblString*      pString;
        GblInstance*    pInstance;
        GblObject*      pObject;
    };
};

typedef GBL_RESULT (*GblValueTransformFn)(const GblValue*, GblValue*);

GBL_API gblValueTypeCompatible(GblType fromType, GblType toType);
GBL_API gblValueTypeTransformable(GblType fromType, GblType toType);
GBL_API gblValueTypeTransformRegister(GblType fromType, GblType toType, GblValueTransformFn pFnTransform);

//initialized to NIL
//overloads from hell to initialize with whatever
GBL_API gblValueInit(GblValue* pValue, GblType gblType);
GBL_API gblValueInitInstance(GblValue* pValue, GblInstance* pInstance); // initializes with EXACT type of pInstance
GBL_API gblValueFinalize(GblValue* pValue); //releases internal data, equivalent to destructor
GBL_API gblValueReset(GblValue* pValue); //clears value as if you just took it
GBL_API gblValueType(GblValue* pValue, GblType* pType);
GBL_API gblValueCopy(const GblValue* pSrc, GblValue* pDst);
GBL_API gblValueTransform(const GblValue* pSrc, GblValue* pDst);

GBL_API gblValueGetp(const GblValue* pValue, void** pPtr);
GBL_API gblValueGetf(const GblValue* pValue, GblFloat* pFloat);
GBL_API gblValueGeti(const GblValue* pValue, GblInt* pInteger);
GBL_API gblValueGetEnum(GblValue* pValue, GblEnum* pEnum);
GBL_API gblValueGetFlags(GblValue* pValue, GblFlags* pFlags);
GBL_API gblValueGetType(GblValue* pValue, GblType* pType);
GBL_API gblValueGetInstance(const GblValue* pValue, GblInstance* pInstance);
GBL_API gblValueGetObject(const GblValue* pValue, GblObject* pObject);
GBL_API gblValueGetVariant(const GblValue* pValue, GblVariant* pVariant);

// shit only works if it is currently of the correct type!!
GBL_API gblValueSetp(GblValue* pValue, void* pPtr);
GBL_API gblValueSetf(GblValue* pValue, GblFloat floating);
GBL_API gblValueSeti(GblValue* pValue, GblInt* integer);
GBL_API gblValueSets(GblValue* pValue, const char* pString);
GBL_API gblValueSetEnum(GblValue* pValue, GblEnum value);
GBL_API gblValueSetFlags(GblValue* pValue, GblFlags flags);
// INCREASES REFERENCE COUNT
GBL_API gblValueSetInstance(GblValue* pValue, GblInstance* pInstance);
GBL_API gblValueSetVariant(GblValue* pValue, GblVariant* pVariant); // consumed if floating type
GBL_API gblValueSetType(GblValue* pValue, GblType typeValue); // SETS CONTENTS, DOESN'T CHANGE TYPE!!!
GBL_API gblValueSetObject(GblValue* pValue, GblObject* pObject);

// ==== MOVE SEMANTICS ====

// takes ownership of value types and releases internal reference for move semantics
GBL_API gblValueTakes(GblValue* pValue, char* pString);
GBL_API gblValueTakeObject(GblValue* pValue, GblObject* pObject);
GBL_API gblValueTakeVariant(GblValue* pValue, GblVariant* pVariant);

// assumes ownership of reference types!
GBL_API gblValueGives(GblValue* pValue, const char** ppString);
GBL_API gblValueTakeObject(GblValue* pValue, GblObject* pObject);
GBL_API gblValueTakeVariant(GblValue* pValue, GblVariant* pVariant);

// ==== / MOVE SEMANTICS ====




#ifdef __cplusplus
}
#endif



#endif // GIMBAL_VALUE_H
