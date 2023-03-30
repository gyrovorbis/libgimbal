#include <gimbal/utils/gimbal_scanner.h>

GBL_EXPORT GblScanner* GblScanner_create(const char* pStr,
                                         size_t      count)
{
    GblScanner* pScanner = GBL_NEW(GblScanner);
    GblScanner_setInput(pScanner, pStr, count);
    return pScanner;
}

GBL_EXPORT GblRefCount GblScanner_unref(GblScanner* pSelf) {
    return GBL_UNREF(pSelf);
}

static void GblScanner_takeNextToken_(GblScanner* pSelf, GblStringView* pView) {
    pView->length = 0;
    pView->pData = NULL;

    pSelf->status &= ~GBL_SCANNER_FLAG_READ_ERROR;

    const size_t begin = GblStringView_findFirstNotOf(GBL_PRIV_REF(pSelf).streamBuffer,
                                                      GBL_STRV(GBL_PRIV_REF(pSelf).pDelimeters),
                                                      0);

    if(begin == GBL_STRING_VIEW_NPOS) {
        pView->length = 0;
        GBL_PRIV_REF(pSelf).streamBuffer.length = 0;
        pSelf->status |= GBL_SCANNER_FLAG_READ_ERROR;
        return;
    }

    size_t end = GblStringView_findFirstOf(GBL_PRIV_REF(pSelf).streamBuffer,
                                           GBL_STRV(GBL_PRIV_REF(pSelf).pDelimeters),
                                           begin+1);

    if(end == GBL_STRING_VIEW_NPOS) {
        end = GBL_PRIV_REF(pSelf).streamBuffer.length;
    }

    const size_t size = end - begin;

    pView->pData  = GBL_PRIV_REF(pSelf).streamBuffer.pData + begin;
    pView->length = size;
//    GBL_PRIV_REF(pSelf).streamBuffer.pData  += end;
//    GBL_PRIV_REF(pSelf).streamBuffer.length -= size;

    if(!size) pSelf->status |= GBL_SCANNER_FLAG_READ_ERROR;
}

static GBL_RESULT GblScanner_peek_(GblScanner* pSelf) {
    GBL_CTX_BEGIN(NULL);
    GblScanner_takeNextToken_(pSelf, &pSelf->nextToken);

    if(GblStringView_empty(pSelf->nextToken))
        pSelf->status |= GBL_SCANNER_FLAG_EOF;
    GBL_CTX_END();
}

static GBL_RESULT GblScanner_nextToken_(GblScanner* pSelf) {
    GBL_CTX_BEGIN(NULL);

    if(!GblStringView_empty(pSelf->nextToken)) {
        pSelf->token = pSelf->nextToken;
    } else {
        GblScanner_takeNextToken_(pSelf, &pSelf->token);
    }

    // If the token is empty or the entire streambuffer, zero it out
    if(GblStringView_empty(pSelf->token))
    {
        GBL_PRIV_REF(pSelf).streamBuffer = GblStringView_fromEmpty();
    } else {
        const size_t size = (pSelf->token.pData + pSelf->token.length -
                             GBL_PRIV_REF(pSelf).streamBuffer.pData);

        if(size == GBL_PRIV_REF(pSelf).streamBuffer.length) {
            GBL_PRIV_REF(pSelf).streamBuffer = GblStringView_fromEmpty();
        } else {
            GBL_PRIV_REF(pSelf).streamBuffer.pData += size + 1;
            GBL_PRIV_REF(pSelf).streamBuffer.length -= size + 1;
        }
    }

    pSelf->nextToken = GblStringView_fromEmpty();

    if(GblStringView_empty(pSelf->token))
        pSelf->status |= GBL_SCANNER_FLAG_EOF;

    GBL_CTX_END();
}

GBL_EXPORT GblBool GblScanner_nextToken(GblScanner* pSelf) {
    GBL_CTX_BEGIN(NULL);
    GBL_INSTANCE_VCALL(GblScanner, pFnNextToken, pSelf);
    GBL_CTX_END_BLOCK();
    return !GblStringView_empty(pSelf->token);
}

