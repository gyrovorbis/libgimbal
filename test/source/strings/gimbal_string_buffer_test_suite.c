#include "strings/gimbal_string_buffer_test_suite.h"
#include <gimbal/test/gimbal_test_macros.h>
#include <gimbal/strings/gimbal_string_buffer.h>

#define GBL_STRING_BUFFER_TEST_SUITE_(inst) (GBL_PRIVATE(GblStringBufferTestSuite, inst))

typedef struct GblStringBufferTestSuite_ {
    struct {
        GblStringBuffer buffer;
        char            ext[5];
    } str;
    char*   pBuffer;
    size_t  capacity;
    GblStringBuffer heapOnlyStr;
} GblStringBufferTestSuite_;

static GBL_RESULT verifyBuffer_(const GblStringBuffer* pBuffer, const char* pString, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GBL_TEST_COMPARE(GblStringBuffer_length(pBuffer), strlen(pString));
    GBL_TEST_COMPARE(GblStringBuffer_cString(pBuffer), pString);
    GBL_TEST_VERIFY(GblStringView_equals(GblStringBuffer_view(pBuffer), GBL_STRV(pString)));
    GBL_TEST_COMPARE(GblStringBuffer_context(pBuffer), pCtx);
    if(!strlen(pString)) GBL_TEST_VERIFY(GblStringBuffer_empty(pBuffer));

    GBL_CTX_END();
}

static GBL_RESULT GblStringBufferTestSuite_construct_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblStringBufferTestSuite_* pSelf_ = GBL_STRING_BUFFER_TEST_SUITE_(pSelf);

    GBL_CTX_VERIFY_CALL(GblStringBuffer_construct(&pSelf_->str.buffer,
                                                  GBL_STRV(""),
                                                  sizeof(pSelf_->str),
                                                  pCtx));

    GBL_CTX_VERIFY_CALL(verifyBuffer_(&pSelf_->str.buffer, "", pCtx));
    GBL_CTX_END();
}

static GBL_RESULT GblStringBufferTestSuite_constructString_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblStringBufferTestSuite_* pSelf_ = GBL_STRING_BUFFER_TEST_SUITE_(pSelf);
    GBL_CTX_VERIFY_CALL(GblStringBuffer_construct(&pSelf_->heapOnlyStr,
                                                  GBL_STRV("Heap Allocation"),
                                                  sizeof(GblStringBuffer), pCtx));
    GBL_CTX_VERIFY_CALL(verifyBuffer_(&pSelf_->heapOnlyStr, "Heap Allocation", pCtx));
    GBL_CTX_END();
}

static GBL_RESULT GblStringBufferTestSuite_destruct_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblStringBufferTestSuite_* pSelf_ = GBL_STRING_BUFFER_TEST_SUITE_(pSelf);
    GBL_CTX_VERIFY_CALL(GblStringBuffer_destruct(&pSelf_->heapOnlyStr));
    GBL_CTX_END();
}

static GBL_RESULT GblStringBufferTestSuite_constructAlloca_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_UNUSED(pSelf);
    GBL_CTX_BEGIN(pCtx);
    GblStringBuffer* pBuffer = GBL_STRING_BUFFER_ALLOCA(0, pCtx, GBL_STRV("Alloca String"));
    GBL_CTX_VERIFY_CALL(verifyBuffer_(pBuffer, "Alloca String", pCtx));
    GBL_CTX_VERIFY_CALL(GblStringBuffer_destruct(pBuffer));
    GBL_CTX_END();
}


static GBL_RESULT GblStringBufferTestSuite_charInvalid_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblStringBufferTestSuite_* pSelf_ = GBL_STRING_BUFFER_TEST_SUITE_(pSelf);
    GBL_TEST_EXPECT_ERROR();
    const char value = GblStringBuffer_char(&pSelf_->str.buffer, 0);
    GBL_TEST_COMPARE(value, (char)'\0');
    GBL_TEST_COMPARE(GBL_CTX_LAST_RESULT(), GBL_RESULT_ERROR_OUT_OF_RANGE);
    GBL_CTX_CLEAR_LAST_RECORD();
    GBL_CTX_END();
}

