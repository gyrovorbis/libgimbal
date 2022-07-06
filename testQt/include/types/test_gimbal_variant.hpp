#ifndef VARIANT_TESTS_HPP
#define VARIANT_TESTS_HPP

#include "test_gimbal.hpp"
#include <gimbal/types/gimbal_variant.h>
#include <gimbal/meta/gimbal_type.hpp>
#include "test_gimbal_variant.h"

namespace gimbal::test {

class Variant: public UnitTestSet {
private slots:

    GBL_VARIANT_TEST_SET(GBL_TEST_CASE_QT_SLOT)
};


}

#endif // VARIANT_TESTS_HPP
