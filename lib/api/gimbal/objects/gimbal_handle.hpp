#ifndef GIMBAL_HANDLE_HPP
#define GIMBAL_HANDLE_HPP

#include "gimbal_handle.h"
#include "gimbal_object.hpp"
#include "../core/gimbal_call_stack.hpp"
#include "../types/gimbal_exception.hpp"

namespace gimbal {

class Context;

class Handle: public Object, public PrimitiveCompatible<GblHandle, Handle> {
public:

    Handle*         getParent(void) const {
        Handle* pHandle = nullptr;
        Exception::checkThrow(gblHandleParentGet(*this, reinterpret_cast<GblHandle*>(&pHandle)));
        return pHandle;
    }
    void            setParent(Handle* pParent) const;

    Context*        getContext(void) const {
        GblContext hCtx = GBL_HANDLE_INVALID;
        Context* pCtx = nullptr;
        Exception::checkThrow(gblHandleContext((GblHandle)*this, &hCtx));
        Exception::checkThrow(gblHandleUserdata(reinterpret_cast<GblHandle>(hCtx), reinterpret_cast<void**>(&pCtx)));
        return pCtx;
    }
    void*           getUserdata(void) const {
        void* pUd = nullptr;
        Exception::checkThrow(gblHandleUserdata(*this, &pUd));
        return pUd;
    }
    /*
    const CallRecord&    getCallRecord(void) const {
        const CallRecord* pCallRecord = nullptr;
        Exception::checkThrow(gblHandleLastCallRecordGet(*this, reinterpret_cast<const GblCallRecord**>(&pCallRecord)));
        return *pCallRecord;
    }

    void clearLastCallRecord(void) const {
        Exception::checkThrow(gblHandleLastCallRecordSet(*this, nullptr));
    }
    */

};


}

#endif // GIMBAL_HANDLE_HPP
