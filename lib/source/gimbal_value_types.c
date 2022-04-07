#include <gimbal/meta/gimbal_type.h>
#include <gimbal/core/gimbal_api_frame.h>
#include <gimbal/meta/gimbal_ivariant.h>
#include <gimbal/meta/gimbal_variant.h>

typedef struct GblPrimitiveClass {
    GblClass            base;
    GblIVariantIFace    iVariantIFace;
} GblPrimitiveClass;

static GBL_RESULT GblPrimitiveClass_init(GblPrimitiveClass* pClass, GblIVariantIFace* pIFace, GblContext hCtx) {
    GBL_API_BEGIN(hCtx);
    memcpy(&pClass->iVariantIFace.supportedOps,
           &pIFace->supportedOps,
           sizeof(GblIVariantIFace) - offsetof(GblIVariantIFace, supportedOps));
    GBL_API_END();
}

static GBL_RESULT nilSave_(const GblVariant* pVariant, GblString* pString) {
    GBL_API_BEGIN(NULL);
    GBL_API_VERIFY_EXPRESSION(GblVariant_type(pVariant) == GBL_TYPE_NIL);
    GBL_API_CALL(gblStringFromNil(pString));
    GBL_API_END();
}

static GBL_RESULT nilLoad_(GblVariant* pVariant, const GblString* pString) {
    GblBool isNil = GBL_FALSE;
    GBL_API_BEGIN(pString->data.hCtx);
    GBL_API_CALL(gblStringToNil(pString, &isNil));
    GBL_API_VERIFY_EXPRESSION(isNil);
    GBL_API_VERIFY_TYPE(pVariant->type, GBL_TYPE_NIL);
    GBL_API_END();
}

static GBL_RESULT boolSave_(const GblVariant* pVariant, GblString* pString) {
    GBL_API_BEGIN(NULL);
    GBL_API_VERIFY_EXPRESSION(GblVariant_type(pVariant) == GBL_TYPE_NIL);
    GBL_API_CALL(gblStringFromb(pString, pVariant->boolean));
    GBL_API_END();
}

static GBL_RESULT boolLoad_(GblVariant* pVariant, const GblString* pString) {
    GBL_API_BEGIN(pString->data.hCtx);
    GBL_API_CALL(gblStringTob(pString, &pVariant->boolean));
    GBL_API_END();
}

static GBL_RESULT boolSet_(GblVariant* pVariant, GblUint argc, const GblVariant* pArgs, GBL_IVARIANT_OP_FLAGS op) {
    GBL_API_BEGIN(NULL);
    GBL_UNUSED(argc);
    GBL_API_VERIFY_EXPRESSION(op & GBL_IVARIANT_OP_FLAG_SET_VALUE_COPY);
    pVariant->boolean = pArgs->i32? GBL_TRUE : GBL_FALSE;
    GBL_API_END();
}

static GBL_RESULT boolGet_(const GblVariant* pVariant, GblUint argc, GblVariant* pArgs, GBL_IVARIANT_OP_FLAGS op) {
    GBL_API_BEGIN(NULL);
    GBL_UNUSED(argc);
    GBL_API_VERIFY_EXPRESSION(op & GBL_IVARIANT_OP_FLAG_GET_VALUE_COPY);
    GBL_API_VERIFY_TYPE(pArgs[0].type, GBL_TYPE_POINTER);
    *((GblBool*)pArgs->pVoidPtr) = pVariant->boolean;
    GBL_API_END();
}

static GBL_RESULT charSave_(const GblVariant* pVariant, GblString* pString) {
    GBL_API_BEGIN(NULL);
    gblStringSprintf(pString, "'%c'", pVariant->character);
    GBL_API_END();
}

static GBL_RESULT charLoad_(GblVariant* pVariant, const GblString* pString) {
    GBL_API_BEGIN(NULL);
    const char* pStr = NULL;
    GblSize length = 0;
    GBL_API_CALL(gblStringLength(pString, &length));
    if(length == 3) {
        GBL_API_CALL(gblStringCStr(pString, &pStr));
        GBL_API_VERIFY_EXPRESSION(pStr[0] == '\'');
        GBL_API_VERIFY_EXPRESSION(pStr[2] == '\'');
        pVariant->character = pStr[1];
    }
    GBL_API_END();
}

static GBL_RESULT charSet_(GblVariant* pVariant, GblUint argc, const GblVariant* pArgs, GBL_IVARIANT_OP_FLAGS op) {
    GBL_API_BEGIN(NULL);
    GBL_UNUSED(argc);
    GBL_API_VERIFY_EXPRESSION(op & GBL_IVARIANT_OP_FLAG_SET_VALUE_COPY);
    GBL_API_VERIFY_TYPE(pArgs->type, GBL_TYPE_INT32);
    pVariant->character = pArgs->i32;
    GBL_API_END();
}

static GBL_RESULT charGet_(const GblVariant* pVariant, GblUint argc, GblVariant* pArgs, GBL_IVARIANT_OP_FLAGS op) {
    GBL_API_BEGIN(NULL);
    GBL_UNUSED(argc);
    GBL_API_VERIFY_EXPRESSION(op & GBL_IVARIANT_OP_FLAG_GET_VALUE_COPY);
    GBL_API_VERIFY_TYPE(pArgs[0].type, GBL_TYPE_POINTER);
    *((char*)pArgs->pVoidPtr) = pVariant->character;
    GBL_API_END();
}


