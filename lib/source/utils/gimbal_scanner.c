#include <gimbal/utils/gimbal_scanner.h>
#include <gimbal/strings/gimbal_string_buffer.h>
#include <gimbal/strings/gimbal_pattern.h>
#include <gimbal/containers/gimbal_array_list.h>

#define GBL_SCANNER_ERROR_BUFFER_DEFAULT_SIZE_   256

#define GBL_SCANNER_CLASS_(klass)               (GBL_CLASS_PRIVATE(GblScanner, klass))

#define GBL_SCANNER_(self)                      (GBL_PRIVATE(GblScanner, self))
#define GBL_SCANNER_CURSOR_(self)               ((GblScannerCursor*)(GblScanner_cursor(pSelf)))
#define GBL_SCANNER_CHAR_(self, idx)            (GblStringView_at(GBL_SCANNER_(self)->streamBuffer, idx))

GBL_DECLARE_STRUCT(GblScannerClass_) {
    GblStringRef* pDefaultDelimeters;
};

GBL_DECLARE_STRUCT(GblScanner_) {
    GblArrayList        cursorStack;
    GblStringView       streamBuffer;
    GblStringRef* pInputString;
    GblStringRef* pDelimeters;
};

GBL_EXPORT GblStringRef* GblScannerClass_defaultDelimeters(const GblScannerClass* pClass) {
    return GBL_SCANNER_CLASS_(pClass)->pDefaultDelimeters;
}

GBL_EXPORT void GblScannerClass_setDefaultDelimeters(GblScannerClass* pClass, const char* pStr) {
    GblScannerClass_* pClass_ = GBL_SCANNER_CLASS_(pClass);

    GblStringRef_unref(pClass_->pDefaultDelimeters);
    if(pStr) pClass_->pDefaultDelimeters = GblStringRef_create(pStr);
}

GBL_EXPORT void GblScannerClass_setDefaultDelimetersRef(GblScannerClass* pClass, GblStringRef* pRef) {
    GblScannerClass_* pClass_ = GBL_SCANNER_CLASS_(pClass);

    GblStringRef_unref(pClass_->pDefaultDelimeters);
    pClass_->pDefaultDelimeters = pRef;
}

GBL_EXPORT size_t GblScanner_cursorDepth(const GblScanner* pSelf) {
    return GblArrayList_size(&GBL_SCANNER_(pSelf)->cursorStack);
}

GBL_EXPORT const GblScannerCursor* GblScanner_cursor(const GblScanner* pSelf, size_t depth) {
    return GblArrayList_at(&GBL_SCANNER_(pSelf)->cursorStack,
                           GblScanner_cursorDepth(pSelf) - depth - 1);
}

GBL_EXPORT GBL_RESULT GblScanner_pushCursor(GblScanner* pSelf) {
    GblScanner_* pSelf_ = GBL_SCANNER_(pSelf);

    return GblArrayList_pushBack(&pSelf_->cursorStack,
                                 GblArrayList_back(&pSelf_->cursorStack));
}

GBL_EXPORT GBL_RESULT GblScanner_popCursor(GblScanner* pSelf) {
    GBL_ASSERT(GblScanner_cursorDepth(pSelf), "Cannot pop last cursor!");
    return GblArrayList_popBack(&GBL_SCANNER_(pSelf)->cursorStack, NULL);
}

GBL_EXPORT GblScanner* GblScanner_create(const char* pStr,
                                         size_t      count)
{
    GblScanner* pScanner = GBL_NEW(GblScanner);
    GblScanner_setInput(pScanner, pStr, count);
    return pScanner;
}

GBL_EXPORT GblScanner* GblScanner_ref(GblScanner* pSelf) {
    return GBL_SCANNER(GBL_REF(pSelf));
}

GBL_EXPORT GblRefCount GblScanner_unref(GblScanner* pSelf) {
    return GBL_UNREF(pSelf);
}

GBL_EXPORT GblStringRef* GblScanner_input(const GblScanner* pSelf) {
    return GBL_SCANNER_(pSelf)->pInputString;
}

GBL_EXPORT void GblScanner_setInputRef(GblScanner* pSelf, GblStringRef* pRef) {
    GblScanner_* pSelf_ = GBL_SCANNER_(pSelf);

    GblStringRef_unref(pSelf_->pInputString);
    pSelf_->pInputString = pRef;
    GblScanner_reset(pSelf);
}

