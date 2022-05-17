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

        pOldClass_ = GBL_CONTEXT_CLASS(GblClass_createFloating(GBL_CONTEXT_TYPE));
        GblContextClass* pClass = GBL_CONTEXT_CLASS(GblClass_createFloating(GBL_CONTEXT_TYPE));

        pClass->iAllocatorIFace.pFnAlloc    = gblMemAlloc_;
        pClass->iAllocatorIFace.pFnRealloc  = gblMemRealloc_;
        pClass->iAllocatorIFace.pFnFree     = gblMemFree_;
        pClass->iLoggerIFace.pFnWrite       = gblLogWrite_;
        pClass->iLoggerIFace.pFnPush        = gblLogPush_;
        pClass->iLoggerIFace.pFnPop         = gblLogPop_;

        Exception::checkThrow(GblInstance_constructWithClass(GBL_INSTANCE(static_cast<GblContext*>(this)), GBL_CLASS(pClass)));

        GblObject_userdataSet(GBL_OBJECT(static_cast<GblContext*>(this)), this);

    }


    virtual ~Context(void) {
        GblInstance_destruct(GBL_INSTANCE(static_cast<GblContext*>(this)));
        GblClass_destroyFloating(GBL_CLASS(pOldClass_));
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

    virtual void logPush(const StackFrame& frame) {
        GBL_API_BEGIN(frame.pContext);
        GBL_API_CALL(pOldClass_->iLoggerIFace.pFnPush((GblILogger*)static_cast<GblContext*>(this), &frame));
        GBL_API_END_BLOCK();
        Exception::checkThrow(GBL_API_RESULT());
    }
    virtual void logPop(const StackFrame& frame, uint32_t count) {
        gimbal::Result result = pOldClass_->iLoggerIFace.pFnPop((GblILogger*)static_cast<GblContext*>(this), &frame, count);
        Exception::checkThrow(result);
    }
    virtual void logWrite(const StackFrame& frame, LogLevel level, const char* pFmt, va_list varArgs) {
        GBL_API_BEGIN(static_cast<GblContext*>(this));
        GBL_API_CALL(pOldClass_->iLoggerIFace.pFnWrite((GblILogger*)static_cast<GblContext*>(this), &frame, level, pFmt, varArgs));
        GBL_API_END_BLOCK();
        Exception::checkThrow(GBL_API_RESULT());
    }

    virtual void* memAlloc(const StackFrame& frame, Size size, Size alignment, const char* pDebugInfoStr) {
        void* pPtr = nullptr;
        GBL_API_BEGIN(static_cast<GblContext*>(this));
        GBL_API_CALL(pOldClass_->iAllocatorIFace.pFnAlloc((GblIAllocator*)static_cast<GblContext*>(this), &frame, size, alignment, pDebugInfoStr, &pPtr));
        GBL_API_END_BLOCK();
        Exception::checkThrow(GBL_API_RESULT());
        return pPtr;
    }

    virtual void* memRealloc(const StackFrame& frame, void* pPtr, Size newSize, Size newAlign) {
        void* pNewPtr = nullptr;
        GBL_API_BEGIN(static_cast<GblContext*>(this));
        GBL_API_CALL(pOldClass_->iAllocatorIFace.pFnRealloc((GblIAllocator*)static_cast<GblContext*>(this), &frame, pPtr, newSize, newAlign, &pNewPtr));
        GBL_API_END_BLOCK();
        Exception::checkThrow(GBL_API_RESULT());
        return pNewPtr;
    }

    virtual void memFree(const StackFrame& frame, void* pPtr) {
        GBL_API_BEGIN(static_cast<GblContext*>(this));
        GBL_API_CALL(pOldClass_->iAllocatorIFace.pFnFree((GblIAllocator*)static_cast<GblContext*>(this), &frame, pPtr));
        GBL_API_END_BLOCK();
        Exception::checkThrow(GBL_API_RESULT());
    }

    //===== overridden from std::pmr::memory_resource =====
    virtual void* do_allocate(size_t bytes, size_t align) override {
        GBL_API_BEGIN(static_cast<GblContext*>(this));
        void* pValue = nullptr;
        try {
            pValue = GBL_API_MALLOC(bytes, align, "PMR SHIT!");
        } GBL_RESULT_CATCH(GBL_API_RESULT());
        GBL_API_END_BLOCK();
        return pValue;
    }
    virtual void do_deallocate(void* pPtr, size_t bytes, size_t align) override {
        GBL_UNUSED(bytes); GBL_UNUSED(align);
        GBL_API_BEGIN(static_cast<GblContext*>(this));
        try {
            GBL_API_FREE(pPtr);
        } GBL_RESULT_CATCH(GBL_API_RESULT());
        GBL_API_END_BLOCK();
    }
    virtual bool do_is_equal(const std::pmr::memory_resource& other) const noexcept override {
        return this == &other;
    }

private:

    GblContextClass* pOldClass_ = nullptr;

    static GBL_RESULT gblLogWrite_(GblILogger* pILogger, const GblStackFrame* pFrame, GBL_LOG_LEVEL level, const char* pFmt, va_list varArgs) {
        GBL_CONTEXT_EXT_C_TO_CPP_BEGIN(pFrame, Context);
        static_cast<Context*>((GblContext*)pILogger)->logWrite(*static_cast<const StackFrame*>(pFrame),level, pFmt, varArgs);
        GBL_CONTEXT_EXT_C_TO_CPP_END();
    }
    static GBL_RESULT gblLogPush_(GblILogger* pILogger, const GblStackFrame* pFrame) {
        GBL_CONTEXT_EXT_C_TO_CPP_BEGIN(pFrame, Context)
        static_cast<Context*>((GblContext*)pILogger)->logPush(*static_cast<const StackFrame*>(pFrame));
        GBL_CONTEXT_EXT_C_TO_CPP_END();
    }

    static GBL_RESULT gblLogPop_(GblILogger* pILogger, const GblStackFrame* pFrame, uint32_t count) {
        gimbal::Result result;
        try {
            static_cast<Context*>((GblContext*)pILogger)->logPop(*static_cast<const StackFrame*>(pFrame), count);
        } catch(const Exception& resultException) {
            result = resultException.getResult();
        }
        catch(...) {
            result = Result(Result::ErrorUnhandledException);
        }
        return result;
    }

    static GBL_RESULT gblMemAlloc_(GblIAllocator* pIAllocator, const GblStackFrame* pFrame, GblSize size, GblSize align, const char* pDebugInfoStr, void** ppPtr) {
        GBL_CONTEXT_EXT_C_TO_CPP_BEGIN(pFrame, Context)
        *ppPtr = static_cast<Context*>((GblContext*)pIAllocator)->memAlloc(*static_cast<const StackFrame*>(pFrame), size, align, pDebugInfoStr);
        GBL_API_VERIFY(*ppPtr, GBL_RESULT_ERROR_MEM_ALLOC);
        GBL_CONTEXT_EXT_C_TO_CPP_END();
    }
    static GBL_RESULT gblMemRealloc_(GblIAllocator* pIAllocator, const GblStackFrame* pFrame, void* pPtr, GblSize newSize, GblSize newAlign, void** ppNewPtr) {
        GBL_CONTEXT_EXT_C_TO_CPP_BEGIN(pFrame, Context)
        *ppNewPtr = static_cast<Context*>((GblContext*)pIAllocator)->memRealloc(*static_cast<const StackFrame*>(pFrame), pPtr, newSize, newAlign);
        GBL_API_VERIFY(*ppNewPtr, GBL_RESULT_ERROR_MEM_ALLOC);
        GBL_CONTEXT_EXT_C_TO_CPP_END();
    }
    static GBL_RESULT gblMemFree_(GblIAllocator* pIAllocator, const GblStackFrame* pFrame, void* pPtr) {
        GBL_CONTEXT_EXT_C_TO_CPP_BEGIN(pFrame, Context)
        static_cast<Context*>((GblContext*)pIAllocator)->memFree(*static_cast<const StackFrame*>(pFrame), pPtr);
        GBL_CONTEXT_EXT_C_TO_CPP_END();
    }


};


}

#endif // GIMBAL_CONTEXT_HPP
