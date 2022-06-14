#ifndef TEST_GIMBAL_FLAGS_HPP
#define TEST_GIMBAL_FLAGS_HPP

#include "test_gimbal.hpp"
#include "test_gimbal_flags.h"

namespace gimbal::test {

class Flags: public UnitTestSet {
    Q_OBJECT
private:

private slots:
    GBL_FLAGS_TEST_SET(GBL_TEST_CASE_QT_SLOT)
};

}

#endif // TEST_GIMBAL_FLAGS_HPP