GBL_EXPORT void GblScanner_setInput(GblScanner* pSelf, const char* pStr, size_t length) {
    GblScanner_setInputRef(pSelf, GblStringRef_create(pStr, length));
}

void GblScanner_reset(GblScanner* pSelf) {
    GblScanner_* pSelf_ = GBL_SCANNER_(pSelf);

    GblScanner_clearError(pSelf);

    pSelf->token = GblStringView_fromEmpty();
    pSelf->next  = GblStringView_fromEmpty();

    pSelf_->streamBuffer =
        GblStringRef_view(pSelf_->pInputString);

    GblArrayList_resize(&pSelf_->cursorStack, 1);

    memset(GBL_SCANNER_CURSOR_(pSelf), 0, sizeof(GblScannerCursor));

    pSelf->status = (GblStringView_empty(pSelf_->streamBuffer))?
                       GBL_SCANNER_EOF :
                       GBL_SCANNER_OK;
}

GBL_EXPORT GblStringRef* GblScanner_delimeters(const GblScanner* pSelf) {
    return GBL_SCANNER_(pSelf)->pDelimeters;
}

GBL_EXPORT void GblScanner_setDelimeters(GblScanner* pSelf, const char* pDelimeters) {
    GblStringRef_unref(GBL_SCANNER_(pSelf)->pDelimeters);
    GBL_SCANNER_(pSelf)->pDelimeters = GblStringRef_create(pDelimeters);
}

GBL_EXPORT void GblScanner_clearError(GblScanner* pSelf) {
    pSelf->status &= ~GBL_SCANNER_ERROR;
    GblStringRef_unref(pSelf->pError);
    pSelf->pError = NULL;
}

GBL_EXPORT void GblScanner_raiseError(GblScanner* pSelf,
                                      GblFlags    flags,
                                      const char* pFmt,
                                      ...)
{
    GBL_CTX_BEGIN(NULL);

    va_list varArgs;
    va_start(varArgs, pFmt);

    GblScanner_clearError(pSelf);

    pSelf->status |= flags;

    const size_t bufferSize = sizeof(GblStringBuffer) +
                              GBL_SCANNER_ERROR_BUFFER_DEFAULT_SIZE_;
    GblStringBuffer* pStrBuff = GBL_ALLOCA(bufferSize);
    GblStringBuffer_construct(pStrBuff, NULL, 0, bufferSize);

    GblStringBuffer_appendPrintf(pStrBuff,
                                 "[line: %lu, column: %lu]: ",
                                 (unsigned long)GBL_SCANNER_CURSOR_(pSelf)->line   + 1,
                                 (unsigned long)GBL_SCANNER_CURSOR_(pSelf)->column + 1);

    GblStringBuffer_appendVPrintf(pStrBuff,
                                  pFmt,
                                  varArgs);

    pSelf->pError = GblStringBuffer_createRef(pStrBuff);

    GBL_CTX_RECORD_SET(GBL_RESULT_ERROR_INVALID_TOKEN,
                       pSelf->pError);

    GblStringBuffer_destruct(pStrBuff);

    va_end(varArgs);

    GBL_CTX_END_BLOCK();
}

static inline GblBool GblScanner_isValidPos_(const GblScanner* pSelf, int pos) {
    return (pos >= 0 && pos < GblStringRef_length(GBL_SCANNER_(pSelf)->pInputString));
}

static GblBool GblScanner_advance_(GblScanner* pSelf, size_t bytes) {
    GblScanner_*      pSelf_     = GBL_SCANNER_(pSelf);
    GblScannerCursor* pCursor    = GBL_SCANNER_CURSOR_(pSelf);
    GblBool           outOfRange = GBL_FALSE;

    if(bytes > pSelf_->streamBuffer.length) {
        bytes = pSelf_->streamBuffer.length;
        outOfRange = GBL_TRUE;
    }

    for(size_t c = 0; c < bytes; ++c) {
        switch(pSelf_->streamBuffer.pData[c]) {
        case '\n':
            ++pCursor->line;
            pCursor->column = 0;
            break;
        default:
            ++pCursor->column;
            break;
        }
        ++pCursor->position;
    }

    pSelf_->streamBuffer =
        GblStringView_removePrefix(pSelf_->streamBuffer, bytes);

    if(GblStringView_empty(pSelf_->streamBuffer))
        pSelf->status |= GBL_SCANNER_EOF;

    pCursor->length = pSelf->token.length;

    if(outOfRange) {
        GblScanner_raiseError(pSelf,
                              GBL_SCANNER_SCAN_ERROR,
                              "Unexpected end-of-stream!");
        return GBL_FALSE;
    } else return GBL_TRUE;
}