static GBL_RESULT GblStringBufferTestSuite_set_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblStringBufferTestSuite_* pSelf_ = GBL_STRING_BUFFER_TEST_SUITE_(pSelf);
    GBL_TEST_COMPARE(GblStringBuffer_set(&pSelf_->str.buffer,
                                            GBL_STRV("Dreamcart")),
                     "Dreamcart");

    GBL_CTX_VERIFY_CALL(verifyBuffer_(&pSelf_->str.buffer,
                                      "Dreamcart",
                                      pCtx));
    GBL_CTX_END();
}

static GBL_RESULT GblStringBufferTestSuite_setChar_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblStringBufferTestSuite_* pSelf_ = GBL_STRING_BUFFER_TEST_SUITE_(pSelf);

    GBL_CTX_VERIFY_CALL(GblStringBuffer_setChar(&pSelf_->str.buffer, 7, 's'));
    GBL_CTX_VERIFY_CALL(verifyBuffer_(&pSelf_->str.buffer, "Dreamcast", pCtx));

    GBL_CTX_END();
}

static GBL_RESULT GblStringBufferTestSuite_release_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblStringBufferTestSuite_* pSelf_ = GBL_STRING_BUFFER_TEST_SUITE_(pSelf);
    GBL_CTX_VERIFY_CALL(GblStringBuffer_release(&pSelf_->str.buffer,
                                                &pSelf_->pBuffer,
                                                &pSelf_->capacity));
    GBL_CTX_VERIFY_CALL(verifyBuffer_(&pSelf_->str.buffer, "", pCtx));
    GBL_CTX_END();
}

static GBL_RESULT GblStringBufferTestSuite_acquire_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblStringBufferTestSuite_* pSelf_ = GBL_STRING_BUFFER_TEST_SUITE_(pSelf);
    GBL_CTX_VERIFY_CALL(GblStringBuffer_acquire(&pSelf_->str.buffer,
                                                pSelf_->pBuffer,
                                                pSelf_->capacity));
    GBL_CTX_VERIFY_CALL(verifyBuffer_(&pSelf_->str.buffer, "Dreamcast", pCtx));
    GBL_CTX_END();
}

static GBL_RESULT GblStringBufferTestSuite_append_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblStringBufferTestSuite_* pSelf_ = GBL_STRING_BUFFER_TEST_SUITE_(pSelf);
    GBL_CTX_VERIFY_CALL(GblStringBuffer_append(&pSelf_->str.buffer,
                                               GBL_STRV("er")));
    GBL_CTX_VERIFY_CALL(verifyBuffer_(&pSelf_->str.buffer, "Dreamcaster", pCtx));
    GBL_CTX_END();
}

static GBL_RESULT GblStringBufferTestSuite_prepend_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblStringBufferTestSuite_* pSelf_ = GBL_STRING_BUFFER_TEST_SUITE_(pSelf);
    GBL_CTX_VERIFY_CALL(GblStringBuffer_prepend(&pSelf_->str.buffer,
                                                GBL_STRV("Sayga ")));
    GBL_CTX_VERIFY_CALL(verifyBuffer_(&pSelf_->str.buffer, "Sayga Dreamcaster", pCtx));
    GBL_CTX_END();
}

static GBL_RESULT appendVaArgs_(GblStringBuffer* pBuffer, GblContext* pCtx, const char* pFmt, ...) {
    GBL_CTX_BEGIN(pCtx);
    va_list varArgs;
    va_start(varArgs, pFmt);

    GBL_CTX_VERIFY_CALL(GblStringBuffer_appendVPrintf(pBuffer, pFmt, varArgs));

    GBL_CTX_END_BLOCK();
    va_end(varArgs);
    return GBL_CTX_RESULT();
}

static GBL_RESULT GblStringBufferTestSuite_appendVPrintf_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_UNUSED(pSelf);
    GBL_CTX_BEGIN(pCtx);
    GblStringBuffer buffer;

    GBL_CTX_VERIFY_CALL(GblStringBuffer_construct(&buffer, GBL_STRV(""), sizeof(GblStringBuffer), pCtx));
    GBL_CTX_VERIFY_CALL(appendVaArgs_(&buffer, pCtx, "Hi %s %c %d", "Dude", 'b', 12));
    GBL_CTX_VERIFY_CALL(verifyBuffer_(&buffer, "Hi Dude b 12", pCtx));
    GBL_CTX_VERIFY_CALL(GblStringBuffer_destruct(&buffer));

    GBL_CTX_END();
}

