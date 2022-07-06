#include "containers/gimbal_byte_array_test_suite.h"
#include <gimbal/utils/gimbal_test.h>
#include <gimbal/core/gimbal_api_frame.h>
#include <gimbal/containers/gimbal_byte_array.h>

#define GBL_BYTE_ARRAY_TEST_SUITE_(inst)    ((GblByteArrayTestSuite_*)GBL_INSTANCE_PRIVATE(inst, GBL_BYTE_ARRAY_TEST_SUITE_TYPE))

typedef struct GblByteArrayTestSuite_ {
    GblSize         refActiveCount;
    GblByteArray*   pByteArray1;
    GblByteArray*   pByteArray2;
} GblByteArrayTestSuite_;


static GBL_RESULT GblByteArrayTestSuite_init_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblByteArrayTestSuite_* pSelf_ = GBL_BYTE_ARRAY_TEST_SUITE_(pSelf);
    GBL_API_CLEAR_LAST_RECORD();
    pSelf_->refActiveCount = GblRef_activeCount();
    GBL_API_END();
}

static GBL_RESULT GblByteArrayTestSuite_final_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblByteArrayTestSuite_* pSelf_ = GBL_BYTE_ARRAY_TEST_SUITE_(pSelf);
    GBL_API_CLEAR_LAST_RECORD();
    GBL_TEST_COMPARE(GblByteArray_unref(pSelf_->pByteArray1), 0);
    GBL_TEST_COMPARE(pSelf_->refActiveCount,  GblRef_activeCount());
    GBL_API_END();
}

static GBL_RESULT GblByteArray_verify_(GblContext* pCtx,
                                       GblByteArray* pByteArray,
                                       const char* pData,
                                       GblRefCount refCount)
{
    GBL_API_BEGIN(pCtx);
    GblSize size = pData? strlen(pData)+1 : 0;
    GBL_TEST_COMPARE(GblByteArray_empty(pByteArray), size? 0 : 1);
    GBL_TEST_COMPARE(GblByteArray_size(pByteArray), size);
    GBL_TEST_COMPARE(GblByteArray_refCount(pByteArray), refCount);
    GBL_TEST_COMPARE(GblByteArray_context(pByteArray), pCtx);
    if(size) {
        char buffer[512];

        GBL_TEST_COMPARE(strcmp((const char*)GblByteArray_data(pByteArray),
                           pData),
                    0);
        GBL_TEST_COMPARE((char)GblByteArray_at(pByteArray, 0), pData[0]);

        GBL_API_VERIFY_CALL(GblByteArray_read(pByteArray, 0, size, buffer));
        GBL_TEST_COMPARE(strcmp(buffer, pData), 0);

        GBL_TEST_COMPARE(strcmp(GblByteArray_cString(pByteArray), pData), 0);

        GBL_TEST_VERIFY(GblStringView_equals(GblByteArray_stringView(pByteArray), GBL_STRV(pData)));

        // Byte array includes null terminator in hash!
        //GBL_TEST_COMPARE(GblByteArray_hash(pByteArray), GblStringView_hash(GblByteArray_stringView(pByteArray)));

    }
    else
        GBL_TEST_COMPARE(GblByteArray_data(pByteArray), NULL);
    GBL_API_END();

}


static GBL_RESULT GblByteArrayTestSuite_create_empty_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblByteArrayTestSuite_* pSelf_ = GBL_BYTE_ARRAY_TEST_SUITE_(pSelf);
    pSelf_->pByteArray1 = GblByteArray_create(0, NULL, pCtx);
    GBL_API_VERIFY_LAST_RECORD();
    GBL_TEST_VERIFY(pSelf_->pByteArray1);
    GBL_API_VERIFY_CALL(GblByteArray_verify_(pCtx,
                                             pSelf_->pByteArray1,
                                             NULL,
                                             1));
    GBL_API_END();
}

static GBL_RESULT GblByteArrayTestSuite_create_pointer_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblByteArrayTestSuite_* pSelf_ = GBL_BYTE_ARRAY_TEST_SUITE_(pSelf);
    pSelf_->pByteArray2 = GblByteArray_create(4, "lol", pCtx);
    GBL_API_VERIFY_LAST_RECORD();
    GBL_TEST_VERIFY(pSelf_->pByteArray2);
    GBL_API_VERIFY_CALL(GblByteArray_verify_(pCtx,
                                             pSelf_->pByteArray2,
                                             "lol",
                                             1));
    GBL_API_END();
}