static inline GblBool GblScanner_setPos_(GblScanner* pSelf, size_t offset) {
    GblScanner_reset(pSelf);
    return GblScanner_advance_(pSelf, offset);
}

GBL_EXPORT size_t GblScanner_tell(const GblScanner* pSelf) {
    return GBL_SCANNER_CURSOR_(pSelf)->position;
}

GBL_EXPORT GblBool GblScanner_seek(GblScanner* pSelf, int whence) {
    const int pos    = GblScanner_tell(pSelf);
    const int newPos = pos + whence;

    if(!GblScanner_isValidPos_(pSelf, newPos))
        return GBL_FALSE;
    else
        return GblScanner_setPos_(pSelf, newPos);
}

static GBL_RESULT GblScanner_nextToken_(GblScanner* pSelf, GblStringView* pToken) {
    GBL_CTX_BEGIN(NULL);

    GblScanner_* pSelf_        = GBL_SCANNER_(pSelf);
    const char*  pStreamBuffer = GBL_STRING_VIEW_CSTR(pSelf_->streamBuffer);

    GblPattern_matchNotStr(pSelf_->pDelimeters,
                           pStreamBuffer,
                           pToken);

    GBL_CTX_END();
}

static GblBool GblScanner_readToken_(GblScanner*    pSelf,
                                     GblStringView* pToken,
                                     GblFlags       errorFlag,
                                     GblBool        advanceStream)
{
    GBL_CTX_BEGIN(NULL);

    pSelf->status &= ~GBL_SCANNER_ERROR;

    if(!(pSelf->status & GBL_SCANNER_EOF))
        GBL_VCALL(GblScanner, pFnNextToken, pSelf, pToken);
    else
        *pToken = GblStringView_fromEmpty();

    GBL_CTX_END_BLOCK();

    if(!GBL_RESULT_SUCCESS(GBL_CTX_RESULT()) || GblStringView_empty(*pToken)) {
        pSelf->status |= errorFlag;

        if(advanceStream)
            GblScanner_raiseError(pSelf,
                                  errorFlag,
                                  "Failed to retrieve next token!");

        return GBL_FALSE;
    } else if(advanceStream)
        return GblScanner_advance_(pSelf, (pToken->pData - GBL_SCANNER_(pSelf)->streamBuffer.pData) + pToken->length);
    else
        return GBL_TRUE;
}

GBL_EXPORT GblBool GblScanner_peekToken(GblScanner* pSelf) {
    return GblScanner_readToken_(pSelf,
                                 &pSelf->next,
                                 GBL_SCANNER_PEEK_ERROR,
                                 GBL_FALSE);
}

GBL_EXPORT GblBool GblScanner_scanToken(GblScanner* pSelf) {
    return GblScanner_readToken_(pSelf,
                                 &pSelf->token,
                                 GBL_SCANNER_SCAN_ERROR,
                                 GBL_TRUE);
}

static GblBool GblScanner_readLines_(GblScanner*    pSelf,
                                     size_t         count,
                                     GblStringView* pView,
                                     GblFlags       errorFlag,
                                     GblBool        advanceStream)
{
    GblScanner_* pSelf_ = GBL_SCANNER_(pSelf);

    pSelf->status &= ~GBL_SCANNER_ERROR;

    for(size_t c = 0; c < pSelf_->streamBuffer.length; ++c) {
        if(GBL_SCANNER_CHAR_(pSelf, c) == '\n') {
            if(!--count) {
                *pView =
                    GblStringView_substr(pSelf_->streamBuffer, 0, c);
                if(advanceStream)
                    return GblScanner_advance_(pSelf, c);
                else
                    return GBL_TRUE;
            }
        }
    }

    *pView = GblStringView_fromEmpty();
    pSelf->status |= errorFlag;

    if(advanceStream)
        GblScanner_raiseError(pSelf,
                              errorFlag,
                              "Failed to read %zu lines!",
                              count);

    return GBL_FALSE;
}

