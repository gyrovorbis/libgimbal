#include "gimbal_tests.h"

int main(int argc, const char* pArgv[]) {
#if defined(__DREAMCAST__) && !defined(NDEBUG)
  //  gdb_init();
#elif defined(__ANDROID__)
    start_logger("");
#endif
    return GblTestScenario_exec(GBL_TEST_SCENARIO(GBL_NEW(GimbalTests)), argc, pArgv);
}
