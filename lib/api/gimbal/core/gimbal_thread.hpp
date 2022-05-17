#ifndef GIMBAL_THREAD_HPP
#define GIMBAL_THREAD_HPP

#include "gimbal_thread.h"
#include "gimbal_api_generators.hpp"

namespace gimbal {

class Thread:
    public PrimitiveCompatible<GblThread*, Thread> {
public:



    static Thread current(void);

};


// ======= inlinez =======

inline Thread Thread::current(void) {
    Thread thd;
    GblThread_current(&thd);
    return thd;
}



}

#endif // GIMBAL_THREAD_HPP