GBL_EXPORT GblBool GblScanner_peekToken(GblScanner* pSelf) {
    GBL_CTX_BEGIN(NULL);
    GBL_INSTANCE_VCALL(GblScanner, pFnPeekToken, pSelf);
    GBL_CTX_END_BLOCK();
    return !GblStringView_empty(pSelf->nextToken);
}

void GblScanner_reset(GblScanner* pSelf) {
    pSelf->token = GblStringView_fromEmpty();
    pSelf->nextToken    = GblStringView_fromEmpty();

    GBL_PRIV_REF(pSelf).streamBuffer =
            GblStringRef_view(GBL_PRIV_REF(pSelf).pInputString);

    pSelf->status  = (GblStringView_empty(GBL_PRIV_REF(pSelf).streamBuffer))?
                        GBL_SCANNER_FLAG_EOF :
                        0;
}

GBL_EXPORT void GblScanner_setInput(GblScanner* pSelf, const char* pStr, size_t length) {
    GblStringRef_unref(GBL_PRIV_REF(pSelf).pInputString);
    GBL_PRIV_REF(pSelf).pInputString = GblStringRef_create(pStr, length);
    GblScanner_reset(pSelf);
}

GBL_EXPORT void GblScanner_setDelimeters(GblScanner* pSelf, const char* pDelimeters) {
    GblStringRef_unref(GBL_PRIV_REF(pSelf).pDelimeters);
    GBL_PRIV_REF(pSelf).pDelimeters = GblStringRef_create(pDelimeters);
}

static GBL_RESULT GblScanner_GblObject_setProperty_(GblObject* pObject, const GblProperty* pProp, GblVariant* pValue) {
    GBL_CTX_BEGIN(NULL);

    GblScanner* pSelf = GBL_SCANNER(pObject);

    switch(pProp->id) {
    case GblScanner_Property_Id_input: {
        GblStringRef* pStr = NULL;
        GblVariant_getValueMove(pValue, &pStr);
        GblStringRef_unref(GBL_PRIV_REF(pSelf).pInputString);
        GBL_PRIV_REF(pSelf).pInputString = pStr;
        GblScanner_reset(pSelf);
        break;
    }
    case GblScanner_Property_Id_delimeters: {
        GblStringRef* pStr = NULL;
        GblVariant_getValueMove(pValue, &pStr);
        GblStringRef_unref(GBL_PRIV_REF(pSelf).pDelimeters);
        GBL_PRIV_REF(pSelf).pDelimeters = pStr;
        break;
    }
    default: GBL_CTX_RECORD_SET(GBL_RESULT_ERROR_INVALID_PROPERTY,
                                "Attempted to set invalid property %s for GblScanner",
                                GblProperty_nameString(pProp));
    }

    GBL_CTX_END();
}

static GBL_RESULT GblScanner_GblObject_property_(const GblObject* pObject, const GblProperty* pProp, GblVariant* pValue) {
    GBL_CTX_BEGIN(NULL);

    GblScanner* pSelf = GBL_SCANNER(pObject);

    switch(pProp->id) {
    case GblScanner_Property_Id_input:
        GblVariant_setString(pValue, GBL_PRIV_REF(pSelf).pInputString); break;
    case GblScanner_Property_Id_delimeters:
        GblVariant_setString(pValue, GBL_PRIV_REF(pSelf).pDelimeters); break;
    case GblScanner_Property_Id_token:
        GblVariant_setStringView(pValue, pSelf->token); break;
    case GblScanner_Property_Id_nextToken:
        GblVariant_setStringView(pValue, pSelf->nextToken); break;
    case GblScanner_Property_Id_status:
        GblVariant_setEnum(pValue, GBL_ENUM_TYPE, pSelf->status); break;
    default: GBL_CTX_RECORD_SET(GBL_RESULT_ERROR_INVALID_PROPERTY,
                                "Attempted to get invalid property %s for GblScanner",
                                GblProperty_nameString(pProp));
    }

    GBL_CTX_END();
}

