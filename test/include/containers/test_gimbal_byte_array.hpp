#ifndef TEST_GIMBAL_BYTE_ARRAY_HPP
#define TEST_GIMBAL_BYTE_ARRAY_HPP

#include "test_gimbal.hpp"
#include "test_gimbal_byte_array.h"

namespace gimbal::test {

class ByteArray: public UnitTestSet {
    Q_OBJECT
private:

private slots:

    GBL_BYTE_ARRAY_TEST_SET(GBL_TEST_CASE_QT_SLOT)
};

}

#endif // TEST_GIMBAL_BYTE_ARRAY_HPP
