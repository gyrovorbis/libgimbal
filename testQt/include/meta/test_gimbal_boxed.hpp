#ifndef TEST_GIMBAL_BOXED_HPP
#define TEST_GIMBAL_BOXED_HPP

#include "test_gimbal.hpp"
#include "test_gimbal_boxed.h"

namespace gimbal::test {

class Boxed: public UnitTestSet {
    Q_OBJECT
private:

private slots:
    GBL_BOXED_TEST_SET(GBL_TEST_CASE_QT_SLOT)
};

}

#endif // TEST_GIMBAL_BOXED_HPP