static GBL_RESULT u8Save_(const GblVariant* pVariant, GblString* pString) {
    GBL_API_BEGIN(NULL);
    gblStringSprintf(pString, "%u", pVariant->u8);
    GBL_API_END();
}

static GBL_RESULT u8Load_(GblVariant* pVariant, const GblString* pString) {
    GBL_API_BEGIN(NULL);
    GblInt integer = 0;
    GBL_API_CALL(gblStringToi(pString, &integer));
    pVariant->u8 = (uint8_t)integer;
    GBL_API_END();
}

static GBL_RESULT u8Set_(GblVariant* pVariant, GblUint argc, const GblVariant* pArgs, GBL_IVARIANT_OP_FLAGS op) {
    GBL_API_BEGIN(NULL);
    GBL_UNUSED(argc);
    GBL_API_VERIFY_EXPRESSION(op & GBL_IVARIANT_OP_FLAG_SET_VALUE_COPY);
    GBL_API_VERIFY_TYPE(pArgs->type, GBL_TYPE_INT32);
    pVariant->u8 = (uint8_t)pArgs->i32;
    GBL_API_END();
}

static GBL_RESULT u8Get_(const GblVariant* pVariant, GblUint argc, GblVariant* pArgs, GBL_IVARIANT_OP_FLAGS op) {
    GBL_API_BEGIN(NULL);
    GBL_UNUSED(argc);
    GBL_API_VERIFY_EXPRESSION(op & GBL_IVARIANT_OP_FLAG_GET_VALUE_COPY);
    GBL_API_VERIFY_TYPE(pArgs[0].type, GBL_TYPE_POINTER);
    *((uint8_t*)pArgs->pVoidPtr) = pVariant->u8;
    GBL_API_END();
}


static GBL_RESULT i16Save_(const GblVariant* pVariant, GblString* pString) {
    GBL_API_BEGIN(NULL);
    gblStringSprintf(pString, "%d", pVariant->i16);
    GBL_API_END();
}

static GBL_RESULT i16Load_(GblVariant* pVariant, const GblString* pString) {
    GBL_API_BEGIN(NULL);
    GblInt integer = 0;
    GBL_API_CALL(gblStringToi(pString, &integer));
    pVariant->i16 = (int16_t)integer;
    GBL_API_END();
}

static GBL_RESULT i16Set_(GblVariant* pVariant, GblUint argc, const GblVariant* pArgs, GBL_IVARIANT_OP_FLAGS op) {
    GBL_API_BEGIN(NULL);
    GBL_UNUSED(argc);
    GBL_API_VERIFY_EXPRESSION(op & GBL_IVARIANT_OP_FLAG_SET_VALUE_COPY);
    GBL_API_VERIFY_TYPE(pArgs->type, GBL_TYPE_INT32);
    pVariant->i16 = (int16_t)pArgs->i32;
    GBL_API_END();
}

static GBL_RESULT i16Get_(const GblVariant* pVariant, GblUint argc, GblVariant* pArgs, GBL_IVARIANT_OP_FLAGS op) {
    GBL_API_BEGIN(NULL);
    GBL_UNUSED(argc);
    GBL_API_VERIFY_EXPRESSION(op & GBL_IVARIANT_OP_FLAG_GET_VALUE_COPY);
    GBL_API_VERIFY_TYPE(pArgs[0].type, GBL_TYPE_POINTER);
    *((int16_t*)pArgs->pVoidPtr) = pVariant->i16;
    GBL_API_END();
}

static GBL_RESULT u16Save_(const GblVariant* pVariant, GblString* pString) {
    GBL_API_BEGIN(NULL);
    gblStringSprintf(pString, "%u", pVariant->u16);
    GBL_API_END();
}

static GBL_RESULT u16Load_(GblVariant* pVariant, const GblString* pString) {
    GBL_API_BEGIN(NULL);
    GblInt integer = 0;
    GBL_API_CALL(gblStringToi(pString, &integer));
    pVariant->u16 = (uint16_t)integer;
    GBL_API_END();
}

static GBL_RESULT u16Set_(GblVariant* pVariant, GblUint argc, const GblVariant* pArgs, GBL_IVARIANT_OP_FLAGS op) {
    GBL_API_BEGIN(NULL);
    GBL_UNUSED(argc);
    GBL_API_VERIFY_EXPRESSION(op & GBL_IVARIANT_OP_FLAG_SET_VALUE_COPY);
    GBL_API_VERIFY_TYPE(pArgs->type, GBL_TYPE_INT32);
    pVariant->u16 = (uint16_t)pArgs->i32;
    GBL_API_END();
}

static GBL_RESULT u16Get_(const GblVariant* pVariant, GblUint argc, GblVariant* pArgs, GBL_IVARIANT_OP_FLAGS op) {
    GBL_API_BEGIN(NULL);
    GBL_UNUSED(argc);
    GBL_API_VERIFY_EXPRESSION(op & GBL_IVARIANT_OP_FLAG_GET_VALUE_COPY);
    GBL_API_VERIFY_TYPE(pArgs[0].type, GBL_TYPE_POINTER);
    *((uint16_t*)pArgs->pVoidPtr) = pVariant->u16;
    GBL_API_END();
}