GBL_EXPORT GblBool GblScanner_peekLines(GblScanner* pSelf, size_t count) {
    return GblScanner_readLines_(pSelf, count, &pSelf->next, GBL_SCANNER_PEEK_ERROR, GBL_FALSE);
}

GBL_EXPORT GblBool GblScanner_scanLines(GblScanner* pSelf, size_t count) {
    return GblScanner_readLines_(pSelf, count, &pSelf->token, GBL_SCANNER_SCAN_ERROR, GBL_TRUE);
}

static GblBool GblScanner_readBytes_(GblScanner*    pSelf,
                                     size_t         count,
                                     GblStringView* pView,
                                     GblFlags       errorFlag,
                                     GblBool        advanceStream)
{
    GblScanner_* pSelf_ = GBL_SCANNER_(pSelf);

    pSelf->status &= ~GBL_SCANNER_ERROR;

    if(count <= pSelf_->streamBuffer.length) {
        *pView = GblStringView_substr(pSelf_->streamBuffer, 0, count);

        if(advanceStream)
            return GblScanner_advance_(pSelf, count);
        else
            return GBL_TRUE;
    }

    *pView = GblStringView_fromEmpty();
    pSelf->status |= errorFlag;
    if(advanceStream)
        GblScanner_raiseError(pSelf,
                              errorFlag,
                              "Failed to read %zu bytes!",
                              count);
    return GBL_FALSE;
}

GBL_EXPORT GblBool GblScanner_peekBytes(GblScanner* pSelf, size_t bytes) {
    return GblScanner_readBytes_(pSelf, bytes, &pSelf->next, GBL_SCANNER_PEEK_ERROR, GBL_FALSE);
}

GBL_EXPORT GblBool GblScanner_scanBytes(GblScanner* pSelf, size_t bytes) {
    return GblScanner_readBytes_(pSelf, bytes, &pSelf->token, GBL_SCANNER_SCAN_ERROR, GBL_TRUE);
}

#define GblScanner_readPrimitive_(type, postfix, initialValue) \
    static GblBool GblScanner_read##postfix##_(GblScanner*    pSelf, \
                                               type*          pValue, \
                                               GblStringView* pView, \
                                               GblFlags       errorFlag, \
                                               GblBool        advanceStream) \
    { \
        *pValue = initialValue; \
         \
        if(!GblScanner_readToken_(pSelf, \
                                  pView, \
                                  errorFlag, \
                                  advanceStream)) \
            return GBL_FALSE; \
         \
        GblBool success; \
        *pValue = GblStringView_to##postfix(*pView, &success); \
         \
        if(success) \
            return GBL_TRUE; \
        else { \
            pSelf->status |= errorFlag; \
            if(advanceStream) \
                GblScanner_raiseError(pSelf, \
                                      errorFlag, \
                                      "Failed to read "GBL_STRINGIFY(postfix)"."); \
            return GBL_FALSE; \
        } \
    }

#define GblScanner_peekPrimitive_(type, postfix) \
    GBL_EXPORT GblBool GblScanner_peek##postfix(GblScanner* pSelf, type* pValue) { \
        return GblScanner_read##postfix##_(pSelf, pValue, &pSelf->next, GBL_SCANNER_PEEK_ERROR, GBL_FALSE); \
    }

#define GblScanner_scanPrimitive_(type, postfix) \
    GBL_EXPORT GblBool GblScanner_scan##postfix(GblScanner* pSelf, type* pValue) { \
        return GblScanner_read##postfix##_(pSelf, pValue, &pSelf->token, GBL_SCANNER_SCAN_ERROR, GBL_TRUE); \
    }

#define GblScanner_exportPrimitive_(type, postfix, initialValue) \
    GblScanner_readPrimitive_(type, postfix, initialValue) \
    GblScanner_peekPrimitive_(type, postfix) \
    GblScanner_scanPrimitive_(type, postfix)

GblScanner_exportPrimitive_(GblBool,  Bool,   GBL_FALSE)
GblScanner_exportPrimitive_(char,     Char,   '\0')
GblScanner_exportPrimitive_(uint8_t,  Uint8,  0)
GblScanner_exportPrimitive_(uint16_t, Uint16, 0)
GblScanner_exportPrimitive_(int16_t,  Int16,  0)
GblScanner_exportPrimitive_(uint32_t, Uint32, 0)
GblScanner_exportPrimitive_(int32_t,  Int32,  0)
GblScanner_exportPrimitive_(uint64_t, Uint64, 0)
GblScanner_exportPrimitive_(int64_t,  Int64,  0)
GblScanner_exportPrimitive_(float,    Float,  0.0f)
GblScanner_exportPrimitive_(double,   Double, 0.0)