static GBL_RESULT GblStringBufferTestSuite_appendPrintf_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_UNUSED(pSelf);
    GBL_CTX_BEGIN(pCtx);
    GblStringBuffer buffer;
    GBL_CTX_VERIFY_CALL(GblStringBuffer_construct(&buffer, GBL_STRV(""), sizeof(GblStringBuffer), pCtx));

    GBL_CTX_VERIFY_CALL(GblStringBuffer_appendPrintf(&buffer,
                                                     "Hello %s %.0f %d",
                                                     "World",
                                                     0.0f,
                                                     30));
    GBL_CTX_VERIFY_CALL(verifyBuffer_(&buffer,
                                      "Hello World 0 30",
                                      pCtx));

    GBL_CTX_VERIFY_CALL(GblStringBuffer_destruct(&buffer));
    GBL_CTX_END();
}

static GBL_RESULT GblStringBufferTestSuite_appendNil_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_UNUSED(pSelf);
    GBL_CTX_BEGIN(pCtx);
    GblStringBuffer buffer;
    GBL_CTX_VERIFY_CALL(GblStringBuffer_construct(&buffer, GBL_STRV(""), sizeof(GblStringBuffer), pCtx));

    GBL_CTX_VERIFY_CALL(GblStringBuffer_appendNil(&buffer));
    GBL_CTX_VERIFY_CALL(verifyBuffer_(&buffer,
                                      "nil",
                                      pCtx));

    GBL_CTX_VERIFY_CALL(GblStringBuffer_destruct(&buffer));
    GBL_CTX_END();
}

static GBL_RESULT GblStringBufferTestSuite_appendBool_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_UNUSED(pSelf);
    GBL_CTX_BEGIN(pCtx);
    GblStringBuffer buffer;
    GBL_CTX_VERIFY_CALL(GblStringBuffer_construct(&buffer, GBL_STRV(""), sizeof(GblStringBuffer), pCtx));

    GBL_CTX_VERIFY_CALL(GblStringBuffer_appendBool(&buffer, GBL_TRUE));
    GBL_CTX_VERIFY_CALL(GblStringBuffer_appendBool(&buffer, GBL_FALSE));
    GBL_CTX_VERIFY_CALL(verifyBuffer_(&buffer,
                                      "truefalse",
                                      pCtx));

    GBL_CTX_VERIFY_CALL(GblStringBuffer_destruct(&buffer));
    GBL_CTX_END();
}

static GBL_RESULT GblStringBufferTestSuite_appendInt_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_UNUSED(pSelf);
    GBL_CTX_BEGIN(pCtx);
    GblStringBuffer buffer;
    GBL_CTX_VERIFY_CALL(GblStringBuffer_construct(&buffer, GBL_STRV(""), sizeof(GblStringBuffer), pCtx));

    GBL_CTX_VERIFY_CALL(GblStringBuffer_appendInt(&buffer, -32));
    GBL_CTX_VERIFY_CALL(verifyBuffer_(&buffer,
                                      "-32",
                                      pCtx));

    GBL_CTX_VERIFY_CALL(GblStringBuffer_destruct(&buffer));

    GBL_CTX_END();
}

static GBL_RESULT GblStringBufferTestSuite_appendUint_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_UNUSED(pSelf);
    GBL_CTX_BEGIN(pCtx);
    GblStringBuffer buffer;
    GBL_CTX_VERIFY_CALL(GblStringBuffer_construct(&buffer, GBL_STRV(""), sizeof(GblStringBuffer), pCtx));

    GBL_CTX_VERIFY_CALL(GblStringBuffer_appendInt(&buffer, 27));
    GBL_CTX_VERIFY_CALL(verifyBuffer_(&buffer,
                                      "27",
                                      pCtx));

    GBL_CTX_VERIFY_CALL(GblStringBuffer_destruct(&buffer));

    GBL_CTX_END();
}

