#include <gimbal/meta2/gimbal_object.h>

#include <gimbal/meta2/gimbal_class.h>

//=== IMPLEMENTATION

static GBL_RESULT GblObject_ref_(void* pSelf, void** ppOutRef) {
    GBL_API_BEGIN(pSelf);
    GBL_API_VERIFY_OBJECT_TYPE(pSelf, GBL_OBJECT_TYPE);
    GBL_API_VERIFY_POINTER(ppOutRef);
    GBL_API_RECORD_SET(GBL_RESULT_ERROR_INVALID_VIRTUAL_CALL);
    GBL_API_END();
}

static GBL_RESULT GblObject_unref_(void* pSelf) {
    GBL_API_BEGIN(pSelf);
    GBL_API_VERIFY_OBJECT_TYPE(pSelf, GBL_OBJECT_TYPE);
    GBL_API_VERIFY_POINTER(ppOutRef);
    GBL_API_RECORD_SET(GBL_RESULT_ERROR_INVALID_VIRTUAL_CALL);
    GBL_API_END();
}


static GBL_RESULT GblObject_ctor_(void* pSelf, va_list* pVarArgs) {
    const GblClass* pClass = NULL;
    GblObject* pSelf_ = (GblObject*)pSelf;
    GBL_API_BEGIN(pSelf);
    GBL_API_VERIFY_POINTER(pVarArgs);
    pClass = va_arg(*pVarArgs, GblClass*);
    GBL_API_VERIFY_POINTER(pClass, "NULL class pointer!");
    *pSelf_->pClass_ = pClass;
    GBL_API_END();

}

static GBL_RESULT GblObject_dtor_(void* pSelf) {
    const GblClass* pClass = NULL;
    GblObject* pSelf_ = (GblObject*)pSelf;
    GBL_API_BEGIN(pSelf);
    pClass = pSelf_->pClass_;
    GBL_API_VERIFY_POINTER(pClass, "NULL class pointer!");
    if(pClass->pFnDtor) {
        GBL_API_CALL(pClass->pFnDtor(pSelf_));
    } else {
        GBL_API_RECORD_SET(GBL_RESULT_UNIMPLEMENTED);
    }
    GBL_API_END();

}

static GBL_RESULT GblObject_asType_(void* pSelf, GblType toType, void** ppOutRef) {
    GBL_API_METHOD_BEGIN(GBL_OBJECT_TYPE, pSelf);
    GBL_API_VERIFY_TYPE(toType);
    GBL_API_VERIFY_HAS_TYPE(GBL_API_SELF(), toType);
    GBL_API_VERIFY_POINTER(ppOutRef);
    *ppOutRef = NULL;
    switch(toType) {
    case GBL_OBJECT_TYPE:
        *ppOutRef = GBL_API_SELF();
    default:
        {
            int index = 0;
            while((GblClass* pSuper = GBL_API_SUPER(index++)) {
                //don't shit if unimplemented
                  GBL_API_CALL(pSuper->asType(GBL_API_SELF(), toType, ppOutRef));
                  if(ppOutRef) {
                       GBL_API_DONE();
                  }
            }
            GBL_API_RECORD_SET(GBL_RESULT_ERROR_TYPE_MISMATCH);
        }
    }

    GBL_API_END();
}

static const GblClass klass_[] = {
    {
        { klass_ + 1 },
        .pName = "Object",
        .superClass = GBL_INVALID_TYPE,
        .instanceSize = sizeof(GblObject),
        .instanceAlign = GBL_ALIGNOF(GblObject),
        .pFnRef = &GblObject_ref_,
        .pFnUnref = &GblObject_unref_,
        .pFnAsType = &GblObject_asType_,
        .pFnCtor    = &GblObject_ctor_,
        .pFnDtor = &GblObject_dtor_
    },
    {
        { klass_ + 1 },
        .pName = "Class",
        .superClass = (GblType)klass_,
        .instanceSize = sizeof(GblClass),
        .instanceAlign = GBL_ALIGNOF(GblClass),
        .pFnRef = &GblObject_ref_,
        .pFnUnref = &GblObject_unref_,
        .pFnAsType = &GblObject_asType_,
        .pFnCtor    = &GblObject_ctor_,
        .pFnDtor = &GblObject_dtor_
    },
};

static const GblClass* pObjectClass_ = &klass_[0];
static const GblClass* pClassClass_ = &klass_[1];


GBL_RESULT GblObject_typeGet(GblType* pType) {
   GBL_API_BEGIN();
   GBL_API_VERIFY_POINTER(pType);
   *pType = (GblType)pObjectClass_;
   GBL_API_END();
}

GBL_RESULT GblClass_typeGet(GblType* pType) {
   GBL_API_BEGIN();
   GBL_API_VERIFY_POINTER(pType);
   *pType = (GblType)pClassClass_;
   GBL_API_END();
}
