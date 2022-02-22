#ifndef GIMBAL_TEST_H
#define GIMBAL_TEST_H

#include "gimbal_context.h"
#include "gimbal_container.h"

#ifdef __cplusplus
extern "C" {

#define GBL_TEST_NAME_SIZE_DEFAULT  2000


#ifndef GBL_TEST_SET_USERDATA_TYPE
#   define GBL_TEST_SET_USERDATA_TYPE void*
#endif

typedef GBL_TEST_SET_USERDATA_TYPE GblTestSetUserdata;

struct GblTestCase;

typedef struct GblTestContext {
    GblTestSet baseSet;
    GblContext baseContext;
    //struct GblTestCase* pCaseCurrent;
    struct GblTestCase* pCaseTopLevel;

} GblTestContext;

typedef GBL_TEST_RESULT (*GblTestRunFn)(struct GblContext* pCtx, GblTest* pUserdata, void* pCaseArgs);
typedef GBL_TEST_RESULT (*GblTestCaseInitFn)(struct GblContext* pCtx, GblTestCase* pTestCase, void** ppUserdata);
typedef GBL_TEST_RESULT (*GblTestCaseDestroyFn)(struct GblContext* pCtx, GblTestSet* pUserdata);

typedef enum GBL_TEST_RESULT {
    GBL_TEST_RESULT_UNKNOWN,
    GBL_TEST_RESULT_PASS,
    GBL_TEST_RESULT_FAIL,
    GBL_TEST_RESULT_SKIPPED
} GBL_TEST_CASE_RESULT;

typedef struct GblTest {
    char pName[GBL_TEST_NAME_SIZE_DEFAULT];
    GblTestRunFn    pFnTestRun;
    GblTestInitFn   pFnTestInit;
    GblTestDeinitFn pFnTestDeinit;
    // execution time
    // allocation callbacks
} GblTest;


typedef struct GblTestSet {
    GblTest baseTest;
    GblVector tests;
} GblTestSet;

typedef struct GblTestCase {
    GblTest* pTest;
    char pName[GBL_TEST_NAME_SIZE_DEFAULT];
    void* pUserdata;
    GBL_TEST_RESULT result;
} GblTestCase;



GBL_TEST_SET_CREATE("Cpu",
             &_evmuCpuTestSetInit,
             &_evmuCpuTestSetDeinit);
//testCaseInit
//testCaseDeinit
GBL_TEST_CREATE("InstructionDecode",
                &_evmuCpuTestInstructionDecode)


GBL_TEST_ENQUEUE(gblTestContext, //adds a test case to a test set
                      &_gblCpuTestSet,
                      nullptr); // test case args


#define GBL_TEST_CASE_PUSH(CASE)

#define GBL_TEST_CASE_POP(CASE)

inline GBL_TEST_RESULT GBL_TEST_RUN(GblTestContext* pCtx, const GblTest* pTest, void* pCaseData) {
    GBL_TEST_ASSERT(pCtx);
    GBL_TEST_ASSERT(pTest);
    GBL_TEST_CASE_PUSH(pCtx, pTest->pName);

    GBL_TEST_ASSERT(pTest->pFnTestRun);
    GBL_TEST_CASE_VERIFY(pTest->pFnTestRun(pCtx, pTest, pCaseData));

    return GBL_TEST_CASE_POP();
}

inline GBL_TEST_RESULT gblTestCaseConstruct(GblTestContext* pCtx, const GblTestSet* pSet, GblTestCase* pCase) {

}

inline GBL_TEST_RESULT gblTestSetRun(GblTestContext* pCtx, const GblTestSet* pSet, void* pCaseData) {
    const GblSize testCases = 0;
    GblTest* pCurrentTest = NULL;
    void* pTesCaseUserdata = NULL;
    GBL_TEST_ASSERT(gblVectorSize(&pSet->cases));

    if(pSet->pFnTestCaseCreate) {
        GBL_TEST_CASE_PUSH("Init()");
        GBL_TEST_VERIFY(pSet->pfnTestInit(pCtx, )
        GBL_TEST_CASE_POP();
    }

    if(testCases) {
        for(GblSize c = 0; c < testCases; ++c) {
            GBL_TEST_ASSERT(gblVectorAt(&pSet->cases, c, &pCurrentTest));
            GBL_TEST_RUN(pCtx, pSet, pCaseData);

        }

    }



    if(gblVectorSize(&pSet->cases)) {



    }

}


/*   Lua:
 *
 *   return LuaEvmuCpuTestSet = new GimbalTestSet {
 *      init = function (self, cpuWordSize, useJitCore)
 *          self.cpuWordSize = cpuWordSize
 *          self.useJitCore = useJitCore
 *      -- add random test case functions to vector
 *      end,
 *      deinit = function (self)
 *
 *      end,
 *
 *      decodeInstruction = function(self)
 *
 *      end,
 *
 *      runCycle = function(self)
 *
 *      end
 *  }
 *
 *  C++:
 *
 *  class CppEvmuCpuTestSet: public Gimbal::TestSet {
 *  private:
 *      uint8_t cpuWordSize;
 *      bool useJitCore;
 *
 *      CppEvmuTestSet(uint8_t cpuWordSize, bool useJitCore) {
 *          snprintf(this->testCaseName, "Jit = %d, Word = %d", useJitCore, cpuWordSize); // defaults to "Default"
 *          registerTestCases(decodeInstruction,
 *                            runCycle);
 *      }

        virtual Gimbal::TestResult init();
        virtual Gimbal::TestResult deinit();

        Gimbal::TestResult decodeInstruction();
        Gimbal::TestResult runCycle();
 *  };
 *
 *  Qt:
 *  template<typename QtTest>
 *  class QtEvmuCpuTestSetAdapter: public QtTest,
 *                                 public Gimbal::TestSet
 *  {
 *          template<typename... Args>
 *          QtEvmuTestSetAdapter(Args&&.. args):
 *              QtTest(std::forward<Args>(args)...),
 *              Gimbal::TestSet(QObject::name.toStdString())
 *          {
 *              //foreach MOC metamethod
 *              //add to Gimbal::TestSet registry
 *          }
 *
 *          virtual Gimbal::TestResult init() {
 *              return QtTest::init();
 *          }
 *
 *          virtual Gimbal::TestResult deinit() {
 *              return QtTest::deinit();
 *          }
 *  };
 *

 * C:
 *
 *
 * typedef struct GblTestCaseMetaType {
 *      const char pName[GBL_TEST_NAME_SIZE_DEFAULT];
 *      GblTestRunFn pFnRun;
 * } GblTestCaseMetaType;
 *
 * typedef struct GblTestSetMetaType {
 *      const char pName[GBL_TEST_NAME_SIZE_DEFAULT];
 *      GblTestInitFn pFnInit;
 *      GblTestDeinitFn pFnDeinit;
 *      const TestCaseMetaType** ppTestCases;
 * } GblTestSetTypeMetaData;
 *
 *  typedef struct GblTestSet {
 *      const GblTestSetMeta* pMetaData;
 *      char pInstanceName[GBL_TEST_NAME_SIZE_DEFAULT];
 *      void* pUserdata;
 *  } GblTestSet;
 *
 *
 *#define GBL_TEST_SET_DECLARE_METATYPE(NAME,
 *
 *  #define GBL_TEST_SET_INIT_FN(PREFIX)
 *  #define GBL_TEST_SET_DEINIT_FN(PREFIX)
 *  #define GBL_TEST_SET_CASE_FN(PREFIX, NAME)
 *
 *
 *
 * typedef struct CEvmuCpuTestSet {
 *      GblTestSet base;
 *      uint8_t cpuWordSize;
 *      GblBool useJitCore;
 *  } CEVmuCpuTestSet;
 *
 * * *  static const GblTestSetMeta CEvmuCpuTestSetMeta = {
 *      .pName = "CEvmuTestSet",
 *      .pInitFn = &CEvmuCpuTestSetInit,
 *      .pDeinitFn = &CEvmuCpuTestSetDeinit,
 *      .pTestCases = (&(const GimbalTest[]){
 *          (&(GimbalTest){ "decodeInstruction", &CEvmuCpuTestSetDecodeInstruction }),
 *          (&(GimbalTest){ "runCycle", &CEvmuCpuTestRunCycle })
 *      })
 *  };
 *
 *
 *  GBL_TEST_RESULT EvmuCpuTestSetInit(GblTestContext* pCtx, EvmuCpuTestSet* pSelf) {
 *
 *  }
 *
 *  GBL_TEST_RESULT EvmuCpuTestSetDeinit(GblTestContext* pCtx, EvmuCpuTestSet* pSelf) {
 *
 *  }
 *
 *  GBL_TEST_RESULT EvmuCpuTestSetDecodeInstruction(GblTestContext* pCtx, EvmuCpuTestSet* pSelf) {
 *
 *  }
 *
 *
 *
 * *GBL_TEST_RESULT EvmuCpuTestSetTestRunCycl(eGblTestContext* pCtx, EvmuCpuTestSet* pSelf) {
 *
 *  }
 *
 *


 *   GBL_TEST_SET_DEFINE_METATYPE(Cpu,
 *      HAS_INIT_FN,
 *      NO_DEINIT_FN,
 *      TEST_CASES(     //or empty test cases or NO_TEST_CASES
 *          decodeInstruction,
 *          runCycle
 *      ),
 *      INSTANCE_DATA { // or NO_INSTANCE_DATA or empty
 *          uint8_t cpuWordSize;
 *          GblBool useJit;
 *      }
 *  );
 *
 *
 * #define GBL_TEST_META_TYPE_NAME(PREFIX) \
 *      Prefix##TestSetMetaType
 *
 *  #define GBL_TEST_INSTANCE_TYPE_NAME(PREFIX) \
 *      PREFIX##TestSet
 *
 *  #define GBL_TEST_METHOD(PREFIX, NAME) \
 *      GBL_TEST_RESULT PREFIX##NAME(GblTestContext* pCtx,  GBL_TEST_INSTANCE_TYPE* pSelf)

 *  #define GBL_TEST_CASE(PREFIX, NAME) \
 *      GBL_TEST_METHOD(PREFIX, NAME)
*
 *#define GBL_TEST_INIT(PREFIX) \
 *  GBL_TEST_METHOD(PREFIX, Init)
 *
#define GBL_TEST_DEINIT(PREFIX) \
 *      GBL_TEST_METHOD(PREFIX, Deinit)
 *
 *
 *  GBL_TEST_INIT(Cpu) {
 *
 *  }
 *
 *  GBL_TEST_DEINIT(Cpu) {
 *
 *  }
 *
 *  GBL_TEST_CASE(Cpu, DecodeInstruction) {
 *
 *  }
 *
 *  GBL_TEST_CASE(Cpu, RunCycle) {
 *
 *  }
 *
 *

 *


 *
 *
 *
 *  CEvmuCpuTestSet testSet1 = {
 *      {
 *          &CEvmuCpuTestSetMeta,
 *          "JIT on",
 *          NULL
 *      }
 *      12,
 *      GBL_TRUE
 *  };
 *
 *    CEvmuCpuTestSet testSet1 = {
 *      {
 *          &CEvmuCpuTestSetMeta,
 *          "JIT on",
 *          NULL
 *      }
 *      8,
 *      GBL_FALSE
 *  };
 *
 *
 *
 *
 *
 */



int main(int argc, char* argv[]) {

    GblTestContext testCtx;
    // configure based on command-line args
    // logs, errors, filters, export to json/lson, whatever

    gblTestCasePush("main");



    gblTestCasePop();

    GBL_TEST_CASE_RESULT result = gblTestCaseRun(&testCtx.baseSet, &testCtx);

    return result;

}




#ifdef __cplusplus
}
#endif

#endif // GIMBAL_TEST_H