static GBL_RESULT i32Save_(const GblVariant* pVariant, GblString* pString) {
    GBL_API_BEGIN(NULL);
    gblStringSprintf(pString, "%d", pVariant->i32);
    GBL_API_END();
}

static GBL_RESULT i32Load_(GblVariant* pVariant, const GblString* pString) {
    GBL_API_BEGIN(NULL);
    GblInt integer = 0;
    GBL_API_CALL(gblStringToi(pString, &integer));
    pVariant->i32 = (int32_t)integer;
    GBL_API_END();
}

static GBL_RESULT i32Set_(GblVariant* pVariant, GblUint argc, const GblVariant* pArgs, GBL_IVARIANT_OP_FLAGS op) {
    GBL_API_BEGIN(NULL);
    GBL_UNUSED(argc);
    GBL_API_VERIFY_EXPRESSION(op & GBL_IVARIANT_OP_FLAG_SET_VALUE_COPY);
    GBL_API_VERIFY_TYPE(pArgs->type, GBL_TYPE_INT32);
    pVariant->i32 = (int32_t)pArgs->i32;
    GBL_API_END();
}

static GBL_RESULT i32Get_(const GblVariant* pVariant, GblUint argc, GblVariant* pArgs, GBL_IVARIANT_OP_FLAGS op) {
    GBL_API_BEGIN(NULL);
    GBL_UNUSED(argc);
    GBL_API_VERIFY_EXPRESSION(op & GBL_IVARIANT_OP_FLAG_GET_VALUE_COPY);
    GBL_API_VERIFY_TYPE(pArgs[0].type, GBL_TYPE_POINTER);
    *((int32_t*)pArgs->pVoidPtr) = pVariant->i32;
    GBL_API_END();
}

static GBL_RESULT u32Save_(const GblVariant* pVariant, GblString* pString) {
    GBL_API_BEGIN(NULL);
    gblStringSprintf(pString, "%u", pVariant->u32);
    GBL_API_END();
}

static GBL_RESULT u32Load_(GblVariant* pVariant, const GblString* pString) {
    GBL_API_BEGIN(NULL);
    GblInt integer = 0;
    GBL_API_CALL(gblStringToi(pString, &integer));
    pVariant->u32 = (uint32_t)integer;
    GBL_API_END();
}

static GBL_RESULT u32Set_(GblVariant* pVariant, GblUint argc, const GblVariant* pArgs, GBL_IVARIANT_OP_FLAGS op) {
    GBL_API_BEGIN(NULL);
    GBL_UNUSED(argc);
    GBL_API_VERIFY_EXPRESSION(op & GBL_IVARIANT_OP_FLAG_SET_VALUE_COPY);
    GBL_API_VERIFY_TYPE(pArgs->type, GBL_TYPE_UINT32);
    pVariant->u32 = (uint32_t)pArgs->u32;
    GBL_API_END();
}

static GBL_RESULT u32Get_(const GblVariant* pVariant, GblUint argc, GblVariant* pArgs, GBL_IVARIANT_OP_FLAGS op) {
    GBL_API_BEGIN(NULL);
    GBL_UNUSED(argc);
    GBL_API_VERIFY_EXPRESSION(op & GBL_IVARIANT_OP_FLAG_GET_VALUE_COPY);
    GBL_API_VERIFY_TYPE(pArgs[0].type, GBL_TYPE_POINTER);
    *((uint32_t*)pArgs->pVoidPtr) = pVariant->u32;
    GBL_API_END();
}


static GBL_RESULT i64Save_(const GblVariant* pVariant, GblString* pString) {
    GBL_API_BEGIN(NULL);
    gblStringSprintf(pString, "%d", pVariant->i64);
    GBL_API_END();
}

static GBL_RESULT i64Load_(GblVariant* pVariant, const GblString* pString) {
    GBL_API_BEGIN(NULL);
    GblInt integer = 0;
    GBL_API_CALL(gblStringToi(pString, &integer));
    pVariant->i64 = (int64_t)integer;
    GBL_API_END();
}

static GBL_RESULT i64Set_(GblVariant* pVariant, GblUint argc, const GblVariant* pArgs, GBL_IVARIANT_OP_FLAGS op) {
    GBL_API_BEGIN(NULL);
    GBL_UNUSED(argc);
    GBL_API_VERIFY_EXPRESSION(op & GBL_IVARIANT_OP_FLAG_SET_VALUE_COPY);
    GBL_API_VERIFY_TYPE(pArgs->type, GBL_TYPE_INT64);
    pVariant->i64 = (int64_t)pArgs->i64;
    GBL_API_END();
}

static GBL_RESULT i64Get_(const GblVariant* pVariant, GblUint argc, GblVariant* pArgs, GBL_IVARIANT_OP_FLAGS op) {
    GBL_API_BEGIN(NULL);
    GBL_UNUSED(argc);
    GBL_API_VERIFY_EXPRESSION(op & GBL_IVARIANT_OP_FLAG_GET_VALUE_COPY);
    GBL_API_VERIFY_TYPE(pArgs[0].type, GBL_TYPE_POINTER);
    *((int64_t*)pArgs->pVoidPtr) = pVariant->i64;
    GBL_API_END();
}