static GBL_RESULT GblStringBufferTestSuite_appendFloat_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_UNUSED(pSelf);
    GBL_CTX_BEGIN(pCtx);
    GblStringBuffer buffer;
    GBL_CTX_VERIFY_CALL(GblStringBuffer_construct(&buffer, GBL_STRV(""), sizeof(GblStringBuffer), pCtx));

    GBL_CTX_VERIFY_CALL(GblStringBuffer_appendFloat(&buffer, 27.3f));
    GBL_CTX_VERIFY_CALL(verifyBuffer_(&buffer,
                                      "27.300",
                                      pCtx));

    GBL_CTX_VERIFY_CALL(GblStringBuffer_destruct(&buffer));

    GBL_CTX_END();
}

static GBL_RESULT GblStringBufferTestSuite_appendDouble_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_UNUSED(pSelf);
    GBL_CTX_BEGIN(pCtx);
    GblStringBuffer buffer;
    GBL_CTX_VERIFY_CALL(GblStringBuffer_construct(&buffer, GBL_STRV(""), sizeof(GblStringBuffer), pCtx));

    GBL_CTX_VERIFY_CALL(GblStringBuffer_appendDouble(&buffer, 4.7));
    GBL_CTX_VERIFY_CALL(verifyBuffer_(&buffer,
                                      "4.700",
                                      pCtx));

    GBL_CTX_VERIFY_CALL(GblStringBuffer_destruct(&buffer));

    GBL_CTX_END();
}

static GBL_RESULT GblStringBufferTestSuite_appendPointer_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_UNUSED(pSelf);
    GBL_CTX_BEGIN(pCtx);
    GblStringBuffer buffer;
    GBL_CTX_VERIFY_CALL(GblStringBuffer_construct(&buffer, GBL_STRV(""), sizeof(GblStringBuffer), pCtx));

    GBL_CTX_VERIFY_CALL(GblStringBuffer_appendPointer(&buffer, (void*)0xdeadbabe));

    GBL_TEST_COMPARE(GblStringView_toPointer(GblStringBuffer_view(&buffer)),
                     (void*)0xdeadbabe);

    GBL_CTX_VERIFY_CALL(GblStringBuffer_destruct(&buffer));

    GBL_CTX_END();
}

static GBL_RESULT GblStringBufferTestSuite_eraseFront_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblStringBufferTestSuite_* pSelf_ = GBL_STRING_BUFFER_TEST_SUITE_(pSelf);
    GBL_CTX_VERIFY_CALL(GblStringBuffer_erase(&pSelf_->str.buffer, 0, 6));
    GBL_CTX_VERIFY_CALL(verifyBuffer_(&pSelf_->str.buffer,
                                      "Dreamcaster",
                                      pCtx));
    GBL_CTX_END();
}

static GBL_RESULT GblStringBufferTestSuite_eraseBack_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblStringBufferTestSuite_* pSelf_ = GBL_STRING_BUFFER_TEST_SUITE_(pSelf);
    GBL_CTX_VERIFY_CALL(GblStringBuffer_erase(&pSelf_->str.buffer, 9, 2));
    GBL_CTX_VERIFY_CALL(verifyBuffer_(&pSelf_->str.buffer,
                                      "Dreamcast",
                                      pCtx));
    GBL_CTX_END();
}

static GBL_RESULT GblStringBufferTestSuite_eraseMiddle_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblStringBufferTestSuite_* pSelf_ = GBL_STRING_BUFFER_TEST_SUITE_(pSelf);
    GBL_CTX_VERIFY_CALL(GblStringBuffer_erase(&pSelf_->str.buffer, 3, 3));
    GBL_CTX_VERIFY_CALL(verifyBuffer_(&pSelf_->str.buffer,
                                      "Dreast",
                                      pCtx));
    GBL_CTX_END();
}

static GBL_RESULT GblStringBufferTestSuite_clear_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblStringBufferTestSuite_* pSelf_ = GBL_STRING_BUFFER_TEST_SUITE_(pSelf);
    GBL_CTX_VERIFY_CALL(GblStringBuffer_clear(&pSelf_->str.buffer));
    GBL_CTX_VERIFY_CALL(verifyBuffer_(&pSelf_->str.buffer,
                                      "",
                                      pCtx));
    GBL_CTX_END();
}

