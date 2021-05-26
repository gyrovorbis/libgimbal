#ifndef TEST_GIMBAL_CONTEXT_HPP
#define TEST_GIMBAL_CONTEXT_HPP

#include <gimbal/gimbal_macros.h>

#pragma GBL_PRAGMA_MACRO_PUSH("GBL_ASSERT_1");
#undef GBL_ASSERT_1
#define GBL_ASSERT_1    gimbal::test::AssertMgr::assert_

#pragma GBL_PRAGMA_MACRO_PUSH("GBL_ASSERT_2");
#undef GBL_ASSERT_2
#define GBL_ASSERT_2    gimbal::test::AssertMgr::assert_

namespace gimbal::test {
    struct AssertMgr {
        static inline bool asserted = false;
        static inline std::string message = std::string();

        static void assert_(bool expr, std::string str=std::string()) {
            if(!expr) {
                asserted = true;
                message = std::move(str);
                qCritical() << "ASSERTED: " << message.c_str();
            }
        }

        static void get(bool* pAssertion, std::string* pStr) {
            *pAssertion = asserted;
            *pStr = message;
        }

        static void clear(void) {
            asserted = false;
            message.clear();
        }

    };
}


#include "test_gimbal.hpp"

#include <gimbal/gimbal_context.hpp>
#include <elysian_qtest.hpp>
#include <QObject>
#include <QDebug>
#include <QStack>



namespace gimbal::test {



enum API_EXT_CB {


};
#if 1

#endif

class Object: public elysian::UnitTestSet {
Q_OBJECT
    virtual gimbal::ObjectType expectedObjectType(void) const {
        return gimbal::ObjectType::Object;
       // return GL_OBJECT_TYPE_OBJECT;
    }
private slots:
    void checkMetaType(void) {}
    void isValid(void) {}
    //check C object compatibiity
    //check for equality/comparison with INVALID/NULL
};

/*
 *
GBL_API gblHandleConstruct(GblContext hContext,
                        GblHandle* phHandle,
                        const GblHandleCreateInfo* pInfo);

GBL_API gblHandleDestruct(GblHandle hHandle);

GBL_API gblHandleParentGet(GblHandle hHandle,
                        GblHandle* phParent);

GBL_API gblHandleParentSet(GblHandle hHandle,   //useful for run-time dependency injection
                           GblHandle hParent);

GBL_API gblHandleContext(GblHandle hHandle,
                         GblContext* phCtx);

GBL_API gblHandleUserdata(GblHandle hHandle,
                          void** ppUserdata);


// this could be returning just a the base of a custom subclass
GBL_API gblHandleLastErrorGet(GblHandle hHandle,
                            const GblApiResult** pError);


// this could theoretically construct or call into some userdata shit for more info
GBL_API gblHandleLastErrorSet(GblHandle hHandle,
                            const GblApiResult* pError);

 */

class Handle: public Object {
Q_OBJECT
private:
    virtual gimbal::ObjectType expectedObjectType(void) const {
        return gimbal::ObjectType::Handle;
        //return GL_OBJECT_TYPE_HANDLE;
    }
private slots:



    void create(void) {}
    void destroy(void) {}
    void getParent(void) {}
    void setParent(void) {}
    void getContext(void) {}
    void getUserdata(void) {}
    void getLastError(void) {}
};

#if 0
class ApiBlock {
private:
    gimbal::StackFrame stackFrame_;
    gimbal::Handle* pHandle_    = nullptr;

public:
    ApiBlock(gimbal::Handle* pHandle,
             const char* pFile = nullptr,
             const char* pFunc = nullptr,
             gimbal::Size line = 0,
             gimbal::Size col = 0):
        stackFrame_(*pHandle,
                    gimbal::Result(gimbal::Result::Success),
                    GBL_SOURCE_LOCATION(pFile, pFunc, line, col)),
        _pHandle(pHandle) {}

    gimbal::Handle* getHandle(void) const { return pHandle_; }
    gimbal::Context* getContext(void) const { return getHandle()->getContext()); }

    const gimbal::StackFrame& getFrame(void) const { return stackFrame_; }
    Result getResult(void) const { return stackFrame_.getApiResult().getResult(); }
    operator bool() const { return getResult().isSuccess(); }

    template<typename F>
    const ApiBlock& operator=(F&& funcBlock) {
        GBL_UNUSED([&]() {
            GBL_API_BEGIN_FRAME(stackFrame_.getSourceEntry().getFileName().data(),
                                stackFrame_.getSourceEntry().getFunctionName().data(),
                                stackFrame_.getSourceEntry().getLineNumber(),
                                stackFrame_.getSourceEntry().getColumn(),
                                *pHandle_,
                                &stackFrame_);
                funcBlock(&stackFrame_);
                ApiResult::checkThrow(GBL_API_RESULT());
            GBL_API_END();
        }());
        return *this;
    }

};
#endif

