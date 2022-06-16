#include "containers/test_gimbal_byte_array.h"
#include <gimbal/core/gimbal_api_frame.h>
#include <gimbal/containers/gimbal_byte_array.h>

GblSize         refActiveCount_ = 0;
GblByteArray*   pByteArray1_    = NULL;
GblByteArray*   pByteArray2_    = NULL;

GBL_API GblByteArray_test_testCaseInit(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GBL_API_END();
}
GBL_API GblByteArray_test_testCaseFinal(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GBL_API_END();
}
GBL_API GblByteArray_test_init(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GBL_API_CLEAR_LAST_RECORD();
    refActiveCount_ = GblRef_activeCount();
    GBL_API_END();
}

GBL_API GblByteArray_verify_(GblContext* pCtx,
                             GblByteArray* pByteArray,
                             const char* pData,
                             GblRefCount refCount)
{
    GBL_API_BEGIN(pCtx);
    GblSize size = pData? strlen(pData)+1 : 0;
    GBL_COMPARE(GblByteArray_empty(pByteArray), size? 0 : 1);
    GBL_COMPARE(GblByteArray_size(pByteArray), size);
    GBL_COMPARE(GblByteArray_refCount(pByteArray), refCount);
    GBL_COMPARE(GblByteArray_context(pByteArray), pCtx);
    if(size) {
        char buffer[512];

        GBL_COMPARE(strcmp((const char*)GblByteArray_data(pByteArray),
                           pData),
                    0);
        GBL_COMPARE((char)GblByteArray_at(pByteArray, 0), pData[0]);

        GBL_API_VERIFY_CALL(GblByteArray_read(pByteArray, 0, size, buffer));
        GBL_COMPARE(strcmp(buffer, pData), 0);

        GBL_COMPARE(strcmp(GblByteArray_cString(pByteArray), pData), 0);

        GBL_VERIFY(GblStringView_equals(GblByteArray_stringView(pByteArray), GBL_STRV(pData)));

        // Byte array includes null terminator in hash!
        //GBL_COMPARE(GblByteArray_hash(pByteArray), GblStringView_hash(GblByteArray_stringView(pByteArray)));

    }
    else
        GBL_COMPARE(GblByteArray_data(pByteArray), NULL);
    GBL_API_END();

}


GBL_API GblByteArray_test_create_empty(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    pByteArray1_ = GblByteArray_create(0, NULL, pCtx);
    GBL_API_VERIFY_LAST_RECORD();
    GBL_VERIFY(pByteArray1_);
    GBL_API_VERIFY_CALL(GblByteArray_verify_(pCtx,
                                             pByteArray1_,
                                             NULL,
                                             1));
    GBL_API_END();
}

GBL_API GblByteArray_test_create_pointer(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    pByteArray2_ = GblByteArray_create(4, "lol", pCtx);
    GBL_API_VERIFY_LAST_RECORD();
    GBL_VERIFY(pByteArray2_);
    GBL_API_VERIFY_CALL(GblByteArray_verify_(pCtx,
                                             pByteArray2_,
                                             "lol",
                                             1));
    GBL_API_END();
}

GBL_API GblByteArray_test_copy(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GBL_API_VERIFY_CALL(GblByteArray_copy(pByteArray1_, pByteArray2_));
    GBL_API_VERIFY_CALL(GblByteArray_verify_(pCtx,
                                             pByteArray1_,
                                             "lol",
                                             1));
    GBL_VERIFY(GblByteArray_compare(pByteArray1_, pByteArray2_) == 0);
    GBL_VERIFY(GblByteArray_equals(pByteArray1_, pByteArray2_));

    GBL_API_END();
}

GBL_API GblByteArray_test_set(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GBL_API_VERIFY_CALL(GblByteArray_set(pByteArray1_, 4, "HEY"));
    GBL_API_VERIFY_CALL(GblByteArray_verify_(pCtx,
                                             pByteArray1_,
                                             "HEY",
                                             1));
    GBL_API_END();
}


GBL_API GblByteArray_test_move(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GBL_API_VERIFY_CALL(GblByteArray_move(pByteArray1_, pByteArray2_));
    GBL_API_VERIFY_CALL(GblByteArray_verify_(pCtx,
                                             pByteArray1_,
                                             "lol",
                                             1));
    GBL_API_VERIFY_CALL(GblByteArray_verify_(pCtx,
                                             pByteArray2_,
                                             NULL,
                                             1));

    GBL_API_END();
}

GBL_API GblByteArray_test_ref(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblByteArray * pArray= GblByteArray_ref(pByteArray1_);
    GBL_API_VERIFY_CALL(GblByteArray_verify_(pCtx,
                                             pArray,
                                             "lol",
                                             2));

    GBL_API_END();
}

GBL_API GblByteArray_test_unref(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GBL_COMPARE(GblByteArray_unref(pByteArray1_), 1);
    GBL_API_VERIFY_CALL(GblByteArray_verify_(pCtx,
                                             pByteArray1_,
                                             "lol",
                                             1));

    GblSize refCount = GblRef_activeCount();
    GBL_COMPARE(GblByteArray_unref(pByteArray2_), 0);
    GBL_COMPARE(refCount, GblRef_activeCount()+1);

    GBL_API_END();
}

GBL_API GblByteArray_test_write_read(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GBL_API_VERIFY_CALL(GblByteArray_set(pByteArray1_, 7, "abcdef"));
    GBL_API_VERIFY_CALL(GblByteArray_verify_(pCtx,
                                             pByteArray1_,
                                             "abcdef",
                                             1));
    GBL_API_VERIFY_CALL(GblByteArray_write(pByteArray1_,
                                           2,
                                           3,
                                           "xxx"));
    GBL_API_VERIFY_CALL(GblByteArray_verify_(pCtx,
                                             pByteArray1_,
                                             "abxxxf",
                                             1));
    char buffer[4] = "";
    GBL_API_VERIFY_CALL(GblByteArray_read(pByteArray1_,
                                          2,
                                          3,
                                          buffer));
    GBL_COMPARE(strcmp(buffer, "xxx"),  0);
    GBL_API_END();
}


