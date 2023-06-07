#include "core/gimbal_thread_test_suite.h"
#include <gimbal/test/gimbal_test_macros.h>
#include <gimbal/core/gimbal_thread.h>
#include <gimbal/core/gimbal_atomics.h>
#include <tinycthread.h>

#define GBL_TEST_SUITE_SELF GblThreadTestSuite

#define GBL_TEST_THREAD_TYPE_               (GBL_TYPEOF(GblTestThread))
#define GBL_TEST_THREAD_(self)              (GBL_INSTANCE_CAST(self, GblTestThread))
#define GBL_TEST_THREAD_CLASS_(klass)       (GBL_CLASS_CAST(klass, GblTestThread))
#define GBL_TEST_THREAD_GET_CLASS_(self)    (GBL_INSTANCE_GET_CLASS(self, GblTestThread))

#define GBL_TEST_THREAD_TLS_VERIFIER_THREAD_COUNT_  3

GBL_CLASS_DERIVE(GblTestThread, GblThread)
    GBL_ATOMIC_INT16 runningCount;
GBL_CLASS_END

GBL_INSTANCE_DERIVE(GblTestThread, GblThread)
    struct TlsValues {
        uint16_t    uint16;
        int32_t     int32;
        double      dbl;
        char        strRef[64];
        char        charArray[64];
        char        objectName[64];
        void*       objectUserdata;
        GblObject*  objectParent;
    } input, output;

    volatile GblBool tlsInitRan;
    volatile GblBool tlsWriteRan;
    volatile GblBool tlsReadBackRan;

    volatile GblBool tlsInitDataPass;
    volatile GblBool tlsInitBssPass;
    volatile GblBool tlsInitAlignPass;

GBL_INSTANCE_END

GBL_TEST_FIXTURE {
    GblThread* pThreadMain;
    uint16_t   refCount;
    size_t     threadCount;
    uint16_t   classRefCount;
    uint16_t   instanceCount;

    GblThread* pThread1;
    volatile GblBool thread1Started;
    volatile GblBool thread1Ran;
    volatile GblBool thread1Finished;

    volatile GblBool thread2Ran;
    volatile GblBool thread2Finished;

    GblThread* pTlsThreads[GBL_TEST_THREAD_TLS_VERIFIER_THREAD_COUNT_ ];
};

typedef GblThreadTestSuite_ GblFixture;

static GBL_ATOMIC_INT16 threadActiveCount_ = 0;
#if 1
static GBL_THREAD_LOCAL                 uint16_t      tlsUint16_      = '\0';
static GBL_THREAD_LOCAL GBL_ALIGNAS(16) int32_t       tlsInt32_       = -346;
static GBL_THREAD_LOCAL                 GblStringRef* tlsStringRef_   = NULL;
static GBL_THREAD_LOCAL GBL_ALIGNAS(32) char          tlsCharArray_[] = { "abcdefghijklmnopqrstuvwxyz012345" };
static GBL_THREAD_LOCAL GBL_ALIGNAS(64) GblObject     tlsObject_;
static GBL_THREAD_LOCAL GBL_ALIGNAS(8)  double        tlsDouble_      = 12345.6789;
#endif
static GblType GblTestThread_type(void) {
    static GblType type = GBL_INVALID_TYPE;

    if(type == GBL_INVALID_TYPE) {
        type = GblType_registerStatic(GblQuark_internStringStatic("GblTestThread"),
                                      GBL_THREAD_TYPE,
                                      &(GblTypeInfo) {
                                          .classSize    = sizeof(GblTestThreadClass),
                                          .instanceSize = sizeof(GblTestThread)
                                      },
                                      GBL_TYPE_FLAGS_NONE);
    }

    return type;
}


static GBL_RESULT GblTestThread_GblThread_run_(GblThread* pThread) {
    GBL_CTX_BEGIN(NULL);
#if 1
    GblTestThread* pSelf = GBL_TEST_THREAD_(pThread);

    // check TLS BSS zeroed values
    GBL_TEST_COMPARE(tlsUint16_, '\0');
    GBL_TEST_COMPARE(tlsStringRef_, NULL);
    for(size_t b = 0; b < sizeof(GblObject); ++b)
        GBL_TEST_COMPARE(*(((uint8_t*)&tlsObject_) + 1), 0);

    pSelf->tlsInitBssPass = GBL_TRUE;

    // check TSL DATA initialized values
    GBL_TEST_COMPARE(tlsInt32_, -346);
    GBL_TEST_COMPARE(tlsCharArray_, "abcdefghijklmnopqrstuvwxyz012345");
    GBL_TEST_COMPARE(tlsDouble_, 12345.6789);

    pSelf->tlsInitDataPass = GBL_TRUE;

    // check alignment
    GBL_TEST_VERIFY(!((uintptr_t)&tlsInt32_    & 0xf));
    GBL_TEST_VERIFY(!((uintptr_t)tlsCharArray_ & 0x1f));
    GBL_TEST_VERIFY(!((uintptr_t)&tlsObject_   & 0x3f));
    GBL_TEST_VERIFY(!((uintptr_t)&tlsDouble_   & 0x7));

    pSelf->tlsInitAlignPass = GBL_TRUE;
    pSelf->tlsInitRan = GBL_TRUE;
#endif
    GBL_CTX_END();
}

static GBL_RESULT GblTestThreadClass_init_(GblClass* pClass, const void* pUd, GblContext* pCtx) {
    GBL_CTX_BEGIN(NULL);

    GBL_THREAD_CLASS(pClass)->pFnRun = GblTestThread_GblThread_run_;

    GBL_CTX_END();
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
    pFixture->instanceCount = GblType_instanceRefCount(GBL_THREAD_TYPE);
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
//    GBL_TEST_COMPARE(GblType_instanceRefCount(GBL_THREAD_TYPE), pFixture->instanceCount);
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
    GblFixture* pFixture = GblClosure_currentUserdata();
    pFixture->thread1Started = GBL_TRUE;
}

static void thread1OnFinished_(GblThread* pSelf) {
    GblFixture* pFixture = GblClosure_currentUserdata();
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
    GBL_TEST_COMPARE(GblThread_count(),
                     pFixture->thread1Finished?
                            pFixture->threadCount :
                            pFixture->threadCount + 1);
GBL_TEST_CASE_END

GBL_TEST_CASE(started)
    while(!pFixture->thread1Started);
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

static void thread2OnFinished_(GblThread* pSelf) {
    GBL_UNUSED(pSelf);
    GblFixture* pFixture = GblClosure_currentUserdata();
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

// Destroying a thread
GBL_TEST_CASE(unref)
    // Allow runtime to reclaim thread's resources
    GBL_TEST_COMPARE(GBL_UNREF(pFixture->pThread1), 0);
GBL_TEST_CASE_END


static void _(GblThread* pSelf) {
    GblFixture* pFixture = GblClosure_currentUserdata();
    pFixture->thread1Finished = GBL_TRUE;
}

GBL_TEST_CASE(tlsInit)

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

GBL_TEST_CASE_END
//spinWait
//find
//foreach

//yield
//nanosleep
//exit


//join return value
//other properties
//sigkill

GBL_TEST_REGISTER(new,
                  start,
                  started,
                  finished,
                  join,
                  detach,
                  unref)
