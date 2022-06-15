#ifndef TEST_GIMBAL_ARRAY_MAP_HPP
#define TEST_GIMBAL_ARRAY_MAP_HPP

#include "test_gimbal.hpp"
#include "test_gimbal_array_map.h"

namespace gimbal::test {

class ArrayMap: public UnitTestSet {
    Q_OBJECT
private:

private slots:

    GBL_ARRAY_MAP_TEST_SET(GBL_TEST_CASE_QT_SLOT)
};

}

#endif // TEST_GIMBAL_ARRAY_MAP_HPP