static GBL_RESULT u64Save_(const GblVariant* pVariant, GblString* pString) {
    GBL_API_BEGIN(NULL);
    gblStringSprintf(pString, "%u", pVariant->u64);
    GBL_API_END();
}

static GBL_RESULT u64Load_(GblVariant* pVariant, const GblString* pString) {
    GBL_API_BEGIN(NULL);
    GblInt integer = 0;
    GBL_API_CALL(gblStringToi(pString, &integer));
    pVariant->u64 = (uint64_t)integer;
    GBL_API_END();
}

static GBL_RESULT u64Set_(GblVariant* pVariant, GblUint argc, const GblVariant* pArgs, GBL_IVARIANT_OP_FLAGS op) {
    GBL_API_BEGIN(NULL);
    GBL_UNUSED(argc);
    GBL_API_VERIFY_EXPRESSION(op & GBL_IVARIANT_OP_FLAG_SET_VALUE_COPY);
    GBL_API_VERIFY_TYPE(pArgs->type, GBL_TYPE_INT64);
    pVariant->u64 = (uint64_t)pArgs->i64;
    GBL_API_END();
}

static GBL_RESULT u64Get_(const GblVariant* pVariant, GblUint argc, GblVariant* pArgs, GBL_IVARIANT_OP_FLAGS op) {
    GBL_API_BEGIN(NULL);
    GBL_UNUSED(argc);
    GBL_API_VERIFY_EXPRESSION(op & GBL_IVARIANT_OP_FLAG_GET_VALUE_COPY);
    GBL_API_VERIFY_TYPE(pArgs[0].type, GBL_TYPE_POINTER);
    *((uint64_t*)pArgs->pVoidPtr) = pVariant->u64;
    GBL_API_END();
}

static GBL_RESULT f32Save_(const GblVariant* pVariant, GblString* pString) {
    GBL_API_BEGIN(NULL);
    GBL_API_CALL(gblStringFromf(pString, pVariant->f32));
    GBL_API_END();
}

static GBL_RESULT f32Load_(GblVariant* pVariant, const GblString* pString) {
    GBL_API_BEGIN(NULL);
    GBL_API_CALL(gblStringTof(pString, &pVariant->f32));
    GBL_API_END();
}

static GBL_RESULT f32Set_(GblVariant* pVariant, GblUint argc, const GblVariant* pArgs, GBL_IVARIANT_OP_FLAGS op) {
    GBL_API_BEGIN(NULL);
    GBL_UNUSED(argc);
    GBL_API_VERIFY_EXPRESSION(op & GBL_IVARIANT_OP_FLAG_SET_VALUE_COPY);
    GBL_API_VERIFY_TYPE(pArgs->type, GBL_TYPE_DOUBLE);
    pVariant->f32 = (float)pArgs->f64;
    GBL_API_END();
}

static GBL_RESULT f32Get_(const GblVariant* pVariant, GblUint argc, GblVariant* pArgs, GBL_IVARIANT_OP_FLAGS op) {
    GBL_API_BEGIN(NULL);
    GBL_UNUSED(argc);
    GBL_API_VERIFY_EXPRESSION(op & GBL_IVARIANT_OP_FLAG_GET_VALUE_COPY);
    GBL_API_VERIFY_TYPE(pArgs[0].type, GBL_TYPE_POINTER);
    *((float*)pArgs->pVoidPtr) = pVariant->f32;
    GBL_API_END();
}

static GBL_RESULT f64Save_(const GblVariant* pVariant, GblString* pString) {
    GBL_API_BEGIN(NULL);
    GBL_API_CALL(gblStringFromd(pString, pVariant->f64));
    GBL_API_END();
}

static GBL_RESULT f64Load_(GblVariant* pVariant, const GblString* pString) {
    GBL_API_BEGIN(NULL);
    GBL_API_CALL(gblStringTod(pString, &pVariant->f64));
    GBL_API_END();
}

static GBL_RESULT f64Set_(GblVariant* pVariant, GblUint argc, const GblVariant* pArgs, GBL_IVARIANT_OP_FLAGS op) {
    GBL_API_BEGIN(NULL);
    GBL_UNUSED(argc);
    GBL_API_VERIFY_EXPRESSION(op & GBL_IVARIANT_OP_FLAG_SET_VALUE_COPY);
    GBL_API_VERIFY_TYPE(pArgs->type, GBL_TYPE_DOUBLE);
    pVariant->f64 = pArgs->f64;
    GBL_API_END();
}

static GBL_RESULT f64Get_(const GblVariant* pVariant, GblUint argc, GblVariant* pArgs, GBL_IVARIANT_OP_FLAGS op) {
    GBL_API_BEGIN(NULL);
    GBL_UNUSED(argc);
    GBL_API_VERIFY_EXPRESSION(op & GBL_IVARIANT_OP_FLAG_GET_VALUE_COPY);
    GBL_API_VERIFY_TYPE(pArgs[0].type, GBL_TYPE_POINTER);
    *((double*)pArgs->pVoidPtr) = pVariant->f64;
    GBL_API_END();
}


