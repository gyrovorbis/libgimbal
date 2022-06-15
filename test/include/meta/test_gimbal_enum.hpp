#ifndef TEST_GIMBAL_ENUM_HPP
#define TEST_GIMBAL_ENUM_HPP

#include "test_gimbal.hpp"
#include "test_gimbal_enum.h"

namespace gimbal::test {

class Enum: public UnitTestSet {
    Q_OBJECT
private:

private slots:
    GBL_ENUM_TEST_SET(GBL_TEST_CASE_QT_SLOT)
};

}

#endif // TEST_GIMBAL_ENUM_H