static GblBool GblScanner_readType_(GblScanner*    pSelf,
                                    GblType        type,
                                    GblStringView* pView,
                                    GblFlags       errorFlag,
                                    GblBool        advanceStream,
                                    va_list*       pVarArgs)
{
    GblBool    success = GBL_FALSE;
    GblVariant sVar    = GBL_VARIANT_INIT;
    GblVariant tVar    = GBL_VARIANT_INIT;

    // Grab + validate next token
    if(!GblScanner_readToken_(pSelf,
                               pView,
                               errorFlag,
                               advanceStream))
        goto done;

    // Ensure we can even convert the token to the desired type
    if(!GblVariant_canConvert(GBL_STRING_TYPE, type)) {
        GblScanner_raiseError(pSelf,
                              errorFlag,
                              "Cannot convert token to type: [%s]",
                              GblType_name(type));

        goto done;
    }

    // Create source and destination variants for conversion
    GblVariant_constructStringView(&sVar, *pView);
    GblVariant_constructDefault(&tVar, type);

    // Convert token to desired variant type and validate result
    const GBL_RESULT conversionResult = GblVariant_convert(&sVar, &tVar);
    if(!GBL_RESULT_SUCCESS(conversionResult)) {
        GblScanner_raiseError(pSelf,
                              errorFlag,
                              "Conversion to token to type [%s] failed: [%s]",
                              GblType_name(type),
                              gblResultString(conversionResult));

        goto done;
    }

    // Extract value from result variant and validate result
    const GBL_RESULT moveResult = GblVariant_valueMoveVa(&tVar, pVarArgs);
    if(!GBL_RESULT_SUCCESS(moveResult)) {
        GblScanner_raiseError(pSelf,
                              errorFlag,
                              "Extraction of value type from variant type [%s] failed!",
                              GblType_name(type));

        goto done;
    }

    success = GBL_TRUE;

done:
    // Clean up after ourselves and return whether we succeeded
    GblVariant_destruct(&sVar);
    GblVariant_destruct(&tVar);

    return success;
}

GBL_EXPORT GblBool GblScanner_peekType(GblScanner* pSelf, GblType type, ...) {
    va_list varArgs;
    va_start(varArgs, type);

    const GblBool success =
        GblScanner_readType_(pSelf,
                             type,
                             &pSelf->next,
                             GBL_SCANNER_PEEK_ERROR,
                             GBL_FALSE,
                             &varArgs);

    va_end(varArgs);

    return success;
}

GBL_EXPORT GblBool GblScanner_peekTypeVa(GblScanner* pSelf, GblType t, va_list* pVa) {
    return GblScanner_readType_(pSelf,
                                t,
                                &pSelf->next,
                                GBL_SCANNER_PEEK_ERROR,
                                GBL_FALSE,
                                pVa);
}

GBL_EXPORT GblBool GblScanner_scanType(GblScanner* pSelf, GblType type, ...) {
    va_list varArgs;
    va_start(varArgs, type);

    const GblBool success =
        GblScanner_readType_(pSelf,
                             type,
                             &pSelf->token,
                             GBL_SCANNER_SCAN_ERROR,
                             GBL_TRUE,
                             &varArgs);

    va_end(varArgs);

    return success;
}

GBL_EXPORT GblBool GblScanner_scanTypeVa(GblScanner* pSelf, GblType t, va_list* pVa) {
    return GblScanner_readType_(pSelf,
                                t,
                                &pSelf->token,
                                GBL_SCANNER_SCAN_ERROR,
                                GBL_TRUE,
                                pVa);
}

static GblBool GblScanner_readMatch_(GblScanner*    pSelf,
                                     const char*    pPattern,
                                     GblStringView* pView,
                                     GblFlags       flags,
                                     GblBool        advanceStream)
{
    GblBool success = GBL_TRUE;

    if(!GblScanner_readToken_(pSelf, pView, flags, advanceStream)) {
        return GBL_FALSE;
    }

    const char* pStreamBuffer = GBL_STRING_VIEW_CSTR(*pView);
    if(!GblPattern_matchExactStr(pPattern,
                                 pStreamBuffer))
    {
        pSelf->status |= flags;
        if(advanceStream)
            GblScanner_raiseError(pSelf,
                                  flags,
                                  "Failed to match token [%s] to pattern: [%s]",
                                  pStreamBuffer,
                                  pPattern);
        return GBL_FALSE;

    }

    return GBL_TRUE;
}

