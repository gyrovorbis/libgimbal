#ifndef TEST_GIMBAL_LINKED_LIST_HPP
#define TEST_GIMBAL_LINKED_LIST_HPP

#include "test_gimbal.hpp"
#include "test_gimbal_linked_list.h"

namespace gimbal::test {

class LinkedList: public UnitTestSet {
    Q_OBJECT
private:

private slots:

    GBL_LINKED_LIST_TEST_SET(GBL_TEST_CASE_QT_SLOT)
};

}


#endif // TEST_GIMBAL_LINKED_LIST_HPP