static GBL_RESULT GblByteArrayTestSuite_copy_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblByteArrayTestSuite_* pSelf_ = GBL_BYTE_ARRAY_TEST_SUITE_(pSelf);
    GBL_API_VERIFY_CALL(GblByteArray_copy(pSelf_->pByteArray1, pSelf_->pByteArray2));
    GBL_API_VERIFY_CALL(GblByteArray_verify_(pCtx,
                                             pSelf_->pByteArray1,
                                             "lol",
                                             1));
    GBL_TEST_VERIFY(GblByteArray_compare(pSelf_->pByteArray1, pSelf_->pByteArray2) == 0);
    GBL_TEST_VERIFY(GblByteArray_equals(pSelf_->pByteArray1, pSelf_->pByteArray2));

    GBL_API_END();
}

static GBL_RESULT GblByteArrayTestSuite_set_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblByteArrayTestSuite_* pSelf_ = GBL_BYTE_ARRAY_TEST_SUITE_(pSelf);
    GBL_API_VERIFY_CALL(GblByteArray_set(pSelf_->pByteArray1, 4, "HEY"));
    GBL_API_VERIFY_CALL(GblByteArray_verify_(pCtx,
                                             pSelf_->pByteArray1,
                                             "HEY",
                                             1));
    GBL_API_END();
}


static GBL_RESULT GblByteArrayTestSuite_move_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblByteArrayTestSuite_* pSelf_ = GBL_BYTE_ARRAY_TEST_SUITE_(pSelf);
    GBL_API_VERIFY_CALL(GblByteArray_move(pSelf_->pByteArray1, pSelf_->pByteArray2));
    GBL_API_VERIFY_CALL(GblByteArray_verify_(pCtx,
                                             pSelf_->pByteArray1,
                                             "lol",
                                             1));
    GBL_API_VERIFY_CALL(GblByteArray_verify_(pCtx,
                                             pSelf_->pByteArray2,
                                             NULL,
                                             1));

    GBL_API_END();
}

static GBL_RESULT GblByteArrayTestSuite_ref_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblByteArrayTestSuite_* pSelf_ = GBL_BYTE_ARRAY_TEST_SUITE_(pSelf);
    GblByteArray * pArray= GblByteArray_ref(pSelf_->pByteArray1);
    GBL_API_VERIFY_CALL(GblByteArray_verify_(pCtx,
                                             pArray,
                                             "lol",
                                             2));

    GBL_API_END();
}

static GBL_RESULT GblByteArrayTestSuite_unref_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblByteArrayTestSuite_* pSelf_ = GBL_BYTE_ARRAY_TEST_SUITE_(pSelf);
    GBL_TEST_COMPARE(GblByteArray_unref(pSelf_->pByteArray1), 1);
    GBL_API_VERIFY_CALL(GblByteArray_verify_(pCtx,
                                             pSelf_->pByteArray1,
                                             "lol",
                                             1));

    GblSize refCount = GblRef_activeCount();
    GBL_TEST_COMPARE(GblByteArray_unref(pSelf_->pByteArray2), 0);
    GBL_TEST_COMPARE(refCount, GblRef_activeCount()+1);

    GBL_API_END();
}

static GBL_RESULT GblByteArrayTestSuite_write_read_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblByteArrayTestSuite_* pSelf_ = GBL_BYTE_ARRAY_TEST_SUITE_(pSelf);
    GBL_API_VERIFY_CALL(GblByteArray_set(pSelf_->pByteArray1, 7, "abcdef"));
    GBL_API_VERIFY_CALL(GblByteArray_verify_(pCtx,
                                             pSelf_->pByteArray1,
                                             "abcdef",
                                             1));
    GBL_API_VERIFY_CALL(GblByteArray_write(pSelf_->pByteArray1,
                                           2,
                                           3,
                                           "xxx"));
    GBL_API_VERIFY_CALL(GblByteArray_verify_(pCtx,
                                             pSelf_->pByteArray1,
                                             "abxxxf",
                                             1));
    char buffer[4] = "";
    GBL_API_VERIFY_CALL(GblByteArray_read(pSelf_->pByteArray1,
                                          2,
                                          3,
                                          buffer));
    GBL_TEST_COMPARE(strcmp(buffer, "xxx"),  0);
    GBL_API_END();
}


