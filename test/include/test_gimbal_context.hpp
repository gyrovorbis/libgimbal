#ifndef TEST_GIMBAL_CONTEXT_HPP
#define TEST_GIMBAL_CONTEXT_HPP

#include <gimbal/gimbal_context.hpp>
#include <elysian_qtest.hpp>
#include <QObject>
#include <QDebug>

namespace gimbal::test {



enum API_EXT_CB {


};
#if 1
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

class ApiBlock {
protected:
    gimbal::StackFrame stackFrame_;
    std::string name_;
    Context* pCtx_ = nullptr;
public:
    ApiBlock(gimbal::Context* pCtx,
             std::string name    = "unnamed",
             const char* pFile   = nullptr,
             const char* pFunc   = nullptr,
             gimbal::Size line   = 0,
             gimbal::Size column = 0):
        stackFrame_(*pCtx,
                    gimbal::Result(gimbal::Result::Success),
                    GBL_SOURCE_LOCATION(pFile, pFunc, line, column)),
        name_(std::move(name)),
        pCtx_(pCtx)
    {

    }

    template<typename F>
    const ApiBlock& operator=(F&& funcBlock) {
        GBL_UNUSED([&]() {
            GBL_API_BEGIN_FRAME(stackFrame_.getSourceEntry().getFileName().data(),
                                stackFrame_.getSourceEntry().getFunctionName().data(),
                                stackFrame_.getSourceEntry().getLineNumber(),
                                stackFrame_.getSourceEntry().getColumn(),
                                *pCtx_,
                                &stackFrame_);
            GBL_API_TRY {
                funcBlock(&stackFrame_);
            } GBL_API_CATCH();
            GBL_API_END();
        }());
        return *this;
    }

    std::string_view getName(void) const { return name_; }
    const gimbal::StackFrame& getFrame(void) const { return stackFrame_; }
    Result getResult(void) const { return stackFrame_.getApiResult().getResult(); }

    operator bool() const {
        return getResult().isSuccess();
    }
  //  bool expectLog, expectException, expectStackDepth
    // expectMallocCount

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
class Context: public Handle {
Q_OBJECT

private:
    gimbal::Context* pCtx_ = nullptr;

    virtual gimbal::ObjectType expectedObjectType(void) const {
        return gimbal::ObjectType::Context;
        //return GL_OBJECT_TYPE_CONTEXT;
    }
protected:
    GblContext ctx(void) {
        return *pCtx_;
    }
private slots:

    void version(void) {
        qDebug() << gimbal::Context::getVersion().toString().c_str();
    }



