#ifndef TEST_GIMBAL_QUARK_HPP
#define TEST_GIMBAL_QUARK_HPP

#include "test_gimbal.hpp"
#include "test_gimbal_quark.h"

namespace gimbal::test {

class Quark: public UnitTestSet {
Q_OBJECT
private slots:
    GBL_QUARK_TEST_SET(GBL_TEST_CASE_QT_SLOT);
};

}



#endif // TEST_GIMBAL_QUARK_HPP