static GBL_RESULT GblByteArrayTestSuite_write_read_invalid_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblByteArrayTestSuite_* pSelf_ = GBL_BYTE_ARRAY_TEST_SUITE_(pSelf);
    GBL_TEST_EXPECT_ERROR();

    GBL_TEST_COMPARE(GblByteArray_write(pSelf_->pByteArray1, 12, 33, "lolol"),
                GBL_RESULT_ERROR_OUT_OF_RANGE);
    GBL_API_CLEAR_LAST_RECORD();

    GBL_TEST_COMPARE(GblByteArray_read(pSelf_->pByteArray1, 12, 33, NULL),
                GBL_RESULT_ERROR_OUT_OF_RANGE);
    GBL_API_CLEAR_LAST_RECORD();

    GBL_TEST_COMPARE(GblByteArray_at(pSelf_->pByteArray1, GblByteArray_size(pSelf_->pByteArray1)+1),
                0);
    GBL_API_CLEAR_LAST_RECORD();

    GBL_API_END();
}

static GBL_RESULT GblByteArrayTestSuite_acquire_release_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblByteArrayTestSuite_* pSelf_ = GBL_BYTE_ARRAY_TEST_SUITE_(pSelf);
    GblStringView strv = GBL_STRV("Megaman");
    GBL_API_VERIFY_CALL(GblByteArray_acquire(pSelf_->pByteArray1,
                                             strv.length+1,
                                             GblStringView_strdup(strv)));
    GBL_API_VERIFY_CALL(GblByteArray_verify_(pCtx,
                                             pSelf_->pByteArray1,
                                             "Megaman",
                                             1));
    char* pBuffer = NULL;
    GblSize size = 0;
    GBL_API_VERIFY_CALL(GblByteArray_release(pSelf_->pByteArray1, &size, (void**)&pBuffer));
    GBL_TEST_COMPARE(size, strv.length+1);
    GBL_TEST_COMPARE_STRING(pBuffer, "Megaman");
    GBL_ALIGNED_FREE(pBuffer);

    GBL_API_END();
}

static GBL_RESULT GblByteArrayTestSuite_resize_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblByteArrayTestSuite_* pSelf_ = GBL_BYTE_ARRAY_TEST_SUITE_(pSelf);
    GBL_API_VERIFY_CALL(GblByteArray_resize(pSelf_->pByteArray1, 10));
    GBL_TEST_COMPARE(GblByteArray_size(pSelf_->pByteArray1), 10);
    for(unsigned i = 0; i < 10; ++i) {
        GBL_TEST_COMPARE(GblByteArray_at(pSelf_->pByteArray1, i), 0);
        GBL_API_VERIFY_LAST_RECORD();
    }

    GBL_API_END();
}
static GBL_RESULT GblByteArrayTestSuite_grow_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblByteArrayTestSuite_* pSelf_ = GBL_BYTE_ARRAY_TEST_SUITE_(pSelf);

    GBL_API_VERIFY_CALL(GblByteArray_set(pSelf_->pByteArray1, 3, "XYZ"));
    GBL_API_VERIFY_CALL(GblByteArray_grow(pSelf_->pByteArray1, 1));
    GBL_API_VERIFY_CALL(GblByteArray_verify_(pCtx,
                                             pSelf_->pByteArray1,
                                             "XYZ",
                                             1));

    GBL_API_END();
}
static GBL_RESULT GblByteArrayTestSuite_shrink_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblByteArrayTestSuite_* pSelf_ = GBL_BYTE_ARRAY_TEST_SUITE_(pSelf);
    GBL_API_VERIFY_CALL(GblByteArray_shrink(pSelf_->pByteArray1, 2));
    GBL_API_VERIFY_CALL(GblByteArray_grow(pSelf_->pByteArray1, 2));

    GBL_TEST_COMPARE(GblByteArray_size(pSelf_->pByteArray1), 4);

    GBL_TEST_COMPARE(strcmp(GblByteArray_cString(pSelf_->pByteArray1),
                                            "XY"), 0);
    GBL_API_END();
}
static GBL_RESULT GblByteArrayTestSuite_erase_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblByteArrayTestSuite_* pSelf_ = GBL_BYTE_ARRAY_TEST_SUITE_(pSelf);
    GBL_API_VERIFY_CALL(GblByteArray_set(pSelf_->pByteArray1, 4, "XYZ"));
    GBL_API_VERIFY_CALL(GblByteArray_erase(pSelf_->pByteArray1, 1, 2));
    GBL_API_VERIFY_CALL(GblByteArray_verify_(pCtx,
                                             pSelf_->pByteArray1,
                                             "X",
                                             1));
    GBL_API_END();
}
static GBL_RESULT GblByteArrayTestSuite_insert_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblByteArrayTestSuite_* pSelf_ = GBL_BYTE_ARRAY_TEST_SUITE_(pSelf);
    GBL_API_VERIFY_CALL(GblByteArray_set(pSelf_->pByteArray1, 4, "XYZ"));
    GBL_API_VERIFY_CALL(GblByteArray_insert(pSelf_->pByteArray1, 1, 2, "AB"));
    GBL_API_VERIFY_CALL(GblByteArray_verify_(pCtx,
                                             pSelf_->pByteArray1,
                                             "XABYZ",
                                             1));
    GBL_API_END();
}
static GBL_RESULT GblByteArrayTestSuite_append_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblByteArrayTestSuite_* pSelf_ = GBL_BYTE_ARRAY_TEST_SUITE_(pSelf);
    GBL_API_VERIFY_CALL(GblByteArray_shrink(pSelf_->pByteArray1, 1));
    GBL_API_VERIFY_CALL(GblByteArray_append(pSelf_->pByteArray1, 4,"QRT"));
    GBL_API_VERIFY_CALL(GblByteArray_verify_(pCtx,
                                             pSelf_->pByteArray1,
                                             "XABYZQRT",
                                             1));
    GBL_API_END();
}
static GBL_RESULT GblByteArrayTestSuite_prepend_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblByteArrayTestSuite_* pSelf_ = GBL_BYTE_ARRAY_TEST_SUITE_(pSelf);
    GBL_API_VERIFY_CALL(GblByteArray_prepend(pSelf_->pByteArray1, 8,"Hogwarts"));
    GBL_API_VERIFY_CALL(GblByteArray_verify_(pCtx,
                                             pSelf_->pByteArray1,
                                             "HogwartsXABYZQRT",
                                             1));
    GBL_API_END();
}
static GBL_RESULT GblByteArrayTestSuite_clear_(GblTestSuite* pSelf, GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblByteArrayTestSuite_* pSelf_ = GBL_BYTE_ARRAY_TEST_SUITE_(pSelf);
    GBL_API_VERIFY_CALL(GblByteArray_clear(pSelf_->pByteArray1));
    GBL_API_VERIFY_CALL(GblByteArray_verify_(pCtx,
                                             pSelf_->pByteArray1,
                                             NULL,
                                             1));
    GBL_API_END();
}


