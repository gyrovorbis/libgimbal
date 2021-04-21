#ifndef TEST_GIMBAL_API_HPP
#define TEST_GIMBAL_API_HPP

#include <gimbal/gimbal_api.h>

namespace gimbal::test {


//error strings?
class ApiTest: public QObject {

public slots:
    void test1();
    void test2();
    void test3();
};

}

#endif // TEST_GIMBAL_API_HPP
