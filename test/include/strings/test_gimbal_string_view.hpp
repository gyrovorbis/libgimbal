#ifndef TEST_GIMBAL_STRING_VIEW_HPP
#define TEST_GIMBAL_STRING_VIEW_HPP

#include "test_gimbal.hpp"
#include "test_gimbal_string_view.h"

namespace gimbal::test {

class StringView: public UnitTestSet {
    Q_OBJECT
private:

private slots:

    GBL_STRING_VIEW_TEST_SET(GBL_TEST_CASE_QT_SLOT)
};

}

#endif // TEST_GIMBAL_STRING_VIEW_HPP