static GBL_RESULT GblStringBufferTestSuite_overwrite_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblStringBufferTestSuite_* pSelf_ = GBL_STRING_BUFFER_TEST_SUITE_(pSelf);
    GBL_CTX_VERIFY_CALL(GblStringBuffer_overwrite(&pSelf_->str.buffer,
                                                  0,
                                                  GBL_STRV("Playstation")));
    GBL_CTX_VERIFY_CALL(verifyBuffer_(&pSelf_->str.buffer,
                                      "Playstation",
                                      pCtx));
    GBL_CTX_VERIFY_CALL(GblStringBuffer_overwrite(&pSelf_->str.buffer,
                                                  0,
                                                  GBL_STRV(" G")));
    GBL_CTX_VERIFY_CALL(verifyBuffer_(&pSelf_->str.buffer,
                                      " Gaystation",
                                      pCtx));
    GBL_CTX_END();
}

static GBL_RESULT GblStringBufferTestSuite_replace_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblStringBufferTestSuite_* pSelf_ = GBL_STRING_BUFFER_TEST_SUITE_(pSelf);
    GBL_TEST_COMPARE(GblStringBuffer_replace(&pSelf_->str.buffer,
                                                GBL_STRV(" Gay"),
                                                GBL_STRV("Gas "),
                                                1), 1);
    GBL_CTX_VERIFY_CALL(verifyBuffer_(&pSelf_->str.buffer,
                                      "Gas station",
                                      pCtx));
    GBL_CTX_END();
}

static GBL_RESULT GblStringBufferTestSuite_remove_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblStringBufferTestSuite_* pSelf_ = GBL_STRING_BUFFER_TEST_SUITE_(pSelf);
    GBL_CTX_VERIFY_CALL(GblStringBuffer_remove(&pSelf_->str.buffer,
                                               GBL_STRV("station")));
    GBL_CTX_VERIFY_CALL(verifyBuffer_(&pSelf_->str.buffer,
                                      "Gas ",
                                      pCtx));
    GBL_CTX_END();
}

static GBL_RESULT GblStringBufferTestSuite_chop_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblStringBufferTestSuite_* pSelf_ = GBL_STRING_BUFFER_TEST_SUITE_(pSelf);
    GBL_CTX_VERIFY_CALL(GblStringBuffer_chop(&pSelf_->str.buffer));
    GBL_CTX_VERIFY_CALL(verifyBuffer_(&pSelf_->str.buffer,
                                      "Gas",
                                      pCtx));
    GBL_CTX_END();
}

static GBL_RESULT GblStringBufferTestSuite_chomp_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblStringBufferTestSuite_* pSelf_ = GBL_STRING_BUFFER_TEST_SUITE_(pSelf);
    GBL_CTX_VERIFY_CALL(GblStringBuffer_append(&pSelf_->str.buffer, GBL_STRV("\n")));
    GBL_CTX_VERIFY_CALL(GblStringBuffer_chomp(&pSelf_->str.buffer));
    GBL_CTX_VERIFY_CALL(GblStringBuffer_chomp(&pSelf_->str.buffer));
    GBL_CTX_VERIFY_CALL(verifyBuffer_(&pSelf_->str.buffer,
                                      "Gas",
                                      pCtx));
    GBL_CTX_END();
}

static GBL_RESULT GblStringBufferTestSuite_lower_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblStringBufferTestSuite_* pSelf_ = GBL_STRING_BUFFER_TEST_SUITE_(pSelf);
    GBL_CTX_VERIFY_CALL(GblStringBuffer_lower(&pSelf_->str.buffer));
    GBL_CTX_VERIFY_CALL(verifyBuffer_(&pSelf_->str.buffer,
                                      "gas",
                                      pCtx));
    GBL_CTX_END();
}

static GBL_RESULT GblStringBufferTestSuite_upper_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblStringBufferTestSuite_* pSelf_ = GBL_STRING_BUFFER_TEST_SUITE_(pSelf);
    GBL_CTX_VERIFY_CALL(GblStringBuffer_upper(&pSelf_->str.buffer));
    GBL_CTX_VERIFY_CALL(verifyBuffer_(&pSelf_->str.buffer,
                                      "GAS",
                                      pCtx));
    GBL_CTX_END();
}

static GBL_RESULT GblStringBufferTestSuite_padLeft_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblStringBufferTestSuite_* pSelf_ = GBL_STRING_BUFFER_TEST_SUITE_(pSelf);
    GBL_CTX_VERIFY_CALL(GblStringBuffer_padLeft(&pSelf_->str.buffer,
                                                ' ',
                                                4));
    GBL_CTX_VERIFY_CALL(verifyBuffer_(&pSelf_->str.buffer,
                                      "    GAS",
                                      pCtx));
    GBL_CTX_END();
}

