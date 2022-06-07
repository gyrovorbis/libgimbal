#ifndef TEST_GIMBAL_REF_HPP
#define TEST_GIMBAL_REF_HPP

#include "test_gimbal.hpp"
#include "test_gimbal_ref.h"

namespace gimbal::test {

class Ref: public UnitTestSet {
    Q_OBJECT
private:

private slots:

    GBL_REF_TEST_SET(GBL_TEST_CASE_QT_SLOT)
};

}

#endif // TEST_GIMBAL_REF_HPP