static GBL_RESULT pSave_(const GblVariant* pVariant, GblString* pString) {
    GBL_API_BEGIN(NULL);
    GBL_API_CALL(gblStringFromp(pString, pVariant->pVoid));
    GBL_API_END();
}

static GBL_RESULT pLoad_(GblVariant* pVariant, const GblString* pString) {
    GBL_API_BEGIN(NULL);
    GBL_API_CALL(gblStringToi(pString, (GblInt*)&pVariant->pVoid));
    GBL_API_END();
}

static GBL_RESULT pSet_(GblVariant* pVariant, GblUint argc, const GblVariant* pArgs, GBL_IVARIANT_OP_FLAGS op) {
    GBL_API_BEGIN(NULL);
    GBL_UNUSED(argc);
    GBL_API_VERIFY_EXPRESSION(op & GBL_IVARIANT_OP_FLAG_SET_VALUE_COPY);
    GBL_API_VERIFY_TYPE(pArgs->type, GBL_TYPE_POINTER);
    pVariant->pVoid = pArgs->pVoid;
    GBL_API_END();
}

static GBL_RESULT pGet_(const GblVariant* pVariant, GblUint argc, GblVariant* pArgs, GBL_IVARIANT_OP_FLAGS op) {
    GBL_API_BEGIN(NULL);
    GBL_UNUSED(argc);
    GBL_API_VERIFY_EXPRESSION(op & GBL_IVARIANT_OP_FLAG_GET_VALUE_COPY);
    GBL_API_VERIFY_TYPE(pArgs[0].type, GBL_TYPE_POINTER);
    *((void**)pArgs->pVoidPtr) = pVariant->pVoid;
    GBL_API_END();
}



