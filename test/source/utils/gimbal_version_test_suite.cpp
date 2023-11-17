#include "utils/gimbal_version_test_suite.hpp"
#include <gimbal/utils/gimbal_version.hpp>

#define GBL_SELF_TYPE GblVersionTestSuiteCpp

GBL_TEST_FIXTURE {
    uintptr_t pad;

};

GBL_TEST_INIT()
GBL_TEST_CASE_END

GBL_TEST_FINAL()
GBL_TEST_CASE_END

GBL_TEST_CASE(constructDefault)
    gbl::Version version{};
    GBL_TEST_VERIFY(version == 0);
    GBL_TEST_VERIFY(version.major() == 0);
    GBL_TEST_VERIFY(version.minor() == 0);
    GBL_TEST_VERIFY(version.patch() == 0);
GBL_TEST_CASE_END

GBL_TEST_CASE(constructGimbal)
    gbl::Version version { GblVersion_make(1, 2, 3) };
    GBL_TEST_VERIFY(version == GBL_VERSION_MAKE(1, 2, 3));
    GBL_TEST_VERIFY(version.major() == 1);
    GBL_TEST_VERIFY(version.minor() == 2);
    GBL_TEST_VERIFY(version.patch() == 3);
GBL_TEST_CASE_END

GBL_TEST_CASE(constructValues)
    gbl::Version version { 1, 2, 3 };
    GBL_TEST_VERIFY(version == GBL_VERSION_MAKE(1, 2, 3));
    GBL_TEST_VERIFY(version.major() == 1);
    GBL_TEST_VERIFY(version.minor() == 2);
    GBL_TEST_VERIFY(version.patch() == 3);
GBL_TEST_CASE_END

GBL_TEST_CASE(constructString)
    gbl::Version version { "1.2.3" };
    GBL_TEST_VERIFY(version.major() == 1);
    GBL_TEST_VERIFY(version.minor() == 2);
    GBL_TEST_VERIFY(version.patch() == 3);
GBL_TEST_CASE_END

GBL_TEST_CASE(setValues)
    gbl::Version version { 1, 2, 3 };
    version.setMajor(3);
    version.setMinor(2);
    version.setPatch(1);
    GBL_TEST_VERIFY(version.major() == 3);
    GBL_TEST_VERIFY(version.minor() == 2);
    GBL_TEST_VERIFY(version.patch() == 1);
GBL_TEST_CASE_END

GBL_TEST_CASE(comparisons)
  //  GBL_TEST_VERIFY(gbl::Version(1, 2, 3) == "1.2.3");
GBL_TEST_CASE_END

GBL_TEST_REGISTER(constructDefault,
                  constructGimbal,
                  constructValues,
                  constructString,
                  setValues,
                  comparisons)
