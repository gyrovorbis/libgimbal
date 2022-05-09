#ifndef TEST_GIMBAL_CONTEXT_HPP
#define TEST_GIMBAL_CONTEXT_HPP

#include "test_gimbal.hpp"

#include <gimbal/objects/gimbal_context.hpp>
#include <elysian_qtest.hpp>
#include <QObject>
#include <QDebug>
#include <QStack>



namespace gimbal::test {



class Object: public UnitTestSet {
Q_OBJECT

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

private slots:


};

/* 1) override default log function(s)
 * 2)
 *
 *
 */


/* TO TEST
 * 1) version
 * 2) throwing C++ exceptions from C return codes
 * 3) catching C++ exc eptions as C return codes
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

    char* ppPtr[3] = { nullptr };

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

            qDebug() << std::string(Result(Result::ErrorInvalidPointer).toString()).c_str();

        } catch(const std::exception& ex) {
            QFAIL(ex.what());
        }


    }

    // stack monitors!?!?

    // === API TEST WHICH CAN TAKE A HANDLE ====

    void begin(void) {
        auto test = GBL_TEST_CASE_API_BLOCK(pCtx(), "BEGIN_0") {
            {
                GBL_API_BEGIN(gblCtx());
            }
        };
        QVERIFY(test);
        QCOMPARE(test.getCountersDelta().getLogDepth(), 0);

        test = GBL_TEST_CASE_API_BLOCK(pCtx(), "BEGIN_1") {
            {
                GBL_API_BEGIN(gblCtx(), "Single");
            }
        };
        QVERIFY(test);
        QCOMPARE(test.getCountersDelta().getLogDepth(), 1);
        QCOMPARE(test.getCountersDelta().getLog(LogLevel::Verbose), 1);

        test = GBL_TEST_CASE_API_BLOCK(pCtx_, "BEGIN_N") {
            {
                GBL_API_BEGIN(gblCtx(), "%s - %d", "Variadic", 33);
            }
        };
        QVERIFY(test);
        QCOMPARE(test.getCountersDelta().getLogDepth(), 1);
        QCOMPARE(test.getCountersDelta().getLog(LogLevel::Verbose), 1);
    }

    void end(void) {

        auto test = GBL_TEST_CASE_API_BLOCK(pCtx(), "END") {
            {
                GBL_API_BEGIN(gblCtx(), "Layer1");
                GBL_API_END();
            }
        };
        QVERIFY(test);
        QCOMPARE(test.getCountersDelta().getLogDepth(), 0); //make sure log depth is respected
    }

    void logDebug(void) {
        auto test = GBL_TEST_CASE_API_BLOCK(pCtx_, "DEBUG_1") {
            GBL_API_DEBUG("Standard");
        };
        QVERIFY(test);
        QCOMPARE(test.getCountersDelta().getLog(LogLevel::Debug), 1);

        test = GBL_TEST_CASE_API_BLOCK(pCtx_, "DEBUG_N") {
            GBL_API_DEBUG("Variadic - %s %d %c", "lul", 45, 'c');
        };
        QVERIFY(test);
        QCOMPARE(test.getCountersDelta().getLog(LogLevel::Debug), 1);
    }

    void logVerbose(void) {
        auto test = GBL_TEST_CASE_API_BLOCK(pCtx_, "VERBOSE_1") {
            GBL_API_VERBOSE("Standard");
        };
        QVERIFY(test);
        QCOMPARE(test.getCountersDelta().getLog(LogLevel::Verbose), 1);

        test = GBL_TEST_CASE_API_BLOCK(pCtx_, "VERBOSE_N") {
            GBL_API_VERBOSE("Variadic - %s %d %c", "lul", 45, 'c');
        };
        QVERIFY(test);
        QCOMPARE(test.getCountersDelta().getLog(LogLevel::Verbose), 1);

    }

    void logInfo(void) {
        auto test = GBL_TEST_CASE_API_BLOCK(pCtx_, "INFO_1") {
            GBL_API_INFO("Standard");
        };
        QVERIFY(test);
        QCOMPARE(test.getCountersDelta().getLog(LogLevel::Info), 1);

        test = GBL_TEST_CASE_API_BLOCK(pCtx_, "INFO_N") {
            GBL_API_INFO("Variadic - %s %d %c", "lul", 45, 'c');
        };
        QVERIFY(test);
        QCOMPARE(test.getCountersDelta().getLog(LogLevel::Info), 1);
    }

    void logWarn(void) {
        auto test = GBL_TEST_CASE_API_BLOCK(pCtx_, "WARN_1") {
            GBL_API_WARN("Standard");
        };
        QVERIFY(test);
        QCOMPARE(test.getCountersDelta().getLog(LogLevel::Warning), 1);

        test = GBL_TEST_CASE_API_BLOCK(pCtx_, "WARN_N") {
            GBL_API_WARN("Variadic - %s %d %c", "lul", 45, 'c');
        };
        QVERIFY(test);
        QCOMPARE(test.getCountersDelta().getLog(LogLevel::Warning), 1);
    }

    void logError(void) {
        auto test = GBL_TEST_CASE_API_BLOCK(pCtx_, "ERROR_1") {
            GBL_API_ERROR("Standard");
        };
        QVERIFY(test);
        QCOMPARE(test.getCountersDelta().getLog(LogLevel::Error), 1);

        test = GBL_TEST_CASE_API_BLOCK(pCtx_, "ERROR_N") {
            GBL_API_ERROR("Variadic - %s %d %c", "lul", 45, 'c');
        };
        QVERIFY(test);
        QCOMPARE(test.getCountersDelta().getLog(LogLevel::Error), 1);
    }

    void logPush(void) {
        auto test = GBL_TEST_CASE_API_BLOCK(pCtx_, "PUSH_0") {
            GBL_API_PUSH();
        };
        QVERIFY(test);
        QCOMPARE(test.getCountersDelta().getLogDepth(), 1);
        QCOMPARE(test.getCountersDelta().getExt(ContextCounters::ApiExtCall::LogPush), 1);

        test = GBL_TEST_CASE_API_BLOCK(pCtx_, "PUSH_1") {
            GBL_API_PUSH_VERBOSE("Single Push");
        };
        QVERIFY(test);
        QCOMPARE(test.getCountersDelta().getLogDepth(), 1);
        QCOMPARE(test.getCountersDelta().getLog(LogLevel::Verbose), 1);
        QCOMPARE(test.getCountersDelta().getExt(ContextCounters::ApiExtCall::LogPush), 1);

        test = GBL_TEST_CASE_API_BLOCK(pCtx_, "PUSH_N") {
            GBL_API_PUSH_VERBOSE("%s %d %c", "Variadic Push", -33, 'q');
        };
        QVERIFY(test);
        QCOMPARE(test.getCountersDelta().getLogDepth(), 1);
        QCOMPARE(test.getCountersDelta().getLog(LogLevel::Verbose), 1);
        QCOMPARE(test.getCountersDelta().getExt(ContextCounters::ApiExtCall::LogPush), 1);

    }

    void logPop(void) {
        auto test = GBL_TEST_CASE_API_BLOCK(pCtx_, "POP_BALANCED") {
            GBL_API_PUSH();
            GBL_API_POP(1);
        };
        QVERIFY(test);
        QCOMPARE(test.getCountersDelta().getLogDepth(), 0);
        QCOMPARE(test.getCountersDelta().getExt(ContextCounters::ApiExtCall::LogPop), 1);


        test = GBL_TEST_CASE_API_BLOCK(pCtx_, "POP_FAIL") {
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

    void stlPmr(void) {
        auto test = GBL_TEST_CASE_API_BLOCK(pCtx(), "PMR_VECTOR") {
            std::pmr::vector<std::string> strings{1, pCtx()};
            strings.emplace_back("324324324333333333");
            strings.emplace_back("23432453453453452345234523452");
        };
        QVERIFY(test);
        QVERIFY(test.getCountersDelta().getExt(ContextCounters::ApiExtCall::MemAlloc) >= 2);
        QVERIFY(test.getCountersDelta().getExt(ContextCounters::ApiExtCall::MemFree) >= 2);
        QCOMPARE(test.getActiveAllocCount(), 0);

        test = GBL_TEST_CASE_API_BLOCK(pCtx(), "PMR_STRING") {
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
            auto test = GBL_TEST_CASE_API_BLOCK(pCtx(), "RESULT_SET_1") {                         \
                GBL_API_RECORD_SET(GBL_RESULT_UNKNOWN);                                 \
            };                                                                          \
            verifyBlock(test, options, Result::Unknown, "Unknown");                     \
                                                                                        \
            test = GBL_TEST_CASE_API_BLOCK(pCtx_, "RESULT_SET_2") {                               \
                GBL_API_RECORD_SET(GBL_RESULT_SUCCESS, "WORX!");                        \
            };                                                                          \
            verifyBlock(test, options, Result::Success, "WORX!");                       \
                                                                                        \
            test = GBL_TEST_CASE_API_BLOCK(pCtx_, "RESULT_SET_N") {                               \
                GBL_API_RECORD_SET(GBL_RESULT_TRUNCATED, "Reested to %s.", "fuck");     \
            };                                                                          \
            verifyBlock(test, options, Result::Truncated, "Reested to fuck.");          \
                                                                                        \
            test = GBL_TEST_CASE_API_BLOCK(pCtx_, "RESULT_SET_N") {                               \
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
        auto test = GBL_TEST_CASE_API_BLOCK(pCtx(), "VERIFY_2_SUCCESS") {
            GBL_API_VERIFY(1 < 3, Result(Result::ErrorUnhandledException));
               GBL_API_END_EMPTY();
        };
        QVERIFY(test);

       test = GBL_TEST_CASE_API_BLOCK(pCtx(), "VERIFY_3_ERROR") {
            GBL_API_VERIFY(false == true, Result(Result::ErrorTypeMismatch), "Impossibru!");
               GBL_API_END_EMPTY();
        };
        verifyBlock(test, GBL_CONFIG_OPTIONS_DECL(), GBL_RESULT_ERROR_TYPE_MISMATCH, "Impossibru!");

        test = GBL_TEST_CASE_API_BLOCK(pCtx(), "VERIFY_4_WARN") {
            GBL_API_VERIFY(sizeof(int) < sizeof(char), GBL_RESULT_TRUNCATED, "Truncated: %d", -12);
               GBL_API_END_EMPTY();
        };
        verifyBlock(test, GBL_CONFIG_OPTIONS_DECL(), Result::Truncated, "Truncated: -12");
    }

    void verifyExpression(void) {
        auto test = GBL_TEST_CASE_API_BLOCK(pCtx(), "VERIFY_EXPR_1_ERROR") {
            GBL_API_VERIFY_EXPRESSION(3 < 1);
            GBL_API_END_EMPTY();
        };
        verifyBlock(test, GBL_CONFIG_OPTIONS_DECL(), Result::ErrorInvalidExpression, "Invalid Expression: 3 < 1");

        test = GBL_TEST_CASE_API_BLOCK(pCtx(), "VERIFY_EXPR_2_ERROR") {
            GBL_API_VERIFY_EXPRESSION(sizeof(int) < sizeof(char), "Fucked");
            GBL_API_END_EMPTY();
        };
        verifyBlock(test, GBL_CONFIG_OPTIONS_DECL(), Result::ErrorInvalidExpression, "Fucked");

        test = GBL_TEST_CASE_API_BLOCK(pCtx(), "VERIFY_EXPR_N_SUCCESS") {
            GBL_API_VERIFY_EXPRESSION(sizeof(int) > sizeof(char), "Random Shit: %d, %s, %c, %f", -12, "lulz", 'a', 12.2f);
            GBL_API_END_EMPTY();
        };
        QVERIFY(test);
    }

    void verifyHandle(void) {
#if 0
        auto test = GBL_TEST_CASE_API_BLOCK(pCtx(), "VERIFY_HANDLE_1_FAIL") {
            GBL_API_VERIFY_HANDLE(NULL);
            GBL_API_END_EMPTY();
        };
        verifyBlock(test, GBL_CONFIG_OPTIONS_DECL(), Result::ErrorInvalidHandle, "Invalid Handle");

        test = GBL_TEST_CASE_API_BLOCK(pCtx(), "VERIFY_HANDLE_2_PASS") {
            GBL_API_VERIFY_HANDLE(pCtx(), "Valid as fuck");
            GBL_API_END_EMPTY();
        };
        QVERIFY(test);

        test = GBL_TEST_CASE_API_BLOCK(pCtx(), "VERIFY_HANDLE_N_FAIL") {
            GBL_API_VERIFY_HANDLE(NULL, "Assdouchery %s %d %c", "lul", -33, 'b');
            GBL_API_END_EMPTY();
        };
        verifyBlock(test, GBL_CONFIG_OPTIONS_DECL(), GBL_RESULT_ERROR_INVALID_HANDLE, "Assdouchery lul -33 b");
#endif
    }

    void verifyPointer(void) {
        auto test = GBL_TEST_CASE_API_BLOCK(pCtx(), "VERIFY_POINTER_1_FAIL") {
            GBL_API_VERIFY_POINTER(NULL);
            GBL_API_END_EMPTY();
        };
        verifyBlock(test, GBL_CONFIG_OPTIONS_DECL(), Result::ErrorInvalidPointer, "Invalid Pointer");

        test = GBL_TEST_CASE_API_BLOCK(pCtx(), "VERIFY_POINTER_2_PASS") {
            GBL_API_VERIFY_POINTER(pCtx(), "Valid as fuck");
            GBL_API_END_EMPTY();
        };
        QVERIFY(test);

        test = GBL_TEST_CASE_API_BLOCK(pCtx(), "VERIFY_POINTER_N_FAIL") {
            GBL_API_VERIFY_POINTER(NULL, "Assdouchery %s %d %c", "lul", -33, 'b');
            GBL_API_END_EMPTY();
        };
        verifyBlock(test, GBL_CONFIG_OPTIONS_DECL(), GBL_RESULT_ERROR_INVALID_POINTER, "Assdouchery lul -33 b");
    }

    void verifyArg(void) {
        auto test = GBL_TEST_CASE_API_BLOCK(pCtx(), "VERIFY_ARG_1_FAIL") {
            int arg1 = 3; int arg2 = 44;
            GBL_API_VERIFY_ARG(arg1 != 0);
            GBL_API_VERIFY_ARG(arg2 > 50);
            GBL_API_END_EMPTY();
        };
        verifyBlock(test, GBL_CONFIG_OPTIONS_DECL(), Result::ErrorInvalidArg, "Invalid Arg: arg2 > 50");

        test = GBL_TEST_CASE_API_BLOCK(pCtx(), "VERIFY_ARG_2_PASS") {
            float argIn = -33.0f;
            GBL_API_VERIFY_ARG(fabs(argIn) > 30.0f, "Your argument sucks!");
            GBL_API_END_EMPTY();
        };
        QVERIFY(test);

        test = GBL_TEST_CASE_API_BLOCK(pCtx(), "VERIFY_ARG_N_FAIL") {
            const char* pName = "Fuckwhisp";
            GBL_API_VERIFY_ARG(strcmp(pName, "DickWheeze") == 0, "Assdouchery %s %d %c", "lul", -33, 'b');
            GBL_API_END_EMPTY();
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
    GBL_STMT_START {                                            \
        if(errno) GBL_UNLIKELY {                    \
            const GBL_RESULT code =                 \
                GBL_ERRNO_RESULT(errno);            \
            GBL_API_VERIFY(                         \
                GBL_RESULT_SUCCESS(code),           \
                code);                              \
        }                                           \
    } GBL_STMT_END
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
        QSKIP("These tests need to be fucking updated");
        auto test = GBL_TEST_CASE_API_BLOCK(pCtx(), "CALL_1_FAIL") {
            //GBL_API_CALL(gblContextVersion(nullptr, nullptr));
            GBL_API_END_EMPTY();
        };
        verifyBlock(test, GBL_CONFIG_OPTIONS_DECL(), Result::ErrorInvalidPointer, "Call[gblContextVersion(nullptr, nullptr)] -> Result[Invalid Pointer]");

        test = GBL_TEST_CASE_API_BLOCK(pCtx(), "CALL_2_PASS") {
            GblVersion version;
            //GBL_API_CALL(gblContextVersion(&version, nullptr),
             //            "Getting the goddamn version!");
            GBL_API_END_EMPTY();
        };
        QVERIFY(test);

        test = GBL_TEST_CASE_API_BLOCK(pCtx(), "CALL_N_FAIL") {
          //  GBL_API_CALL(gblOjbectUserdataGet(GBL_API_OBJECT(), nullptr),
            //             "Variadicery %s %d %c %.2f", "lul", -33, 'b', 22.43f);
            GBL_API_END_EMPTY();
        };
        verifyBlock(test, GBL_CONFIG_OPTIONS_DECL(), GBL_RESULT_ERROR_INVALID_POINTER, "Variadicery lul -33 b 22.43");


        //GBL_API_CALL();
        // try with no error
        // try with partial, error, etc
        // try multiple levels of chaining
        // check return type when back at lowest-level
        // check stack and lastError to ensure it's the inner error
    }

    void exceptionsThrow(void) {
        try {
            auto test = GBL_TEST_API_BLOCK() {
                Exception::checkThrow(Result::ErrorUnhandledException);
            };
            QFAIL("Expected Exception!");
        } catch(const gimbal::Exception& gblExcept) {
            QCOMPARE(gblExcept.getResult(), Result(Result::ErrorUnhandledException));
            QCOMPARE(gblExcept.getMessage(), "Unhandled Exception");
        }

        try {
            auto test = GBL_TEST_API_BLOCK() {
                int arg1 = 0;
                GBL_API_VERIFY_ARG(arg1 > 44);
                GBL_API_END_BLOCK();
                gimbal::Exception::checkThrow(GBL_API_RECORD());
            };
            QFAIL("Expected Exception!");
        } catch(std::invalid_argument& invalid_arg) {
            qDebug() << "WHAT: " << invalid_arg.what();
        } catch(std::exception& except) {
            qDebug() << "WTF: " << except.what();
        } catch(gimbal::Exception& except) {
            qDebug() << "EXCEPT: " << except.what();
        }
    }

    void exceptionsCatch(void) {

        auto block = Exception::TryBlock({SRC_FILE, SRC_FN, SRC_LN, SRC_COL}) = [&](){
            throw std::invalid_argument("fucked argc");
        };
        QVERIFY(!block);
        QCOMPARE(block.getResult(), Result::ErrorInvalidArg);
        QCOMPARE(block.getMessage(), "fucked argc");


        block = Exception::TryBlock({SRC_FILE, SRC_FN, SRC_LN, SRC_COL}) = [&](){
            GBL_API_BEGIN(hCtx());
            GBL_API_VERIFY_EXPRESSION(3 > 5);
            GBL_API_END_BLOCK();
            Exception::throwException(GBL_API_RECORD());
        };
        QVERIFY(!block);
        QCOMPARE(block.getResult(), Result::ErrorInvalidExpression);
        QCOMPARE(block.getMessage(), "Invalid Expression: 3 > 5");

        block = Exception::TryBlock({SRC_FILE, SRC_FN, SRC_LN, SRC_COL}) = [&](){
            throw std::bad_cast();
        };
        QVERIFY(!block);
        QCOMPARE(block.getResult(), Result::ErrorUnhandledException);
        QCOMPARE(block.getMessage(), "std::bad_cast");

        block = Exception::TryBlock({SRC_FILE, SRC_FN, SRC_LN, SRC_COL}) = [&](){
            throw "fuck off";
        };
        QVERIFY(!block);
        QCOMPARE(block.getResult(), Result::ErrorUnhandledException);
        QCOMPARE(block.getMessage(), "Unknown Exception Type!");

    }

    void exceptionInterop(void) {

        auto test = GBL_TEST_CASE_API_BLOCK(pCtx(), "C_TO_CPP_CALL") {
            GBL_API_TRY {
                std::pmr::vector<int> vec(1, pCtx());
                auto val = vec.at(5);
                GBL_UNUSED(val);
            } GBL_API_VERIFY_CATCH();
            GBL_API_END();
        };
        QVERIFY(!test.didThrow());
        QString message = test.getRecord().getMessage().data();
        verifyBlock(test, GBL_CONFIG_OPTIONS_DECL(), GBL_RESULT_ERROR_OUT_OF_RANGE);

        auto block = GBL_API_TRY_BLOCK {
            Exception::throwException(test.getRecord());
        };
        QVERIFY(!block);
        QCOMPARE(block.getResult(), Result::ErrorOutOfRange);
        QCOMPARE(block.getMessage(), message);


        ApiBlock ptrTest(GBL_OBJECT(hCtx()));
        try {
            ptrTest = GBL_TEST_API_BLOCK() {
                Exception::checkThrow(block.getRecord());
            };
            QFAIL("Expected Exception!");
        } catch(const gimbal::Exception& gblExcept) {
            QCOMPARE(gblExcept.getResult(), Result::ErrorOutOfRange);
            QCOMPARE(gblExcept.getMessage().data(), message);
        }

#if 0
        QVERIFY(ptrTest.didThrow());

        try {
            throw ptrTest.getExceptionPtr();
            QFAIL("Expected Exception!");
        } catch(const std::exception& stdExcept) {
            QCOMPARE(stdExcept.what(), message);
        }
    #endif
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
