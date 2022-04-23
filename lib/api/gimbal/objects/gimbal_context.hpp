#ifndef GIMBAL_CONTEXT_HPP
#define GIMBAL_CONTEXT_HPP

#include "gimbal_context.h"
#include "../core/gimbal_api_frame.hpp"
#include "../types/gimbal_version.hpp"
#include "gimbal/core/gimbal_call_stack.hpp"
#include "../types/gimbal_exception.hpp"
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
    GBL_API_BEGIN(pFrame->pContext);    \
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
        public GblContext,
        public std::pmr::memory_resource
{
public:

    static Context* fromGblObj(GblContext* pContext) {
        return pContext ?
                    static_cast<Context*>(GblObject_userdata(GBL_OBJECT(pContext)))
                                          : NULL;
    }

    Context(void) {

        GblContextClass* pClass = GBL_CONTEXT_CLASS(gblTypeClassCreateFloating(GBL_TYPE_CONTEXT));

        pClass->iAllocatorIFace.pFnAlloc = gblMemAlloc_;
        pClass->iAllocatorIFace.pFnRealloc = gblMemRealloc_;
        pClass->iAllocatorIFace.pFnFree = gblMemFree_;
        pClass->iLoggerIFace.pFnWrite = gblLogWrite_;
        pClass->iLoggerIFace.pFnPush = gblLogPush_;
        pClass->iLoggerIFace.pFnPop = gblLogPop_;

        Exception::checkThrow(gblTypeInstanceConstructWithClass(GBL_INSTANCE(static_cast<GblContext*>(this)), GBL_CLASS(pClass)));
        GblObject_userdataSet(GBL_OBJECT(static_cast<GblContext*>(this)), this);

    }


    virtual ~Context(void) {
        gblTypeInstanceDestruct(GBL_INSTANCE(static_cast<GblContext*>(this)));
    }


    static Version getVersion(void) {
        Version version;
        //Exception::checkThrow(gblContextVersion(&version, nullptr));
        return version;
    }


    const CallRecord&    getCallRecord(void) const {
        return *static_cast<const CallRecord*>(&this->lastIssue);
    }

    void clearCallRecord(void) {
        Exception::checkThrow(GblContext_lastIssueClear(this));
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
        GBL_API_BEGIN(static_cast<GblContext*>(this));
        void* pValue = nullptr;
        try {
            pValue = GBL_API_MALLOC(bytes, align, "PMR SHIT!");
        } GBL_RESULT_CATCH(GBL_API_RESULT());
        GBL_API_END_BLOCK();
        return pValue;
    }
    virtual void    do_deallocate(void* pPtr, size_t bytes, size_t align) override {
        GBL_UNUSED(bytes); GBL_UNUSED(align);
        GBL_API_BEGIN(static_cast<GblContext*>(this));
        try {
            GBL_API_FREE(pPtr);
        } GBL_RESULT_CATCH(GBL_API_RESULT());
        GBL_API_END_BLOCK();
    }
    virtual bool    do_is_equal(const std::pmr::memory_resource& other) const noexcept override {
        return this == &other;
    }

private:

    static GBL_RESULT gblLogWrite_     (GblILogger* pILogger, const GblStackFrame* pFrame, GBL_LOG_LEVEL level, const char* pFmt, va_list varArgs) {
        GBL_CONTEXT_EXT_C_TO_CPP_BEGIN(pFrame, Context);
        pUd->logWrite(*static_cast<const StackFrame*>(pFrame),level, pFmt, varArgs);
        GBL_CONTEXT_EXT_C_TO_CPP_END();
    }
    static GBL_RESULT gblLogPush_      (GblILogger* pILogger, const GblStackFrame* pFrame) {
        GBL_CONTEXT_EXT_C_TO_CPP_BEGIN(pFrame, Context)
        pUd->logPush(*static_cast<const StackFrame*>(pFrame));
        GBL_CONTEXT_EXT_C_TO_CPP_END();
    }

    static GBL_RESULT gblLogPop_       (GblILogger* pILogger, const GblStackFrame* pFrame, uint32_t count) {
        GBL_CONTEXT_EXT_C_TO_CPP_BEGIN(pFrame, Context)
        pUd->logPop(*static_cast<const StackFrame*>(pFrame), count);
        GBL_CONTEXT_EXT_C_TO_CPP_END();
    }

    static GBL_RESULT gblMemAlloc_     (GblIAllocator* pIAllocator, const GblStackFrame* pFrame, GblSize size, GblSize align, const char* pDebugInfoStr, void** ppPtr) {
        GBL_CONTEXT_EXT_C_TO_CPP_BEGIN(pFrame, Context)
        *ppPtr = pUd->memAlloc(*static_cast<const StackFrame*>(pFrame), size, align, pDebugInfoStr);
        GBL_API_VERIFY(*ppPtr, GBL_RESULT_ERROR_MEM_ALLOC);
        GBL_CONTEXT_EXT_C_TO_CPP_END();
    }
    static GBL_RESULT gblMemRealloc_   (GblIAllocator* pIAllocator, const GblStackFrame* pFrame, void* pPtr, GblSize newSize, GblSize newAlign, void** ppNewPtr) {
        GBL_CONTEXT_EXT_C_TO_CPP_BEGIN(pFrame, Context)
        *ppNewPtr = pUd->memRealloc(*static_cast<const StackFrame*>(pFrame), pPtr, newSize, newAlign);
        GBL_API_VERIFY(*ppNewPtr, GBL_RESULT_ERROR_MEM_ALLOC);
        GBL_CONTEXT_EXT_C_TO_CPP_END();
    }
    static GBL_RESULT gblMemFree_      (GblIAllocator* pIAllocator, const GblStackFrame* pFrame, void* pPtr) {
        GBL_CONTEXT_EXT_C_TO_CPP_BEGIN(pFrame, Context)
        pUd->memFree(*static_cast<const StackFrame*>(pFrame), pPtr);
        GBL_CONTEXT_EXT_C_TO_CPP_END();
    }


};


}

#endif // GIMBAL_CONTEXT_HPP
