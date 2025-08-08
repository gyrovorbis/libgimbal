#include "core/gimbal_thread_test_suite.h"
#include <gimbal/test/gimbal_test_macros.h>
#include <gimbal/core/gimbal_thread.h>
#include <gimbal/utils/gimbal_ref.h>

#include <tinycthread.h>
#include <stdatomic.h>

#define GBL_SELF_TYPE GblThreadTestSuite

#define GBL_TEST_THREAD_TYPE_                   (GBL_TYPEID(GblTestThread))
#define GBL_TEST_THREAD_(self)                  (GBL_CAST(GblTestThread, self))
#define GBL_TEST_THREAD_CLASS_(klass)           (GBL_CLASS_CAST(GblTestThread, klass))
#define GBL_TEST_THREAD_GET_CLASS_(self)        (GBL_CLASSOF(GblTestThread, self))

#define GBL_TEST_THREAD_TLS_THREAD_COUNT_       5
#define GBL_TEST_THREAD_TLS_WRITE_ITERATIONS_   10

GBL_CLASS_DERIVE(GblTestThread, GblThread)
    atomic_short runningCount;
GBL_CLASS_END

GBL_INSTANCE_DERIVE(GblTestThread, GblThread)
    volatile unsigned threadId;

    volatile GblBool tlsInitRan;
    volatile GblBool tlsReadWriteRan;

    volatile GblBool tlsInitBssPass;
    volatile GblBool tlsInitDataPass;
    volatile GblBool tlsInitAlignPass;

    volatile GblBool tlsReadWritePass;
GBL_INSTANCE_END

typedef struct {
    uint8_t inner[3];
} Align4_;

typedef struct {
    uint8_t inner[3];
} Align16_;

GBL_TEST_FIXTURE {
    GblThread*       pThreadMain;
    uint16_t         refCount;
    size_t           threadCount;
    uint16_t         classRefCount;
    uint16_t         instanceCount;

    GblThread*       pThread1;
    volatile GblBool thread1Started;
    volatile GblBool thread1Ran;
    volatile GblBool thread1Finished;

    volatile GblBool thread2Ran;
    volatile GblBool thread2Finished;

    GblTestThread*   pTestThreads[GBL_TEST_THREAD_TLS_THREAD_COUNT_];
    volatile GblBool testThreadRan[GBL_TEST_THREAD_TLS_THREAD_COUNT_];
};

static GblType GblTestThread_type(void);

static volatile GBL_THREAD_LOCAL GBL_ALIGNAS(4)  Align4_       tlsBuff4_       = {.inner = {2, 2, 2}};
static volatile GBL_THREAD_LOCAL GBL_ALIGNAS(16) Align16_      tlsBuff16_      = {.inner = {1, 1, 1}};
static volatile GBL_THREAD_LOCAL                 uint16_t      tlsUint16_      = 0;
static volatile GBL_THREAD_LOCAL GBL_ALIGNAS(16) int32_t       tlsInt32_       = -346;
static volatile GBL_THREAD_LOCAL                 GblStringRef* tlsStringRef_   = NULL;
static volatile GBL_THREAD_LOCAL GBL_ALIGNAS(32) char          tlsCharArray_[] = { "abcdefghijklmnopqrstuvwxyz012345" };
static volatile GBL_THREAD_LOCAL GBL_ALIGNAS(64) GblObject     tlsObject_;
static volatile GBL_THREAD_LOCAL GBL_ALIGNAS(8)  double        tlsDouble_      = 12345.6789;

static GBL_RESULT GblTestThread_tlsInitCheck_(GblTestThread* pSelf) {
    GBL_CTX_BEGIN(NULL);

    // check TLS BSS zeroed values
    GBL_TEST_COMPARE(tlsUint16_, 0);
    GBL_TEST_COMPARE((const char*)tlsStringRef_, NULL);
    for(size_t b = 0; b < sizeof(GblObject); ++b)
        GBL_TEST_COMPARE(*(((uint8_t*)&tlsObject_) + 1), 0);

    pSelf->tlsInitBssPass = GBL_TRUE;

    // check TSL DATA initialized values
    GBL_TEST_COMPARE(tlsInt32_, -346);
    GBL_TEST_COMPARE((const char*)tlsCharArray_, "abcdefghijklmnopqrstuvwxyz012345");
    GBL_TEST_COMPARE(tlsDouble_, 12345.6789);

    for(unsigned b = 0; b < 3; ++b)
        GBL_TEST_COMPARE(tlsBuff4_.inner[b], 2);

    for(unsigned b = 0; b < 3; ++b)
        GBL_TEST_COMPARE(tlsBuff16_.inner[b], 1);

    pSelf->tlsInitDataPass = GBL_TRUE;

    // check alignment
    GBL_TEST_VERIFY(!((uintptr_t)&tlsInt32_    & 0xf));
    GBL_TEST_VERIFY(!((uintptr_t)tlsCharArray_ & 0x1f));
    GBL_TEST_VERIFY(!((uintptr_t)&tlsObject_   & 0x3f));
    GBL_TEST_VERIFY(!((uintptr_t)&tlsDouble_   & 0x7));

    GBL_TEST_VERIFY(!((uintptr_t)&tlsBuff4_    & 0x3));
    GBL_TEST_VERIFY(!((uintptr_t)&tlsBuff16_   & 0xf));

    pSelf->tlsInitAlignPass = GBL_TRUE;

    GBL_CTX_END_BLOCK();
    pSelf->tlsInitRan = GBL_TRUE;
    return GBL_CTX_RESULT();
}

