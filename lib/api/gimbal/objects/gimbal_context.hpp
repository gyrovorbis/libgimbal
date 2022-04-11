#ifndef GIMBAL_CONTEXT_HPP
#define GIMBAL_CONTEXT_HPP

#include "gimbal_context.h"
#include "gimbal_handle.hpp"
#include "../core/gimbal_api_frame.hpp"
#include "../types/gimbal_version.hpp"
#include <iostream>
#include <memory_resource>

namespace gimbal {

#define GBL_RESULT_CATCH(code) \
catch(const Exception& resultException) {     \
    code = resultException.getResult();  \
}   \
catch(...) {    \
    code = Result(Result::ErrorUnhandledException);  \
}

#define GBL_CONTEXT_EXT_C_TO_CPP_BEGIN_NO_THROW(pFrame, UdType) \
    GBL_ASSERT(pFrame); \
    GBL_API_BEGIN(pFrame->hContext);    \
    UdType* pUd = static_cast<UdType*>(pFrame->pContextUd); \
    GBL_API_VERIFY_POINTER(pUd)

#define GBL_CONTEXT_EXT_C_TO_CPP_BEGIN(pFrame, UdType) \
    gimbal::Result result = GBL_RESULT_SUCCESS; \
    GBL_UNUSED(result); \
    GBL_CONTEXT_EXT_C_TO_CPP_BEGIN_NO_THROW(pFrame, Context); \
    try { \

#define GBL_CONTEXT_EXT_C_TO_CPP_END()   \
    } GBL_RESULT_CATCH(GBL_API_RESULT()); \
    GBL_API_END();

class Context:
        public Handle,
        public PrimitiveCompatible<GblContext, Context>,
        public std::pmr::memory_resource
{
public:

    static Context* fromHandle(GblContext hCtx) {
        Context* pCtx = nullptr;
        if(hCtx) Exception::checkThrow(gblHandleUserdata((GblHandle)hCtx, (void**)&pCtx));
        return pCtx;
    }

    Context(void) {

        const GblContextExtLog log = {
            &gblLogWrite_,
            &gblLogPush_,
            &gblLogPop_
        };

        // CONSIDER OPTIONALLY TAKING A PMR INSTANCE FOR HANDLING ALLOCATION
        const GblContextExtMem mem = {
            &gblMemAlloc_,
            &gblMemRealloc_,
            &gblMemFree_
        };

        const GblContextCreateInfo info = {
            .versionMin = Version(),
            .handleInfo = {
                .objectInfo = {
                    .pExtraSpaceInfo = nullptr
                },
                .hParent = GBL_HANDLE_INVALID,
                .pUserdata = this
            },
            .pExtLog = &log,
            .pExtMem = &mem,
        };

        Exception::checkThrow(gblContextCreate(reinterpret_cast<GblContext*>(getPrimitiveAddress()), &info));
    }

    using PrimitiveCompatible<GblContext, Context>::operator&;

    virtual ~Context(void) {
        Exception::checkThrow(gblContextDestroy(*this));
    }

    Context* getParentContext(void) const; //use metatype info and getParentHandle

    static Version getVersion(void) {
        Version version;
        Exception::checkThrow(gblContextVersion(&version, nullptr));
        return version;
    }


    const CallRecord&    getCallRecord(void) const {
        const CallRecord* pCallRecord = nullptr;
        Exception::checkThrow(gblContextCallRecordGet(*this, reinterpret_cast<const GblCallRecord**>(&pCallRecord)));
        return *pCallRecord;
    }

    void clearCallRecord(void) const {
        Exception::checkThrow(gblContextCallRecordSet(*this, nullptr));
    }


    // ===== overriding GblContext C API user callbacks =====

    virtual void    logPush(const StackFrame& frame) { Exception::throwException(GBL_RESULT_UNIMPLEMENTED); }
    virtual void    logPop(const StackFrame& frame, uint32_t count) { Exception::throwException(GBL_RESULT_UNIMPLEMENTED); }
    virtual void    logWrite(const StackFrame& frame, LogLevel level, const char* pFmt, va_list varArgs) { Exception::throwException(GBL_RESULT_UNIMPLEMENTED); }

    virtual void*   memAlloc(const StackFrame& frame, Size size, Size alignment, const char* pDebugInfoStr) { Exception::throwException(GBL_RESULT_UNIMPLEMENTED); return nullptr;}
    virtual void*   memRealloc(const StackFrame& frame, void* pPtr, Size newSize, Size newAlign) { Exception::throwException(GBL_RESULT_UNIMPLEMENTED); return nullptr; }
    virtual void    memFree(const StackFrame& frame, void* pPtr) { Exception::throwException(GBL_RESULT_UNIMPLEMENTED); }

    //===== overridden from std::pmr::memory_resource =====
    virtual void*   do_allocate(size_t bytes, size_t align) override {
        GBL_API_BEGIN(*this);
        void* pValue = nullptr;
        try {
            pValue = GBL_API_MALLOC(bytes, align, "PMR SHIT!");
        } GBL_RESULT_CATCH(GBL_API_RESULT());
        GBL_API_END_BLOCK();
        return pValue;
    }
    virtual void    do_deallocate(void* pPtr, size_t bytes, size_t align) override {
        GBL_UNUSED(bytes); GBL_UNUSED(align);
        GBL_API_BEGIN(*this);
        try {
            GBL_API_FREE(pPtr);
        } GBL_RESULT_CATCH(GBL_API_RESULT());
        GBL_API_END_BLOCK();
    }
    virtual bool    do_is_equal(const std::pmr::memory_resource& other) const noexcept override {
        return this == &other;
    }

private:

    static GBL_RESULT gblLogWrite_     (const GblStackFrame* pFrame, GBL_LOG_LEVEL level, const char* pFmt, va_list varArgs) {
        GBL_CONTEXT_EXT_C_TO_CPP_BEGIN(pFrame, Context);
        pUd->logWrite(*static_cast<const StackFrame*>(pFrame),level, pFmt, varArgs);
        GBL_CONTEXT_EXT_C_TO_CPP_END();
    }
    static GBL_RESULT gblLogPush_      (const GblStackFrame* pFrame) {
        GBL_CONTEXT_EXT_C_TO_CPP_BEGIN(pFrame, Context)
        pUd->logPush(*static_cast<const StackFrame*>(pFrame));
        GBL_CONTEXT_EXT_C_TO_CPP_END();
    }

    static GBL_RESULT gblLogPop_       (const GblStackFrame* pFrame, uint32_t count) {
        GBL_CONTEXT_EXT_C_TO_CPP_BEGIN(pFrame, Context)
        pUd->logPop(*static_cast<const StackFrame*>(pFrame), count);
        GBL_CONTEXT_EXT_C_TO_CPP_END();
    }

    static GBL_RESULT gblMemAlloc_     (const GblStackFrame* pFrame, GblSize size, GblSize align, const char* pDebugInfoStr, void** ppPtr) {
        GBL_CONTEXT_EXT_C_TO_CPP_BEGIN(pFrame, Context)
        *ppPtr = pUd->memAlloc(*static_cast<const StackFrame*>(pFrame), size, align, pDebugInfoStr);
        GBL_API_VERIFY(*ppPtr, GBL_RESULT_ERROR_MEM_ALLOC);
        GBL_CONTEXT_EXT_C_TO_CPP_END();
    }
    static GBL_RESULT gblMemRealloc_   (const GblStackFrame* pFrame, void* pPtr, GblSize newSize, GblSize newAlign, void** ppNewPtr) {
        GBL_CONTEXT_EXT_C_TO_CPP_BEGIN(pFrame, Context)
        *ppNewPtr = pUd->memRealloc(*static_cast<const StackFrame*>(pFrame), pPtr, newSize, newAlign);
        GBL_API_VERIFY(*ppNewPtr, GBL_RESULT_ERROR_MEM_ALLOC);
        GBL_CONTEXT_EXT_C_TO_CPP_END();
    }
    static GBL_RESULT gblMemFree_      (const GblStackFrame* pFrame, void* pPtr) {
        GBL_CONTEXT_EXT_C_TO_CPP_BEGIN(pFrame, Context)
        pUd->memFree(*static_cast<const StackFrame*>(pFrame), pPtr);
        GBL_CONTEXT_EXT_C_TO_CPP_END();
    }


};


}

#endif // GIMBAL_CONTEXT_HPP