GBL_API GblByteArray_test_write_read_invalid(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GBL_TEST_EXPECT_ERROR();

    GBL_COMPARE(GblByteArray_write(pByteArray1_, 12, 33, "lolol"),
                GBL_RESULT_ERROR_OUT_OF_RANGE);
    GBL_API_CLEAR_LAST_RECORD();

    GBL_COMPARE(GblByteArray_read(pByteArray1_, 12, 33, NULL),
                GBL_RESULT_ERROR_OUT_OF_RANGE);
    GBL_API_CLEAR_LAST_RECORD();

    GBL_COMPARE(GblByteArray_at(pByteArray1_, GblByteArray_size(pByteArray1_)+1),
                0);
    GBL_API_CLEAR_LAST_RECORD();

    GBL_API_END();
}

GBL_API GblByteArray_test_acquire_release(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblStringView strv = GBL_STRV("Megaman");
    GBL_API_VERIFY_CALL(GblByteArray_acquire(pByteArray1_,
                                             strv.length+1,
                                             GblStringView_strdup(strv)));
    GBL_API_VERIFY_CALL(GblByteArray_verify_(pCtx,
                                             pByteArray1_,
                                             "Megaman",
                                             1));
    char* pBuffer = NULL;
    GblSize size = 0;
    GBL_API_VERIFY_CALL(GblByteArray_release(pByteArray1_, &size, (void**)&pBuffer));
    GBL_COMPARE(size, strv.length+1);
    GBL_VERIFY(strcmp(pBuffer, "Megaman") == 0);
    GBL_ALIGNED_FREE(pBuffer);

    GBL_API_END();
}

GBL_API GblByteArray_test_resize(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);

    GBL_API_VERIFY_CALL(GblByteArray_resize(pByteArray1_, 10));
    GBL_COMPARE(GblByteArray_size(pByteArray1_), 10);
    for(unsigned i = 0; i < 10; ++i) {
        GBL_COMPARE(GblByteArray_at(pByteArray1_, i), 0);
        GBL_API_VERIFY_LAST_RECORD();
    }

    GBL_API_END();
}
GBL_API GblByteArray_test_grow(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);

    GBL_API_VERIFY_CALL(GblByteArray_set(pByteArray1_, 3, "XYZ"));
    GBL_API_VERIFY_CALL(GblByteArray_grow(pByteArray1_, 1));
    GBL_API_VERIFY_CALL(GblByteArray_verify_(pCtx,
                                             pByteArray1_,
                                             "XYZ",
                                             1));

    GBL_API_END();
}
GBL_API GblByteArray_test_shrink(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GBL_API_VERIFY_CALL(GblByteArray_shrink(pByteArray1_, 2));
    GBL_API_VERIFY_CALL(GblByteArray_grow(pByteArray1_, 2));

    GBL_COMPARE(GblByteArray_size(pByteArray1_), 4);

    GBL_COMPARE(strcmp(GblByteArray_cString(pByteArray1_),
                                            "XY"), 0);
    GBL_API_END();
}
GBL_API GblByteArray_test_erase(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GBL_API_VERIFY_CALL(GblByteArray_set(pByteArray1_, 4, "XYZ"));
    GBL_API_VERIFY_CALL(GblByteArray_erase(pByteArray1_, 1, 2));
    GBL_API_VERIFY_CALL(GblByteArray_verify_(pCtx,
                                             pByteArray1_,
                                             "X",
                                             1));
    GBL_API_END();
}
GBL_API GblByteArray_test_insert(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GBL_API_VERIFY_CALL(GblByteArray_set(pByteArray1_, 4, "XYZ"));
    GBL_API_VERIFY_CALL(GblByteArray_insert(pByteArray1_, 1, 2, "AB"));
    GBL_API_VERIFY_CALL(GblByteArray_verify_(pCtx,
                                             pByteArray1_,
                                             "XABYZ",
                                             1));
    GBL_API_END();
}
GBL_API GblByteArray_test_append(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GBL_API_VERIFY_CALL(GblByteArray_shrink(pByteArray1_, 1));
    GBL_API_VERIFY_CALL(GblByteArray_append(pByteArray1_, 4,"QRT"));
    GBL_API_VERIFY_CALL(GblByteArray_verify_(pCtx,
                                             pByteArray1_,
                                             "XABYZQRT",
                                             1));
    GBL_API_END();
}
GBL_API GblByteArray_test_prepend(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);

    GBL_API_VERIFY_CALL(GblByteArray_prepend(pByteArray1_, 8,"Hogwarts"));
    GBL_API_VERIFY_CALL(GblByteArray_verify_(pCtx,
                                             pByteArray1_,
                                             "HogwartsXABYZQRT",
                                             1));
    GBL_API_END();
}
GBL_API GblByteArray_test_clear(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);

    GBL_API_VERIFY_CALL(GblByteArray_clear(pByteArray1_));
    GBL_API_VERIFY_CALL(GblByteArray_verify_(pCtx,
                                             pByteArray1_,
                                             NULL,
                                             1));
    GBL_API_END();
}

GBL_API GblByteArray_test_final(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GBL_API_CLEAR_LAST_RECORD();
    GBL_COMPARE(GblByteArray_unref(pByteArray1_), 0);
    GBL_COMPARE(refActiveCount_,  GblRef_activeCount());
    GBL_API_END();
}