class ApiBlock {
protected:
    gimbal::StackFrame stackFrame_;
    std::string name_;
    gimbal::Handle* pHandle_    = nullptr;
    ContextCountersMonitor countersMonitor_;
    ContextActiveAllocMonitor activeAllocMonitor_;
    bool asserted_ = false;
    std::string assertMsg_;

public:
    ApiBlock(gimbal::Handle* pHandle,
             std::string name    = "unnamed",
             const char* pFile   = nullptr,
             const char* pFunc   = nullptr,
             gimbal::Size line   = 0,
             gimbal::Size column = 0):
        stackFrame_(*pHandle,
                    gimbal::Result(gimbal::Result::Success),
                    GBL_SOURCE_LOCATION(pFile, pFunc, line, column)),
        name_(std::move(name)),
        pHandle_(pHandle),
        countersMonitor_(getContext()),
        activeAllocMonitor_(getContext())
    {

    }

    template<typename F>
    const ApiBlock& operator=(F&& funcBlock) {
        GBL_UNUSED([&]() {
            GBL_API_BEGIN_FRAME(stackFrame_.getSourceEntry().getFileName().data(),
                                stackFrame_.getSourceEntry().getFunctionName().data(),
                                stackFrame_.getSourceEntry().getLineNumber(),
                                stackFrame_.getSourceEntry().getColumn(),
                                *pHandle_,
                                &stackFrame_);
            GBL_API_TRY {
                countersMonitor_.begin();
                activeAllocMonitor_.begin();
                AssertMgr::clear();
                getHandle()->clearLastCallRecord();
                funcBlock(&stackFrame_);
                AssertMgr::get(&asserted_, &assertMsg_);
                countersMonitor_.end();
                activeAllocMonitor_.end();
            } GBL_API_CATCH();
            GBL_API_END();
        }());
        return *this;
    }

    operator bool() const {
        return getRecord().getResult().isSuccess() && !didAssert();
    }

    std::string_view getName(void) const { return name_; }
    gimbal::Handle* getHandle(void) const { return pHandle_; }
    MonitorableContext* getContext(void) const { return dynamic_cast<MonitorableContext*>(pHandle_->getContext()); }
    const ContextCountersMonitor& getCountersMonitor(void) const { return countersMonitor_; }

    const gimbal::StackFrame& getFrame(void) const { return stackFrame_; }
    const gimbal::CallRecord& getRecord(void) const { return getFrame().getCallRecord(); }
    //Result getResult(void) const { return getApiCallResult().getResult(); }

    ContextCounters getCountersDelta(void) const { return getCountersMonitor().getEndDelta(); }
    size_t getActiveAllocCount(void) const { return activeAllocMonitor_.getEndDelta(); }
    decltype(auto) getLastCallRecord(void) const { return getHandle()->getLastCallRecord(); }

    bool didAssert(void) const { return asserted_; }
    std::string_view getAssertMessage(void) const { return assertMsg_; }
};




#define GBL_API_BLOCK(ctx_, name) \
    ApiBlock(ctx_, name, GBL_SOURCE_FILE, GBL_SOURCE_FUNCTION, GBL_SOURCE_LINE, GBL_SOURCE_COLUMN) = [&](GBL_MAYBE_UNUSED GBL_API_FRAME_DECLARE)


/* 1) override default log function(s)
 * 2)
 *
 *
 */


/* TO TEST
 * 1) version
 * 2) throwing C++ exceptions from C return codes
 * 3) catching C++ exceptions as C return codes
 * 4) results result stringification + getLastError
 * 5) subclass context with validator
 * 6) test API macros for all overloads
 * 7) test C/++ handle type conversion/interop compat
 * 8) inherit from and perform base class tests on Handle/Object
 *
 *
 * OVERALL TODUZ
 * 1) finish gimbal_config.h as CMake generated file for inclusion
 * 2) Implement base/default/implicit parent std C EXT back-end
 * 3) map error-handling compiler flags to CMake options
 * 4) GblString + C++ version
 * 5) GblVariant + metatypes
 * 6) Declare C enums as table structure that can be used to autogen C, C++, and stringified values
 * 7) GblContainer
 * 8) Query/extract build info
 */
// test exception system (both sides)



//NEED TO VERIFY SOURCE FILE LOCATION TRACKING
class Context: public Handle {
Q_OBJECT

private:

    class StandardContext: public gimbal::Context {
    protected:

        virtual void*   memAlloc(const StackFrame& frame, Size size, Size alignment, const char* pDebugInfoStr) { return malloc(size); }
        virtual void*   memRealloc(const StackFrame& frame, void* pPtr, Size newSize, Size newAlign) { return realloc(pPtr, newSize); }
        virtual void    memFree(const StackFrame& frame, void* pPtr) { free(pPtr); }
        virtual void    logWrite(const StackFrame& frame, LogLevel level, const char* pFmt, va_list varArgs) {
            QMessageLogger logger;

#if 0
            frame.getSourceEntry().getFilePath().data(),
                                  frame.getSourceEntry().getLineNumber(),
                                  frame.getSourceEntry().getFunctionName().data());
#endif