    void create(void) {
        try {
            pCtx_ = new gimbal::Context;

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
        GBL_API_BLOCK(pCtx_, "BEGIN_1") {
            {
            GBL_API_BEGIN(ctx(), "Single");
            }
        };


        auto test = GBL_API_BLOCK(pCtx_, "BEGIN_N") {
            {
            GBL_API_BEGIN(ctx(), "%s - %d", "Variadic", 33);
            }
        };
    }

    void end(void) { }

    void logDebug(void) {
        GBL_API_BLOCK(pCtx_, "DEBUG_1") {
            GBL_API_DEBUG("Standard");
        };
        GBL_API_BLOCK(pCtx_, "DEBUG_N") {
            GBL_API_DEBUG("Variadic - %s %d %c", "lul", 45, 'c');
        };
    }

    void logVerbose(void) {
        GBL_API_BLOCK(pCtx_, "VERBOSE_1") {
            GBL_API_VERBOSE("Standard");
        };
        GBL_API_BLOCK(pCtx_, "VERBOSE_N") {
            GBL_API_VERBOSE("Variadic - %s %d %c", "lul", 45, 'c');
        };

    }

    void logInfo(void) {
        GBL_API_BLOCK(pCtx_, "INFO_1") {
            GBL_API_INFO("Standard");
        };
        GBL_API_BLOCK(pCtx_, "INFO_N") {
            GBL_API_INFO("Variadic - %s %d %c", "lul", 45, 'c');
        };
    }

    void logWarn(void) {
        GBL_API_BLOCK(pCtx_, "WARN_1") {
            GBL_API_WARN("Standard");
        };
        GBL_API_BLOCK(pCtx_, "WARN_N") {
            GBL_API_WARN("Variadic - %s %d %c", "lul", 45, 'c');
        };
    }

    void logError(void) {
        GBL_API_BLOCK(pCtx_, "ERROR_1") {
            GBL_API_ERROR("Standard");
        };
        GBL_API_BLOCK(pCtx_, "ERROR_N") {
            GBL_API_ERROR("Variadic - %s %d %c", "lul", 45, 'c');
        };
    }

    void logPush(void) {
        GBL_API_BLOCK(pCtx_, "PUSH_0") {
           // GBL_API_PUSH(); GCC fucked
        };

        GBL_API_BLOCK(pCtx_, "PUSH_1") {
            GBL_API_PUSH("Single Push");
        };

        GBL_API_BLOCK(pCtx_, "PUSH_N") {
            GBL_API_PUSH("%s %d %c", "Variadic Push", -33, 'q');
        };

    }

    void logPop(void) {
        GBL_API_BLOCK(pCtx_, "POP_0") {
            //GBL_API_POP(); FUCKED OVERLOAD!!!
        };
        GBL_API_BLOCK(pCtx_, "POP_1") {
            GBL_API_PUSH(""); //GCC FUCKED
            GBL_API_PUSH("");
            GBL_API_POP(2);
        };
    }

    void memAlloc(void) {
#if 1
        GBL_API_BLOCK(pCtx_, "MALLOC_1") {
           void* pPtr = GBL_API_MALLOC(32);
            QVERIFY2(pPtr, "Malloc returned nullptr!");
        };

        GBL_API_BLOCK(pCtx_, "MALLOC_2") {
           void* pPtr = GBL_API_MALLOC(32, 32);
            QVERIFY2(pPtr, "Aligned malloc returned nullptr!");
        };

        GBL_API_BLOCK(pCtx_, "MALLOC_3") {
            void* pPtr = GBL_API_MALLOC(34, 1, "Alloc Info!");
             QVERIFY2(pPtr, "Aligned malloc with debug info returned nullptr!");
        };
#endif
    }

    void memRealloc(void) {
#if 1
        GBL_API_BLOCK(pCtx_, "REALLOC_2") {
            char* pPtr = (char*)GBL_API_MALLOC(15, 1, "Test");
            QVERIFY2(pPtr, "Aligned malloc + debug info returned nullptr!");
            QVERIFY(strncpy(pPtr, "LUL", 15));
            pPtr = (char*)GBL_API_REALLOC(pPtr, 37);
            QVERIFY2(pPtr, "Realloc returned nullptr!");
            QVERIFY2(strncmp(pPtr, "LUL", 37) == 0, "Realloc lost its data!");
        };

        GBL_API_BLOCK(pCtx_, "REALLOC_3") {
            char* pPtr = (char*)GBL_API_MALLOC(15, 1, "Test");
            QVERIFY2(pPtr, "Aligned malloc + debug info returned nullptr!");
            QVERIFY(strncpy(pPtr, "LUL", 15));
            pPtr = (char*)GBL_API_REALLOC(pPtr, 64, 8);
            QVERIFY2(pPtr, "Realloc returned nullptr!");
            QVERIFY2(strncmp(pPtr, "LUL", 64) == 0, "Realloc lost its data!");
        };
#endif
    }

    void memFree(void) {
#if 1
        GBL_API_BLOCK(pCtx_, "FREE") {
            char* pPtr = (char*)GBL_API_MALLOC(15, 1, "Test");
            QVERIFY2(pPtr, "Aligned malloc + debug info returned nullptr!");
            GBL_API_FREE(pPtr);
        };
#endif
    }

    void verifyHandle(void) {

        {
            auto block = GBL_API_BLOCK(pCtx_, "VERIFY_HANDLE_1") {
                GBL_API_VERIFY_HANDLE(pCtx_);
                GBL_API_END();

                //expect fail?
                //GBL_API_VERIFY_HANDLE(GBL_HANDLE_INVALID);
            };
#if 0
            QVERIFY2(block.succeeded(), block.toString()); //no errors or anything, high-level success
            QVERIFY2(block.apiCountersTotal() == 0, "shouldn't have made any API calls!");
            QVERIFY2(block.activeAllocations() == 0, "shouldn't have allocated anything!");
            QVERIFY2(block.logDepth() == 0, "log isn't balanced!");
            QVERIFY2(block.logWriteCounter(VERBOSE) == 0, "Shouldn' have written!");
            QVERIFY2(!block.threwException(), block.getException().what());
#endif
            //QVERIFY(source location shit);
            // log depth watermark
            // allocation watermark
            // active/free bytes/allocations
            // get last error


        }

        GBL_API_BLOCK(pCtx_, "VERIFY_HANDLE_2") {
           // GBL_API_VERIFY_HANDLE(pCtx_, "Handle is fucking reested!");
            //expect fail?
            //GBL_API_VERIFY_HANDLE(nullptr, "Handle is fucking reested!");
            GBL_API_END();
        };

        GBL_API_BLOCK(pCtx_, "VERIFY_HANDLE_N") {
            //GBL_API_VERIFY_HANDLE(pCtx_, "Handle is fucking reested to shit! %p %s %c!", pCtx_, "Test", 'q');
            //expect fail?
            //GBL_API_VERIFY_HANDLE(pCtx_, "Handle is fucking reested to shit! %p %s %c!", pCtx_, "Test", 'q');
        };
    }

    void verifyPointer(void) {
        GBL_API_BLOCK(pCtx_, "VERIFY_POINTER_1") {
            GBL_API_VERIFY_POINTER(pCtx_);
            //expect fail?
            //GBL_API_VERIFY_POINTER(nullptr);
            GBL_API_END();
        };

        GBL_API_BLOCK(pCtx_, "VERIFY_POINTER_2") {
          //  GBL_API_VERIFY_POINTER(pCtx_, "Pointer is fucking reested!");
            //expect fail?
            //GBL_API_VERIFY_POINTER(nullptr, "Pointer is fucking reested!");
            GBL_API_END();
        };

        GBL_API_BLOCK(pCtx_, "VERIFY_POINTER_N") {
            //GBL_API_VERIFY_POINTER(pCtx_, "Pointer is fucking reested! %p %c %d", pCtx_, 'z', -45);
            //expect fail?
            //GBL_API_VERIFY_POINTER(nullptr, "Pointer is fucking reested! %p %c %d", pCtx_, 'z', -45);
            GBL_API_END();
        };
    }

    void verifyArg(void) {
        GBL_API_BLOCK(pCtx_, "VERIFY_ARG_1") {
            int arg1 = 0;
            GBL_API_VERIFY_ARG(arg1 >= 0 && arg1 <= 100);
            //expect fail?
            //GBL_API_VERIFY_ARG(arg == 10);
            GBL_API_END();
        };

        GBL_API_BLOCK(pCtx_, "VERIFY_ARG_2") {
            int arg1 = 0;
            //GBL_API_VERIFY_ARG(arg1 >= 0 && arg1 <= 100, "Out-of-range!");
            //expect fail?
            //GBL_API_VERIFY_ARG(arg == 10, "Out-of-range!");
            GBL_API_END();
        };

        GBL_API_BLOCK(pCtx_, "VERIFY_ARG_N") {
            int arg1 = 0;
            //GBL_API_VERIFY_ARG(arg1 >= 0 && arg1 <= 100, "%s Out-of-range%c", "WAY", '!');
            //expect fail?
            //GBL_API_VERIFY_ARG(arg1 == 100, "%s Out-of-range%c", "WAY", '!');
        };
    }

    void verifyExpr(void) {
        GBL_API_BLOCK(pCtx_, "VERIFY_EXPR_2") {
            int arg1 = 0;
            //GBL_API_VERIFY_EXPR(arg1 >= 0, Result(Result::ErrorUnderflow));
            //expect fail?
            //GBL_API_VERIFY_EXPR(arg1 < 0, Result(Result::ErrorOverflow));
            GBL_API_END();
        };

        GBL_API_BLOCK(pCtx_, "VERIFY_EXPR_3") {
            int arg1 = 0;
            //GBL_API_VERIFY_EXPR(arg1 >= 0, Result(Result::ErrorUnderflow), "Out of Range!");
            //expect fail?
            //GBL_API_VERIFY_EXPR(arg1 < 0, Result(Result::ErrorOverflow), "Out of Range!");
            GBL_API_END();
        };

        GBL_API_BLOCK(pCtx_, "VERIFY_EXPR_N") {
            int arg1 = 0;
            //GBL_API_VERIFY_EXPR(arg1 >= 0, Result(Result::ErrorUnderflow), "%s Out of Range%c", "WAY", '!');
            //expect fail?
            //GBL_API_VERIFY_EXPR(arg1 < 0, Result(Result::ErrorOverflow), "%s Out of Range%c", "WAY", '!');
            GBL_API_END();
        };
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

    void forwardCall(void) {
        // try with no error
        // try with partial, error, etc
        // try multiple levels of chaining
        // check return type when back at lowest-level
        // check stack and lastError to ensure it's the inner error
    }

    void destroy(void) {
        try {
            delete pCtx_;
        } catch(const std::exception& ex) {
            QFAIL(ex.what());
        }
    }

};


}


#endif // TEST_GIMBAL_CONTEXT_HPP