static GBL_RESULT GblTestThread_tlsReadWriteCheck_(GblTestThread* pSelf) {
    GBL_CTX_BEGIN(NULL);

    // initialize all the TLS data to thread-specific shit
    tlsUint16_ = pSelf->threadId;
    tlsInt32_  = pSelf->threadId;
    tlsDouble_ = pSelf->threadId;

    for(unsigned b = 0; b < 3; ++b)
        tlsBuff4_.inner[b] = pSelf->threadId;

    for(unsigned b = 0; b < 3; ++b)
        tlsBuff16_.inner[b] = pSelf->threadId;


    // Now ping-pong increemnting values between threads
    for(unsigned i = 0; i < GBL_TEST_THREAD_TLS_WRITE_ITERATIONS_; ++i) {
        ++tlsUint16_;
        ++tlsInt32_;
        tlsDouble_ += 1.0;

        for(unsigned b = 0; b < 3; ++b)
            ++tlsBuff4_.inner[b];

        for(unsigned b = 0; b < 3; ++b)
            ++tlsBuff16_.inner[b];

        GBL_CTX_VERIFY_CALL(GblThread_nanoSleep(10000));
    }

    GBL_TEST_COMPARE(tlsUint16_, pSelf->threadId + GBL_TEST_THREAD_TLS_WRITE_ITERATIONS_);
    GBL_TEST_COMPARE(tlsInt32_, pSelf->threadId + GBL_TEST_THREAD_TLS_WRITE_ITERATIONS_);
    GBL_TEST_COMPARE(tlsDouble_, pSelf->threadId + GBL_TEST_THREAD_TLS_WRITE_ITERATIONS_);

    for(unsigned b = 0; b < 3; ++b)
        GBL_TEST_COMPARE(tlsBuff4_.inner[b], pSelf->threadId + GBL_TEST_THREAD_TLS_WRITE_ITERATIONS_);

    for(unsigned b = 0; b < 3; ++b)
        GBL_TEST_COMPARE(tlsBuff16_.inner[b], pSelf->threadId + GBL_TEST_THREAD_TLS_WRITE_ITERATIONS_);

    pSelf->tlsReadWritePass = GBL_TRUE;

    GBL_CTX_END_BLOCK();
    pSelf->tlsReadWriteRan = GBL_TRUE;
    return GBL_CTX_RESULT();
}

static GBL_RESULT GblTestThread_GblThread_run_(GblThread* pThread) {
    GBL_CTX_BEGIN(NULL);

    GblTestThread* pSelf = GBL_TEST_THREAD_(pThread);

    GBL_CTX_CALL(GblTestThread_tlsInitCheck_(pSelf));
    GBL_CTX_CALL(GblTestThread_tlsReadWriteCheck_(pSelf));

    GBL_VCALL_DEFAULT(GblThread, pFnRun, pThread);

    GBL_CTX_END();
}

static GBL_RESULT GblTestThreadClass_init_(GblClass* pClass, const void* pUd) {
    GBL_CTX_BEGIN(NULL);

    GBL_THREAD_CLASS(pClass)->pFnRun = GblTestThread_GblThread_run_;

    GBL_CTX_END();
}

static GblType GblTestThread_type(void) {
    static GblType type = GBL_INVALID_TYPE;

    if(type == GBL_INVALID_TYPE) {
        type = GblType_register(GblQuark_internStatic("GblTestThread"),
                                      GBL_THREAD_TYPE,
                                      &(GblTypeInfo) {
                                          .pFnClassInit = GblTestThreadClass_init_,
                                          .classSize    = sizeof(GblTestThreadClass),
                                          .instanceSize = sizeof(GblTestThread)
                                      },
                                      GBL_TYPE_FLAGS_NONE);
    }

    return type;
}

