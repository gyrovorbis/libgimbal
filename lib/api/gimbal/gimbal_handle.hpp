#ifndef GIMBAL_HANDLE_HPP
#define GIMBAL_HANDLE_HPP

#include "gimbal_handle.h"
#include "gimbal_object.hpp"



namespace gimbal {

class Context;

class Handle: public Object, public PrimitiveCompatible<GblHandle, Handle> {
public:

    Handle*         getParent(void) const;
    void            setParent(Handle* pParent) const;
    Context*        getContext(void) const;
    void*           getUserdata(void) const;
    GblApiResult    getLastError(void) const;

};


}

#endif // GIMBAL_HANDLE_HPP
