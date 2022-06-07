#ifndef TEST_GIMBAL_CLOSURE_HPP
#define TEST_GIMBAL_CLOSURE_HPP


#include "test_gimbal.hpp"
#include "test_gimbal_closure.h"

namespace gimbal::test {

class Closure: public UnitTestSet {
    Q_OBJECT
private:

private slots:

    GBL_CLOSURE_TEST_SET(GBL_TEST_CASE_QT_SLOT)
};

}


#endif // TEST_GIMBAL_CLOSURE_HPP
