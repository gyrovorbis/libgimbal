#ifndef TEST_GIMBAL_HPP
#define TEST_GIMBAL_HPP

#include <gimbal/gimbal_types.hpp>
#include <gimbal/gimbal_context.hpp>
#include <elysian_qtest.hpp>


#define GBL_TEST_VERIFY_EXCEPTION_THROWN(expr, result)              \
    do {                                                            \
        QVERIFY_EXCEPTION_THROWN((expr), gimbal::ResultException);  \
        try {                                                       \
            (expr);                                                 \
        } catch(const gimbal::ResultException& except) {            \
            QCOMPARE(except.getResult(), result);                   \
        }                                                           \
    } while(0)

#define GBL_TEST_VERIFY_RESULT(expr)    \
    QVERIFY(GBL_RESULT_SUCCESS((expr)))

namespace gimbal::test {


class MonitorContext: public gimbal::Context {
public:

    enum class ExtOverride: uint8_t {
        LogPush,
        LogPop,
        LogWrite,
        MemAlloc,
        MemRealloc,
        MemFree,
        Count
    } ExtOverride;

    void resetCounters(void) {
        activeAllocCount_ = 0;
        errorCount_ = 0;
        warnCount_ = 0;
        logDepth_ = 0;
        memset(countersLogWrite_, 0, sizeof(countersLogWrite_));
    }

    unsigned getAciveAllocCount(void) const { return activeAllocCount_; }
    unsigned getErrorCount(void) const { return errorCount_; }
    unsigned getWarnCount(void) const { return warnCount_; }
    unsigned getLogDepth(void) const { return logDepth_; }
    //unsigned getLogCount(LogLevel level) { return logCounts_[static_cast<GBL_LOG_LEVEL>(level)]; }

    void setResultLogPush(Result result) { resultLogPush_ = result; }
    void setResultLogPop(Result result) { resultLogPop_ = result; }
    void setResultLogWrite(Result result) { resultLogWrite_ = result; }
    void setResultMemAlloc(Result result) { resultMemAlloc_ = result; }
    void setResultMemRealloc(Result result) { resultMemRealloc_ = result; }
    void setResultMemFree(Result result) { resultMemFree_ = result; }

protected:
    virtual void    logPush(const StackFrame& frame) override {
        ++counterLogPush_; ++logDepth_; resultLogPush_.checkThrow();
    }
    virtual void    logPop(const StackFrame& frame, uint32_t count) override {
        ++counterLogPop_; --logDepth_; resultLogPop_.checkThrow();
    }
    virtual void    logWrite(const StackFrame& frame, LogLevel level, const char* pFmt, va_list varArgs) override {
     //   ++counterslogWrite_[static_cast<GBL_LOG_LEVEL>(level)]; resultLogWrite_.checkThrow();
    }

    virtual void*   memAlloc(const StackFrame& frame, Size size, Size alignment, const char* pDebugInfoStr) override {
        ++counterMemAlloc_; ++activeAllocCount_; resultMemAlloc_.checkThrow(); return nullptr;
    }
    virtual void*   memRealloc(const StackFrame& frame, void* pPtr, Size newSize, Size newAlign) override {
        ++counterMemRealloc_; resultMemRealloc_.checkThrow(); return nullptr;
    }
    virtual void    memFree(const StackFrame& frame, void* pPtr) override {
        ++counterMemFree_; --activeAllocCount_; resultMemFree_.checkThrow();
    }

private:

    MonitorContext*     pCtxParent_     = nullptr;

    unsigned            activeAllocCount_                       = 0;
    unsigned            errorCount_                             = 0;
    unsigned            warnCount_                              = 0;
    unsigned            logDepth_                               = 0;

    unsigned            counterLogPush_                         = 0;
    unsigned            counterLogPop_                          = 0;
    unsigned            countersLogWrite_[GBL_LOG_LEVEL_COUNT]  = { 0 };
    unsigned            counterMemAlloc_                        = 0;
    unsigned            counterMemRealloc_                      = 0;
    unsigned            counterMemFree_                         = 0;

    Result              resultLogPush_                          = GBL_RESULT_UNIMPLEMENTED;
    Result              resultLogPop_                           = true;
    Result              resultLogWrite_                         = true;
    Result              resultMemAlloc_                         = true;
    Result              resultMemRealloc_                       = true;
    Result              resultMemFree_                          = true;
};


//error strings?
class TestSet: public elysian::UnitTestSet {
Q_OBJECT
protected:

};

}

#endif // TEST_GIMBAL_HPP
