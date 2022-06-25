#include "strings/test_gimbal_quark.h"
#include <gimbal/core/gimbal_api_frame.h>
#include <gimbal/strings/gimbal_quark.h>

GblQuark staticString_  = GBL_QUARK_INVALID;
GblQuark string_        = GBL_QUARK_INVALID;
GblQuark sizedString_   = GBL_QUARK_INVALID;
GblQuark pagedString_   = GBL_QUARK_INVALID;

GBL_API GblQuark_test_testCaseInit(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GBL_API_END();
}

GBL_API GblQuark_test_testCaseFinal(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GBL_API_END();
}

GBL_API GblQuark_test_init(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GBL_API_VERIFY_CALL(GblQuark_init(pCtx, 16, 2));

    GBL_COMPARE(GblQuark_pageCount(), 1);
    GBL_COMPARE(GblQuark_bytesUsed(), 0);
    GBL_COMPARE(GblQuark_count(), 0);
    GBL_COMPARE(GblQuark_context(), pCtx);

    GBL_API_END();
}

GBL_API GblQuark_test_fromStringStatic(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);

    staticString_ = GblQuark_fromStringStatic("StaticString");
    GBL_VERIFY(staticString_ != GBL_QUARK_INVALID);
    GBL_COMPARE(GblQuark_count(), 1);
    GBL_COMPARE(GblQuark_bytesUsed(), 0);

    GBL_API_END();
}

GBL_API GblQuark_test_fromString(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);

    string_ = GblQuark_fromString("String");
    GBL_VERIFY(string_ != GBL_QUARK_INVALID);
    GBL_COMPARE(GblQuark_count(), 2);
    GBL_COMPARE(GblQuark_bytesUsed(), 7);

    GBL_API_END();
}

GBL_API GblQuark_test_fromStringSized(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);

    sizedString_ = GblQuark_fromStringSized("SizedStringxx", 11);
    GBL_VERIFY(string_ != GBL_QUARK_INVALID);
    GBL_COMPARE(GblQuark_count(), 3);
    GBL_COMPARE(GblQuark_bytesUsed(), 19);

    GBL_API_END();
}

GBL_API GblQuark_test_tryString(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);

    GblQuark quark = GblQuark_tryString(NULL);
    GBL_COMPARE(quark, GBL_QUARK_INVALID);

    quark = GblQuark_tryString("FakeNewsString");
    GBL_COMPARE(quark, GBL_QUARK_INVALID);

    quark = GblQuark_tryString("StaticString");
    GBL_COMPARE(quark, staticString_);

    quark = GblQuark_tryString("String");
    GBL_COMPARE(quark, string_);

    quark = GblQuark_tryString("SizedString");
    GBL_COMPARE(quark, sizedString_);

    GBL_API_END();
}

GBL_API GblQuark_test_tryStringSized(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);

    GblQuark quark = GblQuark_tryStringSized(NULL, 0);
    GBL_COMPARE(quark, GBL_QUARK_INVALID);

    quark = GblQuark_tryStringSized("", 0);
    GBL_COMPARE(quark, GBL_QUARK_INVALID);

    quark = GblQuark_tryStringSized(NULL, 128);
    GBL_COMPARE(quark, GBL_QUARK_INVALID);

    quark = GblQuark_tryStringSized("FakeNewsString", 1);
    GBL_COMPARE(quark, GBL_QUARK_INVALID);

    quark = GblQuark_tryStringSized("StaticString", 12);
    GBL_COMPARE(quark, staticString_);

    quark = GblQuark_tryStringSized("Stringr", 6);
    GBL_COMPARE(quark, string_);

    quark = GblQuark_tryStringSized("SizedStringxxyz", 11);
    GBL_COMPARE(quark, sizedString_);

    quark = GblQuark_tryStringSized("SizedString", 2);
    GBL_COMPARE(quark, GBL_QUARK_INVALID);

    GBL_API_END();
}

GBL_API GblQuark_test_extraPage(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    const GblSize pageCount = GblQuark_pageCount();

    const GblSize oldBytesUsed = GblQuark_bytesUsed();
    const GblSize pageFreeBytes = GblQuark_pageSize() - oldBytesUsed - 1;
    char* pBuffer = GBL_ALLOCA(pageFreeBytes);
    for(GblSize i = 0; i <= pageFreeBytes; ++i) pBuffer[i] = 'a';
    const GblQuark wasterQuark = GblQuark_fromStringSized(pBuffer, pageFreeBytes);

    GBL_VERIFY(wasterQuark != GBL_QUARK_INVALID);

    pagedString_ = GblQuark_fromString("PagedString");
    GBL_COMPARE(GblQuark_pageCount(), pageCount+1);
    GBL_VERIFY(pagedString_ != GBL_QUARK_INVALID);
    GBL_COMPARE(GblQuark_tryString("PagedString"), pagedString_);
    GBL_COMPARE(GblQuark_bytesUsed(), oldBytesUsed + pageFreeBytes + 1 + 12);
    GBL_COMPARE(GblQuark_count(), 5);

    GBL_API_END();
}

GBL_API GblQuark_test_toString(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GBL_COMPARE(GblQuark_toString(GBL_QUARK_INVALID), NULL);
    GBL_VERIFY(strcmp(GblQuark_toString(staticString_), "StaticString") == 0);
    GBL_VERIFY(strcmp(GblQuark_toString(string_),       "String")       == 0);
    GBL_VERIFY(strcmp(GblQuark_toString(sizedString_),  "SizedString")  == 0);
    GBL_VERIFY(strcmp(GblQuark_toString(pagedString_),  "PagedString")  == 0);
    GBL_API_END();
}

GBL_API GblQuark_test_internString(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);

    GBL_COMPARE(GblQuark_internString(NULL), NULL);
    GBL_VERIFY(strcmp(GblQuark_internString("InternString"),
                      "InternString") == 0);
    const GblQuark tryQuark = GblQuark_tryString("InternString");
    GBL_VERIFY(strcmp(GblQuark_toString(tryQuark), "InternString") == 0);
    GBL_COMPARE(GblQuark_count(), 6);
    GBL_API_END();
}

GBL_API GblQuark_test_internStringSized(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GBL_COMPARE(GblQuark_internStringSized(NULL, 0), NULL);
    GBL_COMPARE(GblQuark_internStringSized("", 0), NULL);
    GBL_COMPARE(GblQuark_internStringSized(NULL, 1), NULL);

    GBL_VERIFY(strcmp(GblQuark_internStringSized("InternStringSizedabc", 15),
                      "InternStringSiz") == 0);
    const GblQuark tryQuark = GblQuark_tryString("InternStringSiz");
    GBL_VERIFY(strcmp(GblQuark_toString(tryQuark), "InternStringSiz") == 0);
    GBL_COMPARE(GblQuark_count(), 7);
    GBL_API_END();
}

GBL_API GblQuark_test_internStringStatic(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GBL_COMPARE(GblQuark_internStringStatic(NULL), NULL);
    GBL_VERIFY(strcmp(GblQuark_internStringStatic("InternStatic"),
                      "InternStatic") == 0);
    GBL_COMPARE(GblQuark_count(), 8);
    GBL_API_END();
}

GBL_API GblQuark_test_final(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GBL_API_VERIFY_CALL(GblQuark_final());
    GBL_API_END();
}