GBL_EXPORT GblBool GblScanner_peekMatch(GblScanner* pSelf, const char* pPattern) {
    return GblScanner_readMatch_(pSelf,
                                 pPattern,
                                 &pSelf->next,
                                 GBL_SCANNER_PEEK_ERROR,
                                 GBL_FALSE);
}

GBL_EXPORT GblBool GblScanner_peekPattern(GblScanner* pSelf, const GblPattern* pPat) {
    GBL_ASSERT(GBL_FALSE);
    return GBL_FALSE;
}

GBL_EXPORT GblBool GblScanner_scanMatch(GblScanner* pSelf, const char* pPattern) {
    return GblScanner_readMatch_(pSelf,
                                 pPattern,
                                 &pSelf->token,
                                 GBL_SCANNER_SCAN_ERROR,
                                 GBL_TRUE);
}

GBL_EXPORT GblBool GblScanner_scanPattern(GblScanner* pSelf, const GblPattern* pPat) {
    GBL_ASSERT(GBL_FALSE);
    return GBL_FALSE;
}

GBL_EXPORT GblBool GblScanner_skipMatch(GblScanner* pSelf, const char* pStr) {
    GblScanner_* pSelf_ = GBL_SCANNER_(pSelf);

    const char* pStreamBuffer = GBL_STRING_VIEW_CSTR(pSelf_->streamBuffer);
    if(!GblPattern_matchStr(pStr, pStreamBuffer, &pSelf->token)) {
        GblScanner_raiseError(pSelf,
                              GBL_SCANNER_SKIP_ERROR,
                              "skipMatch() failed, could not find pattern: [%s]",
                              pStr);
        return GBL_FALSE;
    }

    return GblScanner_advance_(pSelf, pSelf->token.pData - pStreamBuffer + pSelf->token.length);
}

GBL_EXPORT GblBool GblScanner_skipPattern(GblScanner* pSelf, const GblPattern* pPattern) {
    GblScanner_* pSelf_ = GBL_SCANNER_(pSelf);

    const char* pStreamBuffer = GBL_STRING_VIEW_CSTR(pSelf_->streamBuffer);
    if(!GblPattern_match(pPattern, pStreamBuffer, &pSelf->token)) {
        struct {
            GblStringBuffer buff;
            char            stackBytes[128];
        } str;

        GblStringBuffer_construct(&str.buff, NULL, 0, sizeof(str));

        GblScanner_raiseError(pSelf,
                              GBL_SCANNER_SKIP_ERROR,
                              "skipMatch() failed, could not find pattern: [%s]",
                              GblPattern_string(pPattern, &str.buff));

        GblStringBuffer_destruct(&str.buff);

        return GBL_FALSE;
    }

    return GblScanner_advance_(pSelf, pSelf->token.pData - pStreamBuffer + pSelf->token.length);
}

GBL_EXPORT GblBool GblScanner_skipToMatch(GblScanner* pSelf, const char* pStr) {
    GblScanner_* pSelf_ = GBL_SCANNER_(pSelf);

    const char* pStreamBuffer = GBL_STRING_VIEW_CSTR(pSelf_->streamBuffer);
    if(!GblPattern_matchStr(pStr, pStreamBuffer, &pSelf->token)) {
        GblScanner_raiseError(pSelf,
                              GBL_SCANNER_SKIP_ERROR,
                              "skipToMatch() failed, could not find pattern: [%s]",
                              pStr);
        return GBL_FALSE;
    }

    return GblScanner_advance_(pSelf, pSelf->token.pData - pStreamBuffer);
}

GBL_EXPORT GblBool GblScanner_skipToPattern(GblScanner* pSelf, const GblPattern* pPattern) {
    GBL_ASSERT(GBL_FALSE);
    return GBL_FALSE;
}

