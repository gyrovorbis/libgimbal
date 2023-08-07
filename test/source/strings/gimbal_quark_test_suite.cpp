#include "strings/gimbal_quark_test_suite.hpp"
#include <iostream>
#include <gimbal/strings/gimbal_quark.hpp>

#define GBL_SELF_TYPE GblQuarkTestSuiteCpp

GBL_TEST_FIXTURE {
    int a;
};

GBL_TEST_INIT()

GBL_TEST_CASE_END

GBL_TEST_FINAL()

GBL_TEST_CASE_END


GBL_TEST_CASE(constructDefault)
    auto quark = gbl::Quark();
    GBL_TEST_VERIFY(!quark.isValid());
    GBL_TEST_VERIFY(quark.length() == 0);
    GBL_TEST_VERIFY(!quark);
    GBL_TEST_VERIFY(quark == gbl::Quark::Invalid);
    GBL_TEST_VERIFY(quark == GBL_QUARK_INVALID);
GBL_TEST_CASE_END

GBL_TEST_CASE(customLiteral)
    using namespace gbl;
    auto quark = "Quarkificate"_qrk;
    GBL_TEST_VERIFY(quark.isValid());
    GBL_TEST_VERIFY(quark.length() == 12);
    GBL_TEST_VERIFY(quark);
    GBL_TEST_VERIFY(quark != GBL_QUARK_INVALID);
    GBL_TEST_VERIFY(strcmp(GblQuark_toString(quark), "Quarkificate") == 0);
GBL_TEST_CASE_END

GBL_TEST_CASE(constructFromString)
    auto quark = gbl::Quark("Quarkificater", 12);

GBL_TEST_CASE_END

GBL_TEST_REGISTER(constructDefault,
                  customLiteral)