GBL_TEST_INIT()
    /* Force main thread to be allocated/managed.
       Not necessarily true, but don't support GblThread_main() yet! */
    pFixture->pThreadMain   = GblThread_current();
    // Cache current # of threads
    pFixture->threadCount   = GblThread_count();
    // Cache current # of active references
    pFixture->refCount      = GblRef_activeCount();
    // Cache current # of active refs to threads' class
    pFixture->classRefCount = GblType_classRefCount(GBL_THREAD_TYPE);
    // Cache current # of active threads instances
    pFixture->instanceCount = GblType_instanceCount(GBL_THREAD_TYPE);
GBL_TEST_CASE_END

GBL_TEST_FINAL()
    // Ensure we didn't screw up the main thread or deallocate it
    GBL_TEST_COMPARE(GblThread_current(), pFixture->pThreadMain);
    // Ensure all the new threads are gone
    GBL_TEST_COMPARE(GblThread_count(), pFixture->threadCount);
    // Ensure we aren't leaking anything
    GBL_TEST_COMPARE(GblRef_activeCount(), pFixture->refCount);
    // Ensure no class reference leaks
//    GBL_TEST_COMPARE(GblType_classRefCount(GBL_THREAD_TYPE), pFixture->classRefCount);
    // Ensure no instance leaks
//    GBL_TEST_COMPARE(GblType_instanceCount(GBL_THREAD_TYPE), pFixture->instanceCount);
GBL_TEST_CASE_END

/* Just toggle a boolean on the test fixture to signal
 * to the main test thread that this dude ran.
 */

static GBL_RESULT threadToggleRun_(GblThread* pSelf) {
    volatile GblBool* pToggle = GblBox_userdata(GBL_BOX(pSelf));
    *pToggle = GBL_TRUE;
    return GBL_RESULT_SUCCESS;
}

static void thread1OnStarted_(GblThread* pSelf) {
    GblTestFixture* pFixture = GblClosure_currentUserdata();
    pFixture->thread1Started = GBL_TRUE;
}

static void thread1OnFinished_(GblThread* pSelf, GblEnum result) {
    GblTestFixture* pFixture = GblClosure_currentUserdata();
    pFixture->thread1Finished = GBL_TRUE;
}

// Create a new test thread
GBL_TEST_CASE(new)
    // Spawn a new thread with property constructor
    pFixture->pThread1 = GBL_NEW(GblThread,
                                 "name",     "TestThread1",
                                 "userdata", &pFixture->thread1Ran,
                                 "callback", threadToggleRun_);

    // Basic sanity checks
    GBL_TEST_VERIFY(pFixture->pThread1);

    // Validate properties
    GBL_TEST_COMPARE(GblObject_name(GBL_OBJECT(pFixture->pThread1)), "TestThread1");
    GBL_TEST_COMPARE(GblBox_userdata(GBL_BOX(pFixture->pThread1)), (void*)&pFixture->thread1Ran);
    GBL_TEST_COMPARE(GblThread_callback(pFixture->pThread1), threadToggleRun_);

    // connect signals
    GBL_CONNECT(pFixture->pThread1,
                "started",
                pFixture->pThread1,
                thread1OnStarted_,
                pFixture);

    GBL_CONNECT(pFixture->pThread1,
                "finished",
                pFixture->pThread1,
                thread1OnFinished_,
                pFixture);

    // ensure the guy is ready
    GBL_TEST_COMPARE(pFixture->pThread1->state, GBL_THREAD_STATE_READY);
GBL_TEST_CASE_END

// Actually kick off test thread
GBL_TEST_CASE(start)
    GBL_TEST_CALL(GblThread_start(pFixture->pThread1));
GBL_TEST_CASE_END

GBL_TEST_CASE(started)
    while(!pFixture->thread1Started);
#if 0 //fixme, fragile test!!!
    GBL_TEST_COMPARE(GblThread_count(),
                     pFixture->thread1Finished?
                         pFixture->threadCount :
                         pFixture->threadCount + 1);
#endif
GBL_TEST_CASE_END

GBL_TEST_CASE(finished)
    while(!pFixture->thread1Finished);
GBL_TEST_CASE_END

// Wait on thread to complete
GBL_TEST_CASE(join)
    // Join then verify
    GBL_TEST_CALL(GblThread_join(pFixture->pThread1));
    GBL_TEST_VERIFY(GblThread_isJoined(pFixture->pThread1));

    // Ensure dude did his business
    GBL_TEST_VERIFY(pFixture->thread1Ran);
    // Ensure dude signaled he was done
    GBL_TEST_COMPARE(pFixture->pThread1->state, GBL_THREAD_STATE_FINISHED);
GBL_TEST_CASE_END

static void thread2OnFinished_(GblThread* pSelf, GblEnum result) {
    GBL_UNUSED(pSelf);
    GblTestFixture* pFixture = GblClosure_currentUserdata();
    pFixture->thread2Finished = GBL_TRUE;
}

