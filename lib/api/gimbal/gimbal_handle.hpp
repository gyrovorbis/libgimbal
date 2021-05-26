#ifndef GIMBAL_HANDLE_HPP
#define GIMBAL_HANDLE_HPP

#include "gimbal_handle.h"
#include "gimbal_object.hpp"



namespace gimbal {

class Context;

class Handle: public Object, public PrimitiveCompatible<GblHandle, Handle> {
public:

    Handle*         getParent(void) const {
        Handle* pHandle = nullptr;
        Result::tryThrow(gblHandleParentGet(*this, reinterpret_cast<GblHandle*>(&pHandle)));
        return pHandle;
    }
    void            setParent(Handle* pParent) const;

    Context*        getContext(void) const {
        GblContext hCtx = GBL_HANDLE_INVALID;
        Context* pCtx = nullptr;
        Result::tryThrow(gblHandleContext(*this, &hCtx));
        Result::tryThrow(gblHandleUserdata(reinterpret_cast<GblHandle>(hCtx), reinterpret_cast<void**>(&pCtx)));
        return pCtx;
    }
    void*           getUserdata(void) const {
        void* pUd = nullptr;
        Result::tryThrow(gblHandleUserdata(*this, &pUd));
        return pUd;
    }
    const CallRecord&    getLastCallRecord(void) const {
        const CallRecord* pCallRecord = nullptr;
        Result::tryThrow(gblHandleLastCallRecordGet(*this, reinterpret_cast<const GblCallRecord**>(&pCallRecord)));
        return *pCallRecord;
    }

    void clearLastCallRecord(void) const {
        Result::tryThrow(gblHandleLastCallRecordSet(*this, nullptr));
    }

};


}

#endif // GIMBAL_HANDLE_HPP