           QString buffer = QString::vasprintf(pFmt, varArgs);

            switch(level.getValue()) {
            case LogLevel::Warning:
                logger.warning("%s", Q_CSTR(buffer));
                break;
            case LogLevel::Error:
                logger.critical("%s", Q_CSTR(buffer));
                break;
            case LogLevel::Debug:
                logger.debug("%s", Q_CSTR(buffer));
                break;
            case LogLevel::Verbose:
            case LogLevel::Info:
            default:
                logger.info("%s", Q_CSTR(buffer));
                break;
            }
        }
    };

    MonitorableContext* pCtx_ = nullptr;
    char* ppPtr[3] = { nullptr };

    virtual gimbal::ObjectType expectedObjectType(void) const {
        return gimbal::ObjectType::Context;
        //return GL_OBJECT_TYPE_CONTEXT;
    }
protected:
    GblContext gblCtx(void) const { return *pCtx_; }
    MonitorableContext& ctx(void) const { return *pCtx_; }
    MonitorableContext* pCtx(void) const { return pCtx_; }
#if 0
    void assert_(bool value, QString string) {
        if(!value) {
            qDebug() << "Asserting: " << string;
        }
    }
#endif

    void verifyBlock(const ApiBlock& block, const ConfigOptions& config, Result result, QString message) {
        const auto resultType = static_cast<uint8_t>(result.getType());
        QCOMPARE(block.getRecord().getResult(), result);
        QCOMPARE(block.getRecord().getMessage().data(), message);

        QCOMPARE(block.getCountersDelta().getLog(config.logLevels[resultType]), config.logEnabled[resultType]? 1 : 0);

        if(config.recordEnabled[resultType]) {
            QCOMPARE(block.getLastCallRecord().getResult(), result);
            QCOMPARE(block.getLastCallRecord().getMessage().data(), message);
        } else {
            QVERIFY(block.getLastCallRecord().getResult().isUnknown());
            QCOMPARE(block.getLastCallRecord().getMessage().data(), "");
        }

        if(config.assertEnabled[resultType]) {
            QVERIFY(block.didAssert());
            QCOMPARE(block.getAssertMessage().data(), message);
        } else QVERIFY(!block.didAssert());
    }