GBL_TEST_CASE(detach)
    GblThread* pThread = GblThread_create(threadToggleRun_,
                                          (void*)&pFixture->thread2Ran,
                                          GBL_FALSE);

    GBL_CONNECT(pThread, "finished", pThread, thread2OnFinished_, pFixture);

    GblThread_start(pThread);
    GblThread_unref(pThread);

    while(!pFixture->thread2Ran);
    while(!pFixture->thread2Finished);
GBL_TEST_CASE_END

GBL_TEST_CASE(tlsSpawnThreads)
#if defined(GBL_PSP) || defined(GBL_GAMECUBE)
    GBL_TEST_SKIP("Unimplemented on PSP without `thread_local` support!");
#endif

    for(unsigned t = 0; t < GBL_TEST_THREAD_TLS_THREAD_COUNT_; ++t) {
        char nameBuff[64];

        snprintf(nameBuff, sizeof(nameBuff), "TLSTestThread%u", t);
        pFixture->pTestThreads[t] = GBL_NEW(GblTestThread,
                                            "name",     nameBuff,
                                            "userdata", &pFixture->testThreadRan[t],
                                            "callback", threadToggleRun_);

        pFixture->pTestThreads[t]->threadId = t;

        GBL_TEST_CALL(GblThread_start(GBL_THREAD(pFixture->pTestThreads[t])));
    }
GBL_TEST_CASE_END

GBL_TEST_CASE(tlsInitBss)
#if defined(GBL_PSP) || defined(GBL_GAMECUBE)
    GBL_TEST_SKIP("Unimplemented on PSP without `thread_local` support!");
#endif

    for(unsigned t = 0; t < GBL_TEST_THREAD_TLS_THREAD_COUNT_; ++t) {
        while(!pFixture->pTestThreads[t]->tlsInitRan);

        GBL_TEST_VERIFY(pFixture->pTestThreads[t]->tlsInitBssPass);
    }
GBL_TEST_CASE_END

GBL_TEST_CASE(tlsInitData)
#if defined(GBL_PSP) || defined(GBL_GAMECUBE)
    GBL_TEST_SKIP("Unimplemented on PSP without `thread_local` support!");
#endif

    for(unsigned t = 0; t < GBL_TEST_THREAD_TLS_THREAD_COUNT_; ++t) {
        GBL_TEST_VERIFY(pFixture->pTestThreads[t]->tlsInitDataPass);
    }
GBL_TEST_CASE_END

GBL_TEST_CASE(tlsInitAlignment)
#if defined(GBL_PSP) || defined(GBL_GAMECUBE)
    GBL_TEST_SKIP("Unimplemented on PSP without `thread_local` support!");
#endif

    for(unsigned t = 0; t < GBL_TEST_THREAD_TLS_THREAD_COUNT_; ++t) {
        GBL_TEST_VERIFY(pFixture->pTestThreads[t]->tlsInitAlignPass);
    }
GBL_TEST_CASE_END

GBL_TEST_CASE(tlsReadWrite)
#if defined(GBL_PSP) || defined(GBL_GAMECUBE)
    GBL_TEST_SKIP("Unimplemented on PSP without `thread_local` support!");
#endif

    for(unsigned t = 0; t < GBL_TEST_THREAD_TLS_THREAD_COUNT_; ++t) {
        while(!pFixture->pTestThreads[t]->tlsReadWriteRan);
        GBL_TEST_VERIFY(pFixture->pTestThreads[t]->tlsReadWritePass);
    }
GBL_TEST_CASE_END

GBL_TEST_CASE(tlsJoinThreads)
#if defined(GBL_PSP) || defined(GBL_GAMECUBE)
    GBL_TEST_SKIP("Unimplemented on PSP without `thread_local` support!");
#endif

    for(unsigned t = 0; t < GBL_TEST_THREAD_TLS_THREAD_COUNT_; ++t) {
        GBL_TEST_CALL(GblThread_join(GBL_THREAD(pFixture->pTestThreads[t])));
    }
GBL_TEST_CASE_END

// Destroying a thread
GBL_TEST_CASE(unref)
    // Allow runtime to reclaim thread's resources
    GBL_TEST_COMPARE(GBL_UNREF(pFixture->pThread1), 0);
    for(unsigned t = 0; t < GBL_TEST_THREAD_TLS_THREAD_COUNT_; ++t) {
        GBL_UNREF(pFixture->pTestThreads[t]);
    }
GBL_TEST_CASE_END

GBL_TEST_REGISTER(new,
                  start,
                  started,
                  finished,
                  join,
                  detach,
                  tlsSpawnThreads,
                  tlsInitBss,
                  tlsInitData,
                  tlsInitAlignment,
                  tlsReadWrite,
                  tlsJoinThreads,
                  unref)
