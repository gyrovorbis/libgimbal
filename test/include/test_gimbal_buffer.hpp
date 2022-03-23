#ifndef TEST_GIMBAL_BUFFER_HPP
#define TEST_GIMBAL_BUFFER_HPP


#include <gimbal/types/gimbal_buffer.hpp>
#include "test_gimbal.hpp"


namespace gimbal::test {


//error strings?
class Buffer: public UnitTestSet {
Q_OBJECT
private:

private slots:

    void constructDefault(void);
};

inline void Buffer::constructDefault(void) {
    gimbal::Buffer buffer;

    QVERIFY(buffer.isEmpty());
    QCOMPARE(buffer.getSize(), 0);
    QCOMPARE(buffer.getData(), nullptr);
    QCOMPARE(buffer.getContext(), nullptr);
}

}

#endif // TEST_GIMBAL_BUFFER_HPP
