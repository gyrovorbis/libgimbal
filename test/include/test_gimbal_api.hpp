#ifndef TEST_GIMBAL_API_HPP
#define TEST_GIMBAL_API_HPP

#include <gimbal/core/gimbal_api_frame.hpp>
#include <elysian_qtest.hpp>
#include <concepts>
#include <limits>

namespace gimbal::test {


//error strings?
class Api: public elysian::UnitTestSet {
Q_OBJECT
private:

    void verifyPow2NextTyped_(uint8_t base, uint64_t expected) {
        const decltype(base) pow2 = GBL_POW2_NEXT_UINT8(base);
        QCOMPARE(pow2, expected);
    }

    void verifyPow2NextTyped_(uint16_t base, uint64_t expected) {
        const decltype(base) pow2 = GBL_POW2_NEXT_UINT16(base);
        QCOMPARE(pow2, expected);
    }

    void verifyPow2NextTyped_(uint32_t base, uint64_t expected) {
        const decltype(base) pow2 = GBL_POW2_NEXT_UINT32(base);
        QCOMPARE(pow2, expected);
    }

    void verifyPow2NextTyped_(uint64_t base, uint64_t expected) {
        const decltype(base) pow2 = GBL_POW2_NEXT_UINT64(base);
        QCOMPARE(pow2, expected);
    }

    template<typename T>
    void verifyPow2NextGeneric_(T base, uint64_t expected) {
        if(expected <= std::numeric_limits<uint8_t>::max())
            verifyPow2NextTyped_(static_cast<uint8_t>(base), expected);
        if(expected <= std::numeric_limits<uint16_t>::max())
            verifyPow2NextTyped_(static_cast<uint16_t>(base), expected);
        if(expected <= std::numeric_limits<uint32_t>::max())
            verifyPow2NextTyped_(static_cast<uint32_t>(base), expected);
        if(expected <= std::numeric_limits<uint64_t>::max())
            verifyPow2NextTyped_(static_cast<uint64_t>(base), expected);
    }

    template<typename T>
    void verifyPow2Next_(T base, uint64_t expected) {
        if(base > 1) verifyPow2NextGeneric_(base-1, base);
        verifyPow2NextGeneric_(base, base);
        verifyPow2NextGeneric_(base+1, expected);
    }



private slots:
    void pow2Next(void);
};

inline void Api::pow2Next(void) {
    for(uint64_t b = 2; b < 63; ++b) {
        const uint64_t value = (static_cast<uint64_t>(1u) << b);
        verifyPow2Next_(value, value << 1u);
    }
}

}

#endif // TEST_GIMBAL_API_HPP
