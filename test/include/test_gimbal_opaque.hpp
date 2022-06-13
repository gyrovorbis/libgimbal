#ifndef TEST_GIMBAL_OPAQUE_HPP
#define TEST_GIMBAL_OPAQUE_HPP

#include "test_gimbal.hpp"
#include "test_gimbal_opaque.h"

namespace gimbal::test {

class Opaque: public UnitTestSet {
    Q_OBJECT
private:

private slots:
    GBL_OPAQUE_TEST_SET(GBL_TEST_CASE_QT_SLOT)
};

}

#endif // TEST_GIMBAL_OPAQUE_HPP