GBL_EXPORT GblType GblByteArrayTestSuite_type(void) {
    static GblType type = GBL_INVALID_TYPE;

    const static GblTestCase cases[] = {
        { "createEmpty",        GblByteArrayTestSuite_create_empty_         },
        { "createPointer",      GblByteArrayTestSuite_create_pointer_       },
        { "copy",               GblByteArrayTestSuite_copy_                 },
        { "set",                GblByteArrayTestSuite_set_                  },
        { "move",               GblByteArrayTestSuite_move_                 },
        { "ref",                GblByteArrayTestSuite_ref_                  },
        { "unref",              GblByteArrayTestSuite_unref_                },
        { "writeRead",          GblByteArrayTestSuite_write_read_           },
        { "writeReadInvalid",   GblByteArrayTestSuite_write_read_invalid_   },
        { "acquireRelease",     GblByteArrayTestSuite_acquire_release_      },
        { "resize",             GblByteArrayTestSuite_resize_               },
        { "grow",               GblByteArrayTestSuite_grow_                 },
        { "shrink",             GblByteArrayTestSuite_shrink_               },
        { "erase",              GblByteArrayTestSuite_erase_                },
        { "insert",             GblByteArrayTestSuite_insert_               },
        { "append",             GblByteArrayTestSuite_append_               },
        { "prepend",            GblByteArrayTestSuite_prepend_              },
        { "clear",              GblByteArrayTestSuite_clear_                },
        { NULL,                 NULL                                        }
    };

    const static GblTestSuiteClassVTable vTable = {
        .pFnSuiteInit   = GblByteArrayTestSuite_init_,
        .pFnSuiteFinal  = GblByteArrayTestSuite_final_,
        .pCases         = cases
    };

    if(type == GBL_INVALID_TYPE) {
        GBL_API_BEGIN(NULL);
        type = GblTestSuite_register(GblQuark_internStringStatic("ByteArrayTestSuite"),
                                     &vTable,
                                     sizeof(GblByteArrayTestSuite),
                                     sizeof(GblByteArrayTestSuite_),
                                     GBL_TYPE_FLAGS_NONE);
        GBL_API_VERIFY_LAST_RECORD();
        GBL_API_END_BLOCK();
    }
    return type;
}


