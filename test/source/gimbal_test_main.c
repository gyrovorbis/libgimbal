#include "gimbal_tests.h"

#if defined(__ANDROID__) && TINYCTHREAD_ENABLE_THREADS
int start_logger(const char* app_name);
#endif

int main(int argc, const char* pArgv[]) {
#if defined(__DREAMCAST__) && !defined(NDEBUG)
  //  gdb_init();
#elif defined(__ANDROID__) && TINYCTHREAD_ENABLE_THREADS
    start_logger("");
#endif
    return GblTestScenario_exec(GBL_TEST_SCENARIO(GBL_NEW(GimbalTests)), argc, pArgv);
}