GBL_EXPORT GblBool GblScanner_skipBytes(GblScanner* pSelf, size_t count) {
    GblScanner_* pSelf_ = GBL_SCANNER_(pSelf);

    if(count > pSelf_->streamBuffer.length) {
        pSelf->status |= GBL_SCANNER_SKIP_ERROR;
        return GBL_FALSE;
    } else return GblScanner_advance_(pSelf, count);
}

GBL_EXPORT GblBool GblScanner_skipTokens(GblScanner* pSelf, size_t count) {
    GblStringView token;
    for(size_t t = 0; t < count; ++t) {
        if(!GblScanner_readToken_(pSelf,
                                  &token,
                                  GBL_SCANNER_SKIP_ERROR,
                                  GBL_TRUE))
            return GBL_FALSE;
    }
    return GBL_TRUE;
}

GBL_EXPORT GblBool GblScanner_skipLines(GblScanner* pSelf, size_t count) {
    int offset = 0;
    for(size_t t = 0; t < count; ++t) {

        char value;
        GblBool found = GBL_FALSE;

        while((value = GBL_SCANNER_CHAR_(pSelf, offset++))) {
            if(value == '\n') {
                found = GBL_TRUE;
                break;
            }
        }

        if(!found) {
            pSelf->status |= GBL_SCANNER_SKIP_ERROR;
            return GBL_FALSE;
        }
    }

    return GblScanner_seek(pSelf, offset);
}

GBL_EXPORT int GblScanner_scanf(GblScanner* pSelf, const char* pFmt, ...) {
    va_list varArgs;
    va_start(varArgs, pFmt);
    const int retVal = GblScanner_vscanf(pSelf, pFmt, &varArgs);
    va_end(varArgs);
    return retVal;
}

GBL_EXPORT int GblScanner_vscanf(GblScanner* pSelf, const char* pFmt, va_list* pList) {
    if(!(pSelf->status & GBL_SCANNER_EOF)) {
        GblScanner_* pSelf_ = GBL_SCANNER_(pSelf);
        const char* pStreamBuffer = GBL_STRING_VIEW_CSTR(pSelf_->streamBuffer);
        return vsscanf(pStreamBuffer, pFmt, *pList);
    } else return EOF;
}

static GBL_RESULT GblScanner_GblObject_setProperty_(GblObject*         pObject,
                                                    const GblProperty* pProp,
                                                    GblVariant*        pValue)
{
    GBL_CTX_BEGIN(NULL);

    GblScanner*  pSelf  = GBL_SCANNER(pObject);
    GblScanner_* pSelf_ = GBL_SCANNER_(pSelf);

    switch(pProp->id) {
    case GblScanner_Property_Id_input: {
        GblStringRef* pStr = NULL;
        GblVariant_valueMove(pValue, &pStr);
        GblStringRef_unref(pSelf_->pInputString);
        pSelf_->pInputString = pStr;
        GblScanner_reset(pSelf);
        break;
    }
    case GblScanner_Property_Id_delimeters: {
        GblStringRef* pStr = NULL;
        GblVariant_valueMove(pValue, &pStr);
        GblStringRef_unref(pSelf_->pDelimeters);
        pSelf_->pDelimeters = pStr;
        break;
    }
    default:
        GBL_CTX_RECORD_SET(GBL_RESULT_ERROR_INVALID_PROPERTY,
                           "Attempted to set invalid property %s for GblScanner",
                            GblProperty_name(pProp));
    }

    GBL_CTX_END();
}

static GBL_RESULT GblScanner_GblObject_property_(const GblObject*   pObject,
                                                 const GblProperty* pProp,
                                                 GblVariant*        pValue)
{
    GBL_CTX_BEGIN(NULL);

    GblScanner*  pSelf  = GBL_SCANNER(pObject);
    GblScanner_* pSelf_ = GBL_SCANNER_(pSelf);

    switch(pProp->id) {
    case GblScanner_Property_Id_input:
        GblVariant_setString(pValue, pSelf_->pInputString); break;
    case GblScanner_Property_Id_delimeters:
        GblVariant_setString(pValue, pSelf_->pDelimeters); break;
    case GblScanner_Property_Id_token:
        GblVariant_setStringView(pValue, pSelf->token); break;
    case GblScanner_Property_Id_next:
        GblVariant_setStringView(pValue, pSelf->next); break;
    case GblScanner_Property_Id_status:
        GblVariant_setEnum(pValue, GBL_ENUM_TYPE, pSelf->status); break;
    case GblScanner_Property_Id_error:
        GblVariant_setStringRef(pValue, pSelf->pError); break;
    default:
        GBL_CTX_RECORD_SET(GBL_RESULT_ERROR_INVALID_PROPERTY,
                           "Attempted to get invalid property %s for GblScanner",
                           GblProperty_name(pProp));
    }

    GBL_CTX_END();
}