GBL_RESULT gblValueTypesRegister_(GblContext hCtx) {
    GBL_API_BEGIN(hCtx);
    GBL_API_PUSH_VERBOSE("[GblType] Registering Builtin Types");
    GBL_API_CALL(gblTypeRegisterBuiltinType(0,
                  GBL_TYPE_INVALID,
                  "Interface",
                  &((const GblTypeInfo) {
                      .classSize    = sizeof(GblInterface),
                      .classAlign   = GBL_ALIGNOF(GblInterface),
                  }),
                  GBL_TYPE_FUNDAMENTAL_FLAG_CLASSED |
                 GBL_TYPE_FUNDAMENTAL_FLAG_DEEP_DERIVABLE | GBL_TYPE_FLAG_ABSTRACT));
    GBL_API_CALL(gblTypeRegisterBuiltinType(1,
                  GBL_TYPE_INTERFACE,
                  "IVariant",
                  &((const GblTypeInfo) {
                      .classSize    = sizeof(GblIVariantIFace),
                      .classAlign   = GBL_ALIGNOF(GblIVariantIFace),
                  }),
                  GBL_TYPE_FUNDAMENTAL_FLAG_CLASSED | GBL_TYPE_FLAG_ABSTRACT));
    GBL_API_CALL(gblTypeRegisterBuiltinType(2,
                  GBL_TYPE_INVALID,
                  "nil",
                  &((const GblTypeInfo) {
                      .pFnClassInit = (GblTypeClassInitFn)GblPrimitiveClass_init,
                      .classSize    = sizeof(GblPrimitiveClass),
                      .classAlign   = GBL_ALIGNOF(GblPrimitiveClass),
                      .pClassData   = &((const GblIVariantIFace) {
                            .supportedOps = GBL_IVARIANT_OP_FLAG_RELOCATABLE |
                                            GBL_IVARIANT_OP_FLAG_VALUELESS_TYPE,
                            .pFnSave = nilSave_,
                            .pFnLoad = nilLoad_
                      })
                  }),
                  GBL_TYPE_FUNDAMENTAL_FLAG_CLASSED));
    GBL_API_CALL(gblTypeRegisterBuiltinType(3,
                  GBL_TYPE_INVALID,
                  "bool",
                  &((const GblTypeInfo) {
                      .pFnClassInit = (GblTypeClassInitFn)GblPrimitiveClass_init,
                      .classSize    = sizeof(GblPrimitiveClass),
                      .classAlign   = GBL_ALIGNOF(GblPrimitiveClass),
                      .pClassData   = &((const GblIVariantIFace) {
                            .supportedOps = GBL_IVARIANT_OP_FLAG_RELOCATABLE    |
                                            GBL_IVARIANT_OP_FLAG_SET_VALUE_COPY |
                                            GBL_IVARIANT_OP_FLAG_GET_VALUE_COPY,
                            .pSetValueFmt = { "i"},
                            .pGetValueFmt = { "p" },
                            .pFnSet  = boolSet_,
                            .pFnGet  = boolGet_,
                            .pFnSave = boolSave_,
                            .pFnLoad = boolLoad_
                      })
                  }),
                  GBL_TYPE_FUNDAMENTAL_FLAG_CLASSED));
    GBL_API_CALL(gblTypeRegisterBuiltinType(4,
                  GBL_TYPE_INVALID,
                  "char",
                  &((const GblTypeInfo) {
                      .pFnClassInit = (GblTypeClassInitFn)GblPrimitiveClass_init,
                      .classSize    = sizeof(GblPrimitiveClass),
                      .classAlign   = GBL_ALIGNOF(GblPrimitiveClass),
                      .pClassData   = &((const GblIVariantIFace) {
                            .supportedOps = GBL_IVARIANT_OP_FLAG_RELOCATABLE    |
                                            GBL_IVARIANT_OP_FLAG_SET_VALUE_COPY |
                                            GBL_IVARIANT_OP_FLAG_GET_VALUE_COPY,
                            .pSetValueFmt = { "i"},
                            .pGetValueFmt = { "p" },
                            .pFnSet  = charSet_,
                            .pFnGet  = charGet_,
                            .pFnSave = charSave_,
                            .pFnLoad = charLoad_
                      })
                  }),
                  GBL_TYPE_FUNDAMENTAL_FLAG_CLASSED));
    GBL_API_CALL(gblTypeRegisterBuiltinType(5,
                  GBL_TYPE_INVALID,
                  "uint8",
                  &((const GblTypeInfo) {
                      .pFnClassInit = (GblTypeClassInitFn)GblPrimitiveClass_init,
                      .classSize    = sizeof(GblPrimitiveClass),
                      .classAlign   = GBL_ALIGNOF(GblPrimitiveClass),
                      .pClassData   = &((const GblIVariantIFace) {
                            .supportedOps = GBL_IVARIANT_OP_FLAG_RELOCATABLE    |
                                            GBL_IVARIANT_OP_FLAG_SET_VALUE_COPY |
                                            GBL_IVARIANT_OP_FLAG_GET_VALUE_COPY,
                            .pSetValueFmt = { "i"},
                            .pGetValueFmt = { "p" },
                            .pFnSet  = u8Set_,
                            .pFnGet  = u8Get_,
                            .pFnSave = u8Save_,
                            .pFnLoad = u8Load_
                      })
                  }),
                  GBL_TYPE_FUNDAMENTAL_FLAG_CLASSED));
    GBL_API_CALL(gblTypeRegisterBuiltinType(6,
                  GBL_TYPE_INVALID,
                  "int16",
                  &((const GblTypeInfo) {
                      .pFnClassInit = (GblTypeClassInitFn)GblPrimitiveClass_init,
                      .classSize    = sizeof(GblPrimitiveClass),
                      .classAlign   = GBL_ALIGNOF(GblPrimitiveClass),
                      .pClassData   = &((const GblIVariantIFace) {
                            .supportedOps = GBL_IVARIANT_OP_FLAG_RELOCATABLE    |
                                            GBL_IVARIANT_OP_FLAG_SET_VALUE_COPY |
                                            GBL_IVARIANT_OP_FLAG_GET_VALUE_COPY,
                            .pSetValueFmt = { "i"},
                            .pGetValueFmt = { "p" },
                            .pFnSet  = i16Set_,
                            .pFnGet  = i16Get_,
                            .pFnSave = i16Save_,
                            .pFnLoad = i16Load_
                      })
                  }),
                  GBL_TYPE_FUNDAMENTAL_FLAG_CLASSED));
    GBL_API_CALL(gblTypeRegisterBuiltinType(7,
                  GBL_TYPE_INVALID,
                  "uint16",
                  &((const GblTypeInfo) {
                      .pFnClassInit = (GblTypeClassInitFn)GblPrimitiveClass_init,
                      .classSize    = sizeof(GblPrimitiveClass),
                      .classAlign   = GBL_ALIGNOF(GblPrimitiveClass),
                      .pClassData   = &((const GblIVariantIFace) {
                            .supportedOps = GBL_IVARIANT_OP_FLAG_RELOCATABLE    |
                                            GBL_IVARIANT_OP_FLAG_SET_VALUE_COPY |
                                            GBL_IVARIANT_OP_FLAG_GET_VALUE_COPY,
                            .pSetValueFmt = { "i"},
                            .pGetValueFmt = { "p" },
                            .pFnSet  = u16Set_,
                            .pFnGet  = u16Get_,
                            .pFnSave = u16Save_,
                            .pFnLoad = u16Load_
                      })
                  }),
                  GBL_TYPE_FUNDAMENTAL_FLAG_CLASSED));
    GBL_API_CALL(gblTypeRegisterBuiltinType(8,
                  GBL_TYPE_INVALID,
                  "int32",
                  &((const GblTypeInfo) {
                      .pFnClassInit = (GblTypeClassInitFn)GblPrimitiveClass_init,
                      .classSize    = sizeof(GblPrimitiveClass),
                      .classAlign   = GBL_ALIGNOF(GblPrimitiveClass),
                      .pClassData   = &((const GblIVariantIFace) {
                            .supportedOps = GBL_IVARIANT_OP_FLAG_RELOCATABLE    |
                                            GBL_IVARIANT_OP_FLAG_SET_VALUE_COPY |
                                            GBL_IVARIANT_OP_FLAG_GET_VALUE_COPY,
                            .pSetValueFmt = { "i"},
                            .pGetValueFmt = { "p" },
                            .pFnSet  = i32Set_,
                            .pFnGet  = i32Get_,
                            .pFnSave = i32Save_,
                            .pFnLoad = i32Load_
                      })
                  }),
                  GBL_TYPE_FUNDAMENTAL_FLAG_CLASSED));
    GBL_API_CALL(gblTypeRegisterBuiltinType(9,
                  GBL_TYPE_INVALID,
                  "uint32",
                  &((const GblTypeInfo) {
                      .pFnClassInit = (GblTypeClassInitFn)GblPrimitiveClass_init,
                      .classSize    = sizeof(GblPrimitiveClass),
                      .classAlign   = GBL_ALIGNOF(GblPrimitiveClass),
                      .pClassData   = &((const GblIVariantIFace) {
                            .supportedOps = GBL_IVARIANT_OP_FLAG_RELOCATABLE    |
                                            GBL_IVARIANT_OP_FLAG_SET_VALUE_COPY |
                                            GBL_IVARIANT_OP_FLAG_GET_VALUE_COPY,
                            .pSetValueFmt = { "l"},
                            .pGetValueFmt = { "p" },
                            .pFnSet  = u32Set_,
                            .pFnGet  = u32Get_,
                            .pFnSave = u32Save_,
                            .pFnLoad = u32Load_
                      })
                  }),
                  GBL_TYPE_FUNDAMENTAL_FLAG_CLASSED));
    GBL_API_CALL(gblTypeRegisterBuiltinType(10,
                  GBL_TYPE_INVALID,
                  "int64",
                  &((const GblTypeInfo) {
                      .pFnClassInit = (GblTypeClassInitFn)GblPrimitiveClass_init,
                      .classSize    = sizeof(GblPrimitiveClass),
                      .classAlign   = GBL_ALIGNOF(GblPrimitiveClass),
                      .pClassData   = &((const GblIVariantIFace) {
                            .supportedOps = GBL_IVARIANT_OP_FLAG_RELOCATABLE    |
                                            GBL_IVARIANT_OP_FLAG_SET_VALUE_COPY |
                                            GBL_IVARIANT_OP_FLAG_GET_VALUE_COPY,
                            .pSetValueFmt = { "q"},
                            .pGetValueFmt = { "p" },
                            .pFnSet  = i64Set_,
                            .pFnGet  = i64Get_,
                            .pFnSave = i64Save_,
                            .pFnLoad = i64Load_
                      })
                  }),
                  GBL_TYPE_FUNDAMENTAL_FLAG_CLASSED));
    GBL_API_CALL(gblTypeRegisterBuiltinType(11,
                  GBL_TYPE_INVALID,
                  "uint64",
                  &((const GblTypeInfo) {
                      .pFnClassInit = (GblTypeClassInitFn)GblPrimitiveClass_init,
                      .classSize    = sizeof(GblPrimitiveClass),
                      .classAlign   = GBL_ALIGNOF(GblPrimitiveClass),
                      .pClassData   = &((const GblIVariantIFace) {
                            .supportedOps = GBL_IVARIANT_OP_FLAG_RELOCATABLE    |
                                            GBL_IVARIANT_OP_FLAG_SET_VALUE_COPY |
                                            GBL_IVARIANT_OP_FLAG_GET_VALUE_COPY,
                            .pSetValueFmt = { "q"},
                            .pGetValueFmt = { "p" },
                            .pFnSet  = u64Set_,
                            .pFnGet  = u64Get_,
                            .pFnSave = u64Save_,
                            .pFnLoad = u64Load_
                      })
                  }),
                  GBL_TYPE_FUNDAMENTAL_FLAG_CLASSED));
    GBL_API_CALL(gblTypeRegisterBuiltinType(12,
                  GBL_TYPE_INVALID,
                  "enum",
                  &((const GblTypeInfo) {
                      .pFnClassInit = (GblTypeClassInitFn)GblPrimitiveClass_init,
                      .classSize    = sizeof(GblPrimitiveClass),
                      .classAlign   = GBL_ALIGNOF(GblPrimitiveClass),
                      .pClassData   = &((const GblIVariantIFace) {
                            .supportedOps = GBL_IVARIANT_OP_FLAG_RELOCATABLE    |
                                            GBL_IVARIANT_OP_FLAG_SET_VALUE_COPY |
                                            GBL_IVARIANT_OP_FLAG_GET_VALUE_COPY,
                            .pSetValueFmt = { "q"},
                            .pGetValueFmt = { "p" },
                            .pFnSet  = u64Set_,
                            .pFnGet  = u64Get_,
                            .pFnSave = u64Save_,
                            .pFnLoad = u64Load_
                      })
                  }),
                  GBL_TYPE_FUNDAMENTAL_FLAG_CLASSED));
    GBL_API_CALL(gblTypeRegisterBuiltinType(13,
                  GBL_TYPE_INVALID,
                  "flags",
                  &((const GblTypeInfo) {
                      .pFnClassInit = (GblTypeClassInitFn)GblPrimitiveClass_init,
                      .classSize    = sizeof(GblPrimitiveClass),
                      .classAlign   = GBL_ALIGNOF(GblPrimitiveClass),
                      .pClassData   = &((const GblIVariantIFace) {
                            .supportedOps = GBL_IVARIANT_OP_FLAG_RELOCATABLE    |
                                            GBL_IVARIANT_OP_FLAG_SET_VALUE_COPY |
                                            GBL_IVARIANT_OP_FLAG_GET_VALUE_COPY,
                            .pSetValueFmt = { "q"},
                            .pGetValueFmt = { "p" },
                            .pFnSet  = u64Set_,
                            .pFnGet  = u64Get_,
                            .pFnSave = u64Save_,
                            .pFnLoad = u64Load_
                      })
                  }),
                  GBL_TYPE_FUNDAMENTAL_FLAG_CLASSED));
    GBL_API_CALL(gblTypeRegisterBuiltinType(14,
                  GBL_TYPE_INVALID,
                  "float",
                  &((const GblTypeInfo) {
                      .pFnClassInit = (GblTypeClassInitFn)GblPrimitiveClass_init,
                      .classSize    = sizeof(GblPrimitiveClass),
                      .classAlign   = GBL_ALIGNOF(GblPrimitiveClass),
                      .pClassData   = &((const GblIVariantIFace) {
                            .supportedOps = GBL_IVARIANT_OP_FLAG_RELOCATABLE    |
                                            GBL_IVARIANT_OP_FLAG_SET_VALUE_COPY |
                                            GBL_IVARIANT_OP_FLAG_GET_VALUE_COPY,
                            .pSetValueFmt = { "d"},
                            .pGetValueFmt = { "p" },
                            .pFnSet  = f32Set_,
                            .pFnGet  = f32Get_,
                            .pFnSave = f32Save_,
                            .pFnLoad = f32Load_
                      })
                  }),
                  GBL_TYPE_FUNDAMENTAL_FLAG_CLASSED));
    GBL_API_CALL(gblTypeRegisterBuiltinType(15,
                  GBL_TYPE_INVALID,
                  "double",
                  &((const GblTypeInfo) {
                      .pFnClassInit = (GblTypeClassInitFn)GblPrimitiveClass_init,
                      .classSize    = sizeof(GblPrimitiveClass),
                      .classAlign   = GBL_ALIGNOF(GblPrimitiveClass),
                      .pClassData   = &((const GblIVariantIFace) {
                            .supportedOps = GBL_IVARIANT_OP_FLAG_RELOCATABLE    |
                                            GBL_IVARIANT_OP_FLAG_SET_VALUE_COPY |
                                            GBL_IVARIANT_OP_FLAG_GET_VALUE_COPY,
                            .pSetValueFmt = { "d"},
                            .pGetValueFmt = { "p" },
                            .pFnSet  = f64Set_,
                            .pFnGet  = f64Get_,
                            .pFnSave = f64Save_,
                            .pFnLoad = f64Load_
                      })
                  }),
                  GBL_TYPE_FUNDAMENTAL_FLAG_CLASSED));
    GBL_API_CALL(gblTypeRegisterBuiltinType(16,
                  GBL_TYPE_INVALID,
                  "pointer",
                  &((const GblTypeInfo) {
                      .pFnClassInit = (GblTypeClassInitFn)GblPrimitiveClass_init,
                      .classSize    = sizeof(GblPrimitiveClass),
                      .classAlign   = GBL_ALIGNOF(GblPrimitiveClass),
                      .pClassData   = &((const GblIVariantIFace) {
                            .supportedOps = GBL_IVARIANT_OP_FLAG_RELOCATABLE    |
                                            GBL_IVARIANT_OP_FLAG_SET_VALUE_COPY |
                                            GBL_IVARIANT_OP_FLAG_GET_VALUE_COPY,
                            .pSetValueFmt = { "p"},
                            .pGetValueFmt = { "p" },
                            .pFnSet  = pSet_,
                            .pFnGet  = pGet_,
                            .pFnSave = pSave_,
                            .pFnLoad = pLoad_
                      })
                  }),
                  GBL_TYPE_FUNDAMENTAL_FLAG_CLASSED));
    GBL_API_CALL(gblTypeRegisterBuiltinType(17,
                  GBL_TYPE_INVALID,
                  "string",
                  &((const GblTypeInfo) {
                      .pFnClassInit = (GblTypeClassInitFn)GblPrimitiveClass_init,
                      .classSize    = sizeof(GblPrimitiveClass),
                      .classAlign   = GBL_ALIGNOF(GblPrimitiveClass),
                      .pClassData   = &((const GblIVariantIFace) {
                            .supportedOps = GBL_IVARIANT_OP_FLAG_RELOCATABLE    |
                                            GBL_IVARIANT_OP_FLAG_SET_VALUE_COPY |
                                            GBL_IVARIANT_OP_FLAG_GET_VALUE_COPY,
                            .pSetValueFmt = { "p"},
                            .pGetValueFmt = { "p" },
                            .pFnSet  = pSet_,
                            .pFnGet  = pGet_,
                            .pFnSave = pSave_,
                            .pFnLoad = pLoad_
                      })
                  }),
                  GBL_TYPE_FUNDAMENTAL_FLAG_CLASSED));
    GBL_API_POP(1);
    GBL_API_END();
}