private slots:

    void version(void) {
        qDebug() << gimbal::Context::getVersion().toString().c_str();
    }

    void create(void) {
        try {
            auto* pCtx = new StandardContext;
            pCtx_ = new MonitorableContext(pCtx);

            qDebug() << QString(std::string(Result(GBL_RESULT_ERROR_INVALID_ARG).toString()).c_str());
            qDebug() << QString(std::string(Result(GBL_RESULT_TRUNCATED).toString()).c_str());
            qDebug() << QString(std::string(Result(GBL_RESULT_SUCCESS).toString()).c_str());
            qDebug() << QString(std::string(Result(static_cast<GBL_RESULT>(INTERNAL::Result::ErrorTypeMismatch)).toString()).c_str());

            qDebug() << std::string(Result(Result::InvalidPointer).toString()).c_str();

        } catch(const std::exception& ex) {
            QFAIL(ex.what());
        }


    }

    // stack monitors!?!?

    // === API TEST WHICH CAN TAKE A HANDLE ====

    void begin(void) {
        auto test = GBL_API_BLOCK(pCtx(), "BEGIN_0") {
            {
                GBL_API_BEGIN(gblCtx());
            }
        };
        QVERIFY(test);
        QCOMPARE(test.getCountersDelta().getLogDepth(), 0);

        test = GBL_API_BLOCK(pCtx(), "BEGIN_1") {
            {
                GBL_API_BEGIN(gblCtx(), "Single");
            }
        };
        QVERIFY(test);
        QCOMPARE(test.getCountersDelta().getLogDepth(), 1);
        QCOMPARE(test.getCountersDelta().getLog(LogLevel::Verbose), 1);

        test = GBL_API_BLOCK(pCtx_, "BEGIN_N") {
            {
                GBL_API_BEGIN(gblCtx(), "%s - %d", "Variadic", 33);
            }
        };
        QVERIFY(test);
        QCOMPARE(test.getCountersDelta().getLogDepth(), 1);
        QCOMPARE(test.getCountersDelta().getLog(LogLevel::Verbose), 1);
    }

    void end(void) {

        auto test = GBL_API_BLOCK(pCtx(), "END") {
            {
                GBL_API_BEGIN(gblCtx(), "Layer1");
                GBL_API_END();
            }
        };
        QVERIFY(test);
        QCOMPARE(test.getCountersDelta().getLogDepth(), 0); //make sure log depth is respected
    }

    void logDebug(void) {
        auto test = GBL_API_BLOCK(pCtx_, "DEBUG_1") {
            GBL_API_DEBUG("Standard");
        };
        QVERIFY(test);
        QCOMPARE(test.getCountersDelta().getLog(LogLevel::Debug), 1);

        test = GBL_API_BLOCK(pCtx_, "DEBUG_N") {
            GBL_API_DEBUG("Variadic - %s %d %c", "lul", 45, 'c');
        };
        QVERIFY(test);
        QCOMPARE(test.getCountersDelta().getLog(LogLevel::Debug), 1);
    }

    void logVerbose(void) {
        auto test = GBL_API_BLOCK(pCtx_, "VERBOSE_1") {
            GBL_API_VERBOSE("Standard");
        };
        QVERIFY(test);
        QCOMPARE(test.getCountersDelta().getLog(LogLevel::Verbose), 1);

        test = GBL_API_BLOCK(pCtx_, "VERBOSE_N") {
            GBL_API_VERBOSE("Variadic - %s %d %c", "lul", 45, 'c');
        };
        QVERIFY(test);
        QCOMPARE(test.getCountersDelta().getLog(LogLevel::Verbose), 1);

    }

    void logInfo(void) {
        auto test = GBL_API_BLOCK(pCtx_, "INFO_1") {
            GBL_API_INFO("Standard");
        };
        QVERIFY(test);
        QCOMPARE(test.getCountersDelta().getLog(LogLevel::Info), 1);

        test = GBL_API_BLOCK(pCtx_, "INFO_N") {
            GBL_API_INFO("Variadic - %s %d %c", "lul", 45, 'c');
        };
        QVERIFY(test);
        QCOMPARE(test.getCountersDelta().getLog(LogLevel::Info), 1);
    }

    void logWarn(void) {
        auto test = GBL_API_BLOCK(pCtx_, "WARN_1") {
            GBL_API_WARN("Standard");
        };
        QVERIFY(test);
        QCOMPARE(test.getCountersDelta().getLog(LogLevel::Warning), 1);

        test = GBL_API_BLOCK(pCtx_, "WARN_N") {
            GBL_API_WARN("Variadic - %s %d %c", "lul", 45, 'c');
        };
        QVERIFY(test);
        QCOMPARE(test.getCountersDelta().getLog(LogLevel::Warning), 1);
    }

    void logError(void) {
        auto test = GBL_API_BLOCK(pCtx_, "ERROR_1") {
            GBL_API_ERROR("Standard");
        };
        QVERIFY(test);
        QCOMPARE(test.getCountersDelta().getLog(LogLevel::Error), 1);

        test = GBL_API_BLOCK(pCtx_, "ERROR_N") {
            GBL_API_ERROR("Variadic - %s %d %c", "lul", 45, 'c');
        };
        QVERIFY(test);
        QCOMPARE(test.getCountersDelta().getLog(LogLevel::Error), 1);
    }

    void logPush(void) {
        auto test = GBL_API_BLOCK(pCtx_, "PUSH_0") {
            GBL_API_PUSH();
        };
        QVERIFY(test);
        QCOMPARE(test.getCountersDelta().getLogDepth(), 1);
        QCOMPARE(test.getCountersDelta().getExt(ContextCounters::ApiExtCall::LogPush), 1);

        test = GBL_API_BLOCK(pCtx_, "PUSH_1") {
            GBL_API_PUSH_VERBOSE("Single Push");
        };
        QVERIFY(test);
        QCOMPARE(test.getCountersDelta().getLogDepth(), 1);
        QCOMPARE(test.getCountersDelta().getLog(LogLevel::Verbose), 1);
        QCOMPARE(test.getCountersDelta().getExt(ContextCounters::ApiExtCall::LogPush), 1);

        test = GBL_API_BLOCK(pCtx_, "PUSH_N") {
            GBL_API_PUSH_VERBOSE("%s %d %c", "Variadic Push", -33, 'q');
        };
        QVERIFY(test);
        QCOMPARE(test.getCountersDelta().getLogDepth(), 1);
        QCOMPARE(test.getCountersDelta().getLog(LogLevel::Verbose), 1);
        QCOMPARE(test.getCountersDelta().getExt(ContextCounters::ApiExtCall::LogPush), 1);

    }

    void logPop(void) {
        auto test = GBL_API_BLOCK(pCtx_, "POP_BALANCED") {
            GBL_API_PUSH();
            GBL_API_POP(1);
        };
        QVERIFY(test);
        QCOMPARE(test.getCountersDelta().getLogDepth(), 0);
        QCOMPARE(test.getCountersDelta().getExt(ContextCounters::ApiExtCall::LogPop), 1);


        test = GBL_API_BLOCK(pCtx_, "POP_FAIL") {
            GBL_API_PUSH_VERBOSE("Trolo"); //GCC FUCKED
            GBL_API_PUSH();
            GBL_API_PUSH();
            GBL_API_POP(2);
            // GBL_API_POP(4); CONFIRM THAT THIS IS AN ERROR!
        };
        QVERIFY(test);
        QCOMPARE(test.getCountersDelta().getLogDepth(), 1);
        QCOMPARE(test.getCountersDelta().getExt(ContextCounters::ApiExtCall::LogPop), 1);

    }

    void memAlloc(void) {
        auto test = GBL_API_BLOCK(pCtx_, "MALLOC_1") {
            this->ppPtr[0] = (char*)GBL_API_MALLOC(15, 1, "Test");
            QVERIFY2(ppPtr[0], "Malloc returned nullptr!");
            QVERIFY(strncpy(ppPtr[0], "LUL", 15));
        };
        QCOMPARE(test.getCountersDelta().getExt(ContextCounters::ApiExtCall::MemAlloc), 1);
        QCOMPARE(test.getActiveAllocCount(), 1);

        GBL_API_BLOCK(pCtx_, "MALLOC_2") {
            this->ppPtr[1] = (char*)GBL_API_MALLOC(15, 1, "Test");
            QVERIFY2(ppPtr[1], "Aligned malloc returned nullptr!");
            QVERIFY(strncpy(ppPtr[1], "LUL", 15));
        };
        QCOMPARE(test.getCountersDelta().getExt(ContextCounters::ApiExtCall::MemAlloc), 1);
        QCOMPARE(test.getActiveAllocCount(), 1);

        GBL_API_BLOCK(pCtx_, "MALLOC_3") {
             this->ppPtr[2] = (char*)GBL_API_MALLOC(34, 1, "Alloc Info!");
             QVERIFY2(ppPtr[2], "Aligned malloc with debug info returned nullptr!");
        };
        QCOMPARE(test.getCountersDelta().getExt(ContextCounters::ApiExtCall::MemAlloc), 1);
        QCOMPARE(test.getActiveAllocCount(), 1);

        QCOMPARE(ctx().getAllocTracker().getActiveAllocationCount(), 3);
    }

    void memRealloc(void) {
        auto test = GBL_API_BLOCK(pCtx_, "REALLOC_2") {
            this->ppPtr[0] = (char*)GBL_API_REALLOC(this->ppPtr[0], 37);
            QVERIFY2(this->ppPtr[0], "Realloc returned nullptr!");
            QVERIFY2(strncmp(this->ppPtr[0], "LUL", 37) == 0, "Realloc lost its data!");
        };
        QCOMPARE(test.getCountersDelta().getExt(ContextCounters::ApiExtCall::MemRealloc), 1);

        test = GBL_API_BLOCK(pCtx_, "REALLOC_3") {
            this->ppPtr[1] = (char*)GBL_API_REALLOC(this->ppPtr[1], 64, 8);
            QVERIFY2(this->ppPtr[1], "Realloc returned nullptr!");
            QVERIFY2(strncmp(this->ppPtr[1], "LUL", 64) == 0, "Realloc lost its data!");
        };
        QCOMPARE(test.getCountersDelta().getExt(ContextCounters::ApiExtCall::MemRealloc), 1);

        QCOMPARE(ctx().getAllocTracker().getActiveAllocationCount(), 3);
    }

    void memFree(void) {
        auto test = GBL_API_BLOCK(pCtx_, "FREE") {
            GBL_API_FREE(this->ppPtr[0]);
        };
        QCOMPARE(test.getCountersDelta().getExt(ContextCounters::ApiExtCall::MemFree), 1);
        QCOMPARE(test.getActiveAllocCount(), -1);
        QCOMPARE(ctx().getAllocTracker().getActiveAllocationCount(), 2);

        test = GBL_API_BLOCK(pCtx_, "FREE_2") {
            GBL_API_FREE(this->ppPtr[1]);
            GBL_API_FREE(this->ppPtr[2]);
        };
        QCOMPARE(test.getCountersDelta().getExt(ContextCounters::ApiExtCall::MemFree), 2);
        QCOMPARE(test.getActiveAllocCount(), -2);
        QCOMPARE(ctx().getAllocTracker().getActiveAllocationCount(), 0);
    }

    void stlPmr(void) {
        auto test = GBL_API_BLOCK(pCtx(), "PMR_VECTOR") {
            std::pmr::vector<std::string> strings{1, pCtx()};
            strings.emplace_back("324324324333333333");
            strings.emplace_back("23432453453453452345234523452");
        };
        QVERIFY(test);
        QVERIFY(test.getCountersDelta().getExt(ContextCounters::ApiExtCall::MemAlloc) >= 2);
        QVERIFY(test.getCountersDelta().getExt(ContextCounters::ApiExtCall::MemFree) >= 2);
        QCOMPARE(test.getActiveAllocCount(), 0);

        test = GBL_API_BLOCK(pCtx(), "PMR_STRING") {
            std::pmr::string str("LOLOSDFSDFSDFSDFfLOL", pCtx());
            str += "HEEAAAAAAAAAAAAAAAAAAAAAAP";
        };
        QVERIFY(test);
        QVERIFY(test.getCountersDelta().getExt(ContextCounters::ApiExtCall::MemAlloc) >= 2);
        QVERIFY(test.getCountersDelta().getExt(ContextCounters::ApiExtCall::MemFree) >= 2);
        QCOMPARE(test.getActiveAllocCount(), 0);

        QCOMPARE(ctx().getAllocTracker().getActiveAllocationCount(), 0);
    }

    void recordSet(void) {
#define RUN_TEST_SET(options)                                                           \
        [&](const ConfigOptions& options) {                                             \
            auto test = GBL_API_BLOCK(pCtx(), "RESULT_SET_1") {                         \
                GBL_API_RECORD_SET(GBL_RESULT_UNKNOWN);                                 \
            };                                                                          \
            verifyBlock(test, options, Result::Unknown, "Unknown");                     \
                                                                                        \
            test = GBL_API_BLOCK(pCtx_, "RESULT_SET_2") {                               \
                GBL_API_RECORD_SET(GBL_RESULT_SUCCESS, "WORX!");                        \
            };                                                                          \
            verifyBlock(test, options, Result::Success, "WORX!");                       \
                                                                                        \
            test = GBL_API_BLOCK(pCtx_, "RESULT_SET_N") {                               \
                GBL_API_RECORD_SET(GBL_RESULT_TRUNCATED, "Reested to %s.", "fuck");     \
            };                                                                          \
            verifyBlock(test, options, Result::Truncated, "Reested to fuck.");          \
                                                                                        \
            test = GBL_API_BLOCK(pCtx_, "RESULT_SET_N") {                               \
                GBL_API_RECORD_SET(GBL_RESULT_ERROR_INTERNAL, "Error: %s",              \
                                    Result(Result::ErrorInternal).toString().data());   \
            };                                                                          \
            verifyBlock(test, options, Result::ErrorInternal, "Error: Internal Error"); \
        }((options))

        ConfigOptions opts = GBL_CONFIG_OPTIONS_DECL();
        RUN_TEST_SET(opts);

#pragma GBL_PRAGMA_MACRO_PUSH("GBL_CONFIG_LOG_PARTIAL_ENABLED");
#undef GBL_CONFIG_LOG_PARTIAL_ENABLED
#define GBL_CONFIG_LOG_PARTIAL_ENABLED    0

#pragma GBL_PRAGMA_MACRO_PUSH("GBL_CONFIG_LOG_ERROR_LEVEL");
#undef GBL_CONFIG_LOG_ERROR_LEVEL
#define GBL_CONFIG_LOG_ERROR_LEVEL    GBL_LOG_LEVEL_INFO

#pragma GBL_PRAGMA_MACRO_PUSH("GBL_CONFIG_ASSERT_UNKNOWN_ENABLED");
#undef GBL_CONFIG_ASSERT_UNKNOWN_ENABLED
#define GBL_CONFIG_ASSERT_UNKNOWN_ENABLED    1

#pragma GBL_PRAGMA_MACRO_PUSH("GBL_CONFIG_LAST_CALL_RECORD_ERROR_ENABLED");
#undef GBL_CONFIG_LAST_CALL_RECORD_ERROR_ENABLED
#define GBL_CONFIG_LAST_CALL_RECORD_ERROR_ENABLED    0

        opts = GBL_CONFIG_OPTIONS_DECL();
        RUN_TEST_SET(opts);

#pragma GBL_PRAGMA_MACRO_PUSH("GBL_CONFIG_LOG_SUCCESS_ENABLED");
#undef GBL_CONFIG_LOG_SUCCESS_ENABLED
#define GBL_CONFIG_LOG_SUCCESS_ENABLED    1

#pragma GBL_PRAGMA_MACRO_PUSH("GBL_CONFIG_LOG_SUCCESS_LEVEL");
#undef GBL_CONFIG_LOG_SUCCESS_LEVEL
#define GBL_CONFIG_LOG_SUCCESS_LEVEL    GBL_LOG_LEVEL_ERROR

#pragma GBL_PRAGMA_MACRO_PUSH("GBL_CONFIG_ASSERT_PARTIAL_ENABLED");
#undef GBL_CONFIG_ASSERT_PARTIAL_ENABLED
#define GBL_CONFIG_ASSERT_PARTIAL_ENABLED    1

#pragma GBL_PRAGMA_MACRO_PUSH("GBL_CONFIG_LAST_CALL_RECORD_SUCCESS_ENABLED");
#undef GBL_CONFIG_LAST_CALL_RECORD_SUCCESS_ENABLED
#define GBL_CONFIG_LAST_CALL_RECORD_SUCCESS_ENABLED    0

        opts = GBL_CONFIG_OPTIONS_DECL();
        RUN_TEST_SET(opts);

#pragma GBL_PRAGMA_MACRO_POP("GBL_CONFIG_LOG_PARTIAL_ENABLED");
#pragma GBL_PRAGMA_MACRO_POP("GBL_CONFIG_LOG_ERROR_LEVEL");
#pragma GBL_PRAGMA_MACRO_POP("GBL_CONFIG_ASSERT_UNKNOWN_ENABLED");
#pragma GBL_PRAGMA_MACRO_POP("GBL_CONFIG_LAST_CALL_RECORD_ERROR_ENABLED");
#pragma GBL_PRAGMA_MACRO_POP("GBL_CONFIG_LOG_SUCCESS_ENABLED");
#pragma GBL_PRAGMA_MACRO_POP("GBL_CONFIG_LOG_SUCCESS_LEVEL");
#pragma GBL_PRAGMA_MACRO_POP("GBL_CONFIG_ASSERT_PARTIAL_ENABLED");
#pragma GBL_PRAGMA_MACRO_POP("GBL_CONFIG_LAST_CALL_RECORD_SUCCESS_ENABLED");
    }

    void verify(void) {
        auto test = GBL_API_BLOCK(pCtx(), "VERIFY_2_SUCCESS") {
            GBL_API_VERIFY(1 < 3, Result(Result::ErrorUnhandledException));
            GBL_API_END();
        };
        QVERIFY(test);

        test = GBL_API_BLOCK(pCtx(), "VERIFY_3_ERROR") {
            GBL_API_VERIFY(false == true, Result(Result::ErrorTypeMismatch), "Impossibru!");
            GBL_API_END();
        };
        verifyBlock(test, GBL_CONFIG_OPTIONS_DECL(), GBL_RESULT_ERROR_TYPE_MISMATCH, "Impossibru!");

        test = GBL_API_BLOCK(pCtx(), "VERIFY_4_WARN") {
            GBL_API_VERIFY(sizeof(int) < sizeof(char), GBL_RESULT_TRUNCATED, "Truncated: %d", -12);
            GBL_API_END();
        };
        verifyBlock(test, GBL_CONFIG_OPTIONS_DECL(), Result::Truncated, "Truncated: -12");
    }

    void verifyExpression(void) {
        auto test = GBL_API_BLOCK(pCtx(), "VERIFY_EXPR_1_ERROR") {
            GBL_API_VERIFY_EXPRESSION(3 < 1);
            GBL_API_END();
        };
        verifyBlock(test, GBL_CONFIG_OPTIONS_DECL(), Result::InvalidExpression, "Invalid Expression: 3 < 1");

        test = GBL_API_BLOCK(pCtx(), "VERIFY_EXPR_2_ERROR") {
            GBL_API_VERIFY_EXPRESSION(sizeof(int) < sizeof(char), "Fucked");
            GBL_API_END();
        };
        verifyBlock(test, GBL_CONFIG_OPTIONS_DECL(), Result::InvalidExpression, "Fucked");

        test = GBL_API_BLOCK(pCtx(), "VERIFY_EXPR_N_SUCCESS") {
            GBL_API_VERIFY_EXPRESSION(sizeof(int) > sizeof(char), "Random Shit: %d, %s, %c, %f", -12, "lulz", 'a', 12.2f);
            GBL_API_END();
        };
        QVERIFY(test);
    }

    void verifyHandle(void) {
        auto test = GBL_API_BLOCK(pCtx(), "VERIFY_HANDLE_1_FAIL") {
            GBL_API_VERIFY_HANDLE(GBL_HANDLE_INVALID);
            GBL_API_END();
        };
        verifyBlock(test, GBL_CONFIG_OPTIONS_DECL(), Result::InvalidHandle, "Invalid Handle");

        test = GBL_API_BLOCK(pCtx(), "VERIFY_HANDLE_2_PASS") {
            GBL_API_VERIFY_HANDLE(pCtx(), "Valid as fuck");
            GBL_API_END();
        };
        QVERIFY(test);

        test = GBL_API_BLOCK(pCtx(), "VERIFY_HANDLE_N_FAIL") {
            GBL_API_VERIFY_HANDLE(GBL_HANDLE_INVALID, "Assdouchery %s %d %c", "lul", -33, 'b');
            GBL_API_END();
        };
        verifyBlock(test, GBL_CONFIG_OPTIONS_DECL(), GBL_RESULT_ERROR_INVALID_HANDLE, "Assdouchery lul -33 b");
    }

    void verifyPointer(void) {
        auto test = GBL_API_BLOCK(pCtx(), "VERIFY_POINTER_1_FAIL") {
            GBL_API_VERIFY_POINTER(nullptr);
            GBL_API_END();
        };
        verifyBlock(test, GBL_CONFIG_OPTIONS_DECL(), Result::InvalidPointer, "Invalid Pointer");

        test = GBL_API_BLOCK(pCtx(), "VERIFY_POINTER_2_PASS") {
            GBL_API_VERIFY_POINTER(pCtx(), "Valid as fuck");
            GBL_API_END();
        };
        QVERIFY(test);

        test = GBL_API_BLOCK(pCtx(), "VERIFY_POINTER_N_FAIL") {
            GBL_API_VERIFY_POINTER(nullptr, "Assdouchery %s %d %c", "lul", -33, 'b');
            GBL_API_END();
        };
        verifyBlock(test, GBL_CONFIG_OPTIONS_DECL(), GBL_RESULT_ERROR_INVALID_POINTER, "Assdouchery lul -33 b");
    }

    void verifyArg(void) {
        auto test = GBL_API_BLOCK(pCtx(), "VERIFY_ARG_1_FAIL") {
            int arg1 = 3; int arg2 = 44;
            GBL_API_VERIFY_ARG(arg1 != 0);
            GBL_API_VERIFY_ARG(arg2 > 50);
            GBL_API_END();
        };
        verifyBlock(test, GBL_CONFIG_OPTIONS_DECL(), Result::InvalidArg, "Invalid Arg: arg2 > 50");

        test = GBL_API_BLOCK(pCtx(), "VERIFY_ARG_2_PASS") {
            float argIn = -33.0f;
            GBL_API_VERIFY_ARG(fabs(argIn) > 30.0f, "Your argument sucks!");
            GBL_API_END();
        };
        QVERIFY(test);

        test = GBL_API_BLOCK(pCtx(), "VERIFY_ARG_N_FAIL") {
            const char* pName = "Fuckwhisp";
            GBL_API_VERIFY_ARG(strcmp(pName, "DickWheeze") == 0, "Assdouchery %s %d %c", "lul", -33, 'b');
            GBL_API_END();
        };
        verifyBlock(test, GBL_CONFIG_OPTIONS_DECL(), GBL_RESULT_ERROR_INVALID_ARG, "Assdouchery lul -33 b");
    }

    void cErrno(void) {
        //errno checking/reporting
#if 0

#ifdef GBL_CONFIG_ERRNO_CHECKS
#define     GBL_API_ERRNO_CLEAR()   errno = 0
#else
#define     GBL_API_ERRNO_CLEAR()
#endif

#ifdef GBL_CONFIG_ERRNO_CHECKS
#   define GBL_API_PERROR(...)                      \
    do {                                            \
        if(errno) GBL_UNLIKELY {                    \
            const GBL_RESULT code =                 \
                GBL_ERRNO_RESULT(errno);            \
            GBL_API_VERIFY(                         \
                GBL_RESULT_SUCCESS(code),           \
                code);                              \
        }                                           \
    } while(0)
#else
#   define GBL_API_PERROR(...)
#endif

GBL_MAYBE_UNUSED GBL_INLINE GBL_API GBL_ERRNO_RESULT(int ernum) {
    switch(ernum) {
    case 0:     return GBL_RESULT_SUCCESS;
    default:    return GBL_RESULT_ERROR;
    }
}

#endif
    }

    void call(void) {
        auto test = GBL_API_BLOCK(pCtx(), "CALL_1_FAIL") {
            GBL_API_CALL(gblContextVersion(nullptr));
            GBL_API_END();
        };
        verifyBlock(test, GBL_CONFIG_OPTIONS_DECL(), Result::InvalidPointer, "Call[gblContextVersion(nullptr)] -> Result[Invalid Pointer]");

        test = GBL_API_BLOCK(pCtx(), "CALL_2_PASS") {
            GblVersion version;
            GBL_API_CALL(gblContextVersion(&version),
                         "Getting the goddamn version!");
            GBL_API_END();
        };
        QVERIFY(test);

        test = GBL_API_BLOCK(pCtx(), "CALL_N_FAIL") {
            GBL_API_CALL(gblHandleUserdata(GBL_API_HANDLE(), nullptr),
                         "Variadicery %s %d %c %.2f", "lul", -33, 'b', 22.43f);
            GBL_API_END();
        };
        verifyBlock(test, GBL_CONFIG_OPTIONS_DECL(), GBL_RESULT_ERROR_INVALID_POINTER, "Variadicery lul -33 b 22.43");


        //GBL_API_CALL();
        // try with no error
        // try with partial, error, etc
        // try multiple levels of chaining
        // check return type when back at lowest-level
        // check stack and lastError to ensure it's the inner error
    }


    void cppExceptions(void) {
        // try to throw success shit
        // try to throw failure shit
        // try to throw partial shit
        // confirm it converts correctly to error code
        // confirm what()'s string is right
        // make sure C++ exceptions are caught and propagated as GBL_UNHANDLED_EXCEPTION type with correct error and shit!
        // confirm the try/catch shit works correctly
    }

    void destroy(void) {
        try {

            delete pCtx_;
        } catch(const std::exception& ex) {
            QFAIL(ex.what());
        }
    }

};

#pragma GBL_PRAGMA_MACRO_POP("GBL_ASSERT_1");
#pragma GBL_PRAGMA_MACRO_POP("GBL_ASSERT_2");

}


#endif // TEST_GIMBAL_CONTEXT_HPP
