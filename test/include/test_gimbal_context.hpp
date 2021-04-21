#ifndef TEST_GIMBAL_CONTEXT_HPP
#define TEST_GIMBAL_CONTEXT_HPP

#include <gimbal/gimbal_context.hpp>
#include <elysian_qtest.hpp>
#include <QObject>
#include <QDebug>

namespace gimbal::test {

class MonitorContext: public gimbal::Context {


};

/* TO TEST
 * 1) version
 * 2) throwing C++ exceptions from C return codes
 * 3) catching C++ exceptions as C return codes
 * 4) results result stringification + getLastError
 * 5) subclass context with validator
 * 6) test API macros for all overloads
 * 7) test C/++ handle type conversion/interop compat
 * 8) inherit from and perform base class tests on Handle/Object
 *
 *
 * OVERALL TODUZ
 * 1) finish gimbal_config.h as CMake generated file for inclusion
 * 2) Implement base/default/implicit parent std C EXT back-end
 * 3) map error-handling compiler flags to CMake options
 * 4) GblString + C++ version
 * 5) GblVariant + metatypes
 * 6) Declare C enums as table structure that can be used to autogen C, C++, and stringified values
 * 7) GblContainer
 * 8) Query/extract build info
 */
// test exception system (both sides)
class Context: public elysian::UnitTestSet {
Q_OBJECT

private:
    Context* pCtx = nullptr;
private slots:
    void version(void);
    void create(void);
   // void destroy(void);
    //set/get userdata
    //override all shit
    //handle/parent shit
    // set/get last error

};

inline void Context::version(void) {
    qDebug() << gimbal::Context::getVersion().toString().c_str();
}



inline void Context::create(void) {
    try {
        gimbal::Context* pCtx = new gimbal::Context;


        GBL_API_BEGIN((GblContext)*pCtx, "Begin fuckers");
        void* pPtr = GBL_API_MALLOC(21);
        pPtr = GBL_API_REALLOC(pPtr, 32);
        GBL_API_FREE(pPtr);
        GBL_API_END_BLOCK();

        delete pCtx;

    } catch(const std::exception& ex) {

        qDebug() << "Fucker caught: " << ex.what();
    }
}

}


#endif // TEST_GIMBAL_CONTEXT_HPP
