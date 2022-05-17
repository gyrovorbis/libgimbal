#ifndef TEST_GIMBAL_BYTE_ARRAY_HPP
#define TEST_GIMBAL_BYTE_ARRAY_HPP


#include <gimbal/types/gimbal_byte_array.hpp>
#include "test_gimbal.hpp"


namespace gimbal::test {

//error strings?
class ByteArray: public UnitTestSet {
Q_OBJECT
private:

private slots:
    void init(void);
    void constructDefault(void);
    void constructBytes(void);
    void constructData(void);
    void constructCopy(void);
    void constructMove(void);
    void assignCopy(void);
    void assignMove(void);
    void acquireRelease(void);
    void clear(void);
    void resize(void);
    void grow(void);
    void shrink(void);
    void erase(void);
    void insert(void);
    void append(void);
    void prepend(void);
    void readValue(void);
    void writeValue(void);
};

}

#endif // TEST_GIMBAL_ByteArray_HPP