static GBL_RESULT GblScanner_GblBox_destructor_(GblBox* pBox) {
    GBL_CTX_BEGIN(NULL);

    GblScanner* pSelf   = GBL_SCANNER(pBox);
    GblScanner_* pSelf_ = GBL_SCANNER_(pSelf);

    GblScanner_clearError(pSelf);

    GblStringRef_unref(pSelf_->pInputString);
    GblStringRef_unref(pSelf_->pDelimeters);

    GBL_CTX_CALL(GblArrayList_destruct(&pSelf_->cursorStack));

    GBL_VCALL_DEFAULT(GblObject, base.pFnDestructor, pBox);

    GBL_CTX_END();
}

static GBL_RESULT GblScanner_init_(GblInstance* pInstance) {
    GBL_CTX_BEGIN(NULL);

    GblScanner*      pSelf  = GBL_SCANNER(pInstance);
    GblScanner_*     pSelf_ = GBL_SCANNER_(pSelf);
    GblScannerClass* pClass = GBL_SCANNER_GET_CLASS(pSelf);

    pSelf_->pDelimeters =
        GblStringRef_ref(GblScannerClass_defaultDelimeters(pClass));

    GBL_CTX_CALL(GblArrayList_construct(&pSelf_->cursorStack,
                                        sizeof(GblScannerCursor),
                                        1));

    GblScanner_reset(pSelf);

    GBL_CTX_END();
}

static GBL_RESULT GblScannerClass_final_(GblClass* pClass, const void* pUd) {
    GBL_UNUSED(pUd);
    GBL_CTX_BEGIN(NULL);

    GblScannerClass* pSelfClass = GBL_SCANNER_CLASS(pClass);
    GblScannerClass_setDefaultDelimeters(pSelfClass, NULL);

    GBL_CTX_END();
}

static GBL_RESULT GblScannerClass_init_(GblClass* pClass, const void* pUd) {
    GBL_UNUSED(pUd);
    GBL_CTX_BEGIN(NULL);

    GblScannerClass* pSelfClass = GBL_SCANNER_CLASS(pClass);

    if(!GblType_classRefCount(GBL_CLASS_TYPEOF(pClass))) {
        GBL_PROPERTIES_REGISTER(GblScanner);

        GblScannerClass_setDefaultDelimeters(pSelfClass, GBL_SCANNER_DELIMETERS_DEFAULT);

    } else {
        GblScannerClass_setDefaultDelimetersRef(pSelfClass,
            GblStringRef_ref(
                GblScannerClass_defaultDelimeters(
                    GBL_SCANNER_CLASS(
                        GblClass_weakRefDefault(GBL_SCANNER_TYPE)))));
    }

    GBL_BOX_CLASS(pClass)    ->pFnDestructor  = GblScanner_GblBox_destructor_;
    GBL_OBJECT_CLASS(pClass) ->pFnProperty    = GblScanner_GblObject_property_;
    GBL_OBJECT_CLASS(pClass) ->pFnSetProperty = GblScanner_GblObject_setProperty_;
    GBL_SCANNER_CLASS(pClass)->pFnNextToken   = GblScanner_nextToken_;

    GBL_CTX_END();
}

GBL_EXPORT GblType GblScanner_type(void) {
    static GblType type = GBL_INVALID_TYPE;

    static const GblTypeInfo info = {
        .pFnClassInit        = GblScannerClass_init_,
        .pFnClassFinal       = GblScannerClass_final_,
        .classSize           = sizeof(GblScannerClass),
        .classPrivateSize    = sizeof(GblScannerClass_),
        .pFnInstanceInit     = GblScanner_init_,
        .instanceSize        = sizeof(GblScanner),
        .instancePrivateSize = sizeof(GblScanner_)
    };

    if GBL_UNLIKELY(type == GBL_INVALID_TYPE) {
        type = GblType_register(GblQuark_internStatic("GblScanner"),
                                GBL_OBJECT_TYPE,
                                &info,
                                GBL_TYPE_FLAG_TYPEINFO_STATIC);
    }

    return type;
}