static GBL_RESULT GblScanner_GblBox_destructor_(GblBox* pBox) {
    GBL_CTX_BEGIN(NULL);

    GblScanner* pSelf = GBL_SCANNER(pBox);
    GblStringRef_unref(GBL_PRIV_REF(pSelf).pInputString);
    GblStringRef_unref(GBL_PRIV_REF(pSelf).pDelimeters);

    GBL_INSTANCE_VCALL_DEFAULT(GblObject, base.pFnDestructor, pBox);

    GBL_CTX_END();
}

static GBL_RESULT GblScanner_init_(GblInstance* pInstance, GblContext* pCtx) {
    GBL_UNUSED(pCtx);
    GBL_CTX_BEGIN(NULL);

    GblScanner* pSelf = GBL_SCANNER(pInstance);
    GblScannerClass* pClass = GBL_SCANNER_GET_CLASS(pSelf);

    GBL_PRIV_REF(pSelf).pDelimeters = GblStringRef_ref(GBL_PRIV_REF(pClass).pDefaultDelimeters);

    GBL_CTX_END();
}

static GBL_RESULT GblScannerClass_final_(GblClass* pClass, const void* pUd, GblContext* pCtx) {
    GBL_UNUSED(pUd, pCtx);
    GBL_CTX_BEGIN(NULL);

    GblScannerClass* pSelfClass = GBL_SCANNER_CLASS(pClass);
    GblStringRef_unref(GBL_PRIV_REF(pSelfClass).pDefaultDelimeters);

    GBL_CTX_END();
}

static GBL_RESULT GblScannerClass_init_(GblClass* pClass, const void* pUd, GblContext* pCtx) {
    GBL_UNUSED(pUd, pCtx);
    GBL_CTX_BEGIN(NULL);

    GblScannerClass* pSelfClass = GBL_SCANNER_CLASS(pClass);

    if(!GblType_classRefCount(GBL_CLASS_TYPEOF(pClass))) {
        GBL_PROPERTIES_REGISTER(GblScanner);

        GBL_PRIV_REF(pSelfClass).pDefaultDelimeters =
                GblStringRef_create(GBL_SCANNER_DELIMETERS_DEFAULT);
    } else {
        GBL_PRIV_REF(pSelfClass).pDefaultDelimeters =
            GblStringRef_ref(
                    GBL_PRIV_REF(
                        GBL_SCANNER_CLASS(
                            GblClass_weakRefDefault(GBL_SCANNER_TYPE)))
                    .pDefaultDelimeters);
    }

    GBL_BOX_CLASS(pClass)->pFnDestructor     = GblScanner_GblBox_destructor_;
    GBL_OBJECT_CLASS(pClass)->pFnProperty    = GblScanner_GblObject_property_;
    GBL_OBJECT_CLASS(pClass)->pFnSetProperty = GblScanner_GblObject_setProperty_;
    GBL_SCANNER_CLASS(pClass)->pFnNextToken  = GblScanner_nextToken_;
    GBL_SCANNER_CLASS(pClass)->pFnPeekToken  = GblScanner_peek_;

    GBL_CTX_END();
}

GBL_EXPORT GblType GblScanner_type(void) GBL_NOEXCEPT {
    static GblType type = GBL_INVALID_TYPE;

    static const GblTypeInfo info = {
        .pFnClassInit    = GblScannerClass_init_,
        .pFnClassFinal   = GblScannerClass_final_,
        .classSize       = sizeof(GblScannerClass),
        .pFnInstanceInit = GblScanner_init_,
        .instanceSize    = sizeof(GblScanner)
    };

    if(type == GBL_INVALID_TYPE) {
        type = GblType_registerStatic(GblQuark_internStringStatic("GblScanner"),
                                      GBL_OBJECT_TYPE,
                                      &info,
                                      GBL_TYPE_FLAG_TYPEINFO_STATIC);

    }

    return type;
}