static GBL_RESULT GblStringBufferTestSuite_padRight_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblStringBufferTestSuite_* pSelf_ = GBL_STRING_BUFFER_TEST_SUITE_(pSelf);
    GBL_CTX_VERIFY_CALL(GblStringBuffer_padRight(&pSelf_->str.buffer,
                                                 'Y',
                                                 3));
    GBL_CTX_VERIFY_CALL(verifyBuffer_(&pSelf_->str.buffer,
                                      "    GASYYY",
                                      pCtx));
    GBL_CTX_END();
}

static GBL_RESULT GblStringBufferTestSuite_trimStart_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblStringBufferTestSuite_* pSelf_ = GBL_STRING_BUFFER_TEST_SUITE_(pSelf);
    GBL_CTX_VERIFY_CALL(GblStringBuffer_trimStart(&pSelf_->str.buffer,
                                                 ' '));
    GBL_CTX_VERIFY_CALL(verifyBuffer_(&pSelf_->str.buffer,
                                      "GASYYY",
                                      pCtx));
    GBL_CTX_END();
}

static GBL_RESULT GblStringBufferTestSuite_trimEnd_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblStringBufferTestSuite_* pSelf_ = GBL_STRING_BUFFER_TEST_SUITE_(pSelf);
    GBL_CTX_VERIFY_CALL(GblStringBuffer_trimEnd(&pSelf_->str.buffer,
                                                 'Y'));
    GBL_CTX_VERIFY_CALL(verifyBuffer_(&pSelf_->str.buffer,
                                      "GAS",
                                      pCtx));
    GBL_CTX_END();
}

static GBL_RESULT GblStringBufferTestSuite_reserve_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblStringBufferTestSuite_* pSelf_ = GBL_STRING_BUFFER_TEST_SUITE_(pSelf);
    GBL_CTX_VERIFY_CALL(GblStringBuffer_reserve(&pSelf_->str.buffer, 64));
    GBL_TEST_VERIFY(GblStringBuffer_capacity(&pSelf_->str.buffer) >= 64);
    GBL_CTX_VERIFY_CALL(verifyBuffer_(&pSelf_->str.buffer,
                                      "GAS",
                                      pCtx));
    GBL_CTX_END();
}

static GBL_RESULT GblStringBufferTestSuite_resize_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblStringBufferTestSuite_* pSelf_ = GBL_STRING_BUFFER_TEST_SUITE_(pSelf);
    GBL_CTX_VERIFY_CALL(GblStringBuffer_resize(&pSelf_->str.buffer, 8));
    GBL_TEST_COMPARE(GblStringBuffer_length(&pSelf_->str.buffer), 8);
    GBL_TEST_COMPARE(GblStringBuffer_cString(&pSelf_->str.buffer), "GAS");

    GBL_CTX_END();
}

static GBL_RESULT GblStringBufferTestSuite_grow_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblStringBufferTestSuite_* pSelf_ = GBL_STRING_BUFFER_TEST_SUITE_(pSelf);
    GBL_CTX_VERIFY_CALL(GblStringBuffer_grow(&pSelf_->str.buffer, 3));
    GBL_TEST_COMPARE(GblStringBuffer_length(&pSelf_->str.buffer), 11);
    GBL_TEST_COMPARE(GblStringBuffer_cString(&pSelf_->str.buffer), "GAS");
    GBL_CTX_END();
}

static GBL_RESULT GblStringBufferTestSuite_shrink_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblStringBufferTestSuite_* pSelf_ = GBL_STRING_BUFFER_TEST_SUITE_(pSelf);
    GBL_CTX_VERIFY_CALL(GblStringBuffer_shrink(&pSelf_->str.buffer, 9));
    GBL_CTX_VERIFY_CALL(verifyBuffer_(&pSelf_->str.buffer,
                                      "GA",
                                      pCtx));
    GBL_CTX_END();
}

static GBL_RESULT GblStringBufferTestSuite_shrinkToFit_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblStringBufferTestSuite_* pSelf_ = GBL_STRING_BUFFER_TEST_SUITE_(pSelf);
    const size_t  capacity = GblStringBuffer_capacity(&pSelf_->str.buffer);
    GBL_CTX_VERIFY_CALL(GblStringBuffer_shrinkToFit(&pSelf_->str.buffer));
    GBL_CTX_VERIFY_CALL(verifyBuffer_(&pSelf_->str.buffer,
                                      "GA",
                                      pCtx));
    GBL_TEST_VERIFY(GblStringBuffer_capacity(&pSelf_->str.buffer) < capacity);
    GBL_CTX_END();
}

GBL_EXPORT GblType GblStringBufferTestSuite_type(void) {
    static GblType type = GBL_INVALID_TYPE;

    const static GblTestCase cases[] = {
        { "construct",          GblStringBufferTestSuite_construct_         },
        { "constructString",    GblStringBufferTestSuite_constructString_   },
        { "destruct",           GblStringBufferTestSuite_destruct_          },
        { "constructAlloca",    GblStringBufferTestSuite_constructAlloca_   },
        { "charInvalid",        GblStringBufferTestSuite_charInvalid_       },
        { "set",                GblStringBufferTestSuite_set_               },
        { "setChar",            GblStringBufferTestSuite_setChar_           },
        { "release",            GblStringBufferTestSuite_release_           },
        { "acquire",            GblStringBufferTestSuite_acquire_           },
        { "append",             GblStringBufferTestSuite_append_            },
        { "prepend",            GblStringBufferTestSuite_prepend_           },
        { "appendVPrintf",      GblStringBufferTestSuite_appendVPrintf_     },
        { "appendPrintf",       GblStringBufferTestSuite_appendPrintf_      },
        { "appendNil",          GblStringBufferTestSuite_appendNil_         },
        { "appendBool",         GblStringBufferTestSuite_appendBool_        },
        { "appendInt",          GblStringBufferTestSuite_appendInt_         },
        { "appendUint",         GblStringBufferTestSuite_appendUint_        },
        { "appendFloat",        GblStringBufferTestSuite_appendFloat_       },
        { "appendDouble",       GblStringBufferTestSuite_appendDouble_      },
        { "appendPointer",      GblStringBufferTestSuite_appendPointer_     },
        { "eraseFront",         GblStringBufferTestSuite_eraseFront_        },
        { "eraseBack",          GblStringBufferTestSuite_eraseBack_         },
        { "eraseMiddle",        GblStringBufferTestSuite_eraseMiddle_       },
        { "clear",              GblStringBufferTestSuite_clear_             },
        { "overwrite",          GblStringBufferTestSuite_overwrite_         },
        { "replace",            GblStringBufferTestSuite_replace_           },
        { "remove",             GblStringBufferTestSuite_remove_            },
        { "chop",               GblStringBufferTestSuite_chop_              },
        { "chomp",              GblStringBufferTestSuite_chomp_             },
        { "lower",              GblStringBufferTestSuite_lower_             },
        { "upper",              GblStringBufferTestSuite_upper_             },
        { "padLeft",            GblStringBufferTestSuite_padLeft_           },
        { "padRight",           GblStringBufferTestSuite_padRight_          },
        { "trimStart",          GblStringBufferTestSuite_trimStart_         },
        { "trimEnd",            GblStringBufferTestSuite_trimEnd_           },
        { "reserve",            GblStringBufferTestSuite_reserve_           },
        { "resize",             GblStringBufferTestSuite_resize_            },
        { "grow",               GblStringBufferTestSuite_grow_              },
        { "shrink",             GblStringBufferTestSuite_shrink_            },
        { "shrinkToFit",        GblStringBufferTestSuite_shrinkToFit_       },
        { NULL,                 NULL                                        }
    };

    const static GblTestSuiteVTable vTable = {
        .pCases         = cases
    };

    if(type == GBL_INVALID_TYPE) {
        GBL_CTX_BEGIN(NULL);
        type = GblTestSuite_register(GblQuark_internStringStatic("StringBufferTestSuite"),
                                     &vTable,
                                     sizeof(GblStringBufferTestSuite),
                                     sizeof(GblStringBufferTestSuite_),
                                     GBL_TYPE_FLAGS_NONE);
        GBL_CTX_VERIFY_LAST_RECORD();
        GBL_CTX_END_BLOCK();
    }
    return type;
}

