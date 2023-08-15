#ifndef GIMBAL_CALL_STACK_HPP
#define GIMBAL_CALL_STACK_HPP

#include "gimbal_call_stack.h"
#include "../types/gimbal_typedefs.hpp"
#include "../types/gimbal_result.hpp"
//#include "../containers/gimbal_generics.hpp"


namespace gbl {

class SourceLocation: public GblSourceLocation {
public:
    SourceLocation(const char* pFile, const char* pFunc, GblSize line, GblSize col):
        GblSourceLocation{pFile, pFunc, line, col} {}

    SourceLocation(void) {
        memset(this, 0, sizeof(SourceLocation));
    }
    SourceLocation(const GblSourceLocation& rhs) {
        memcpy(this, &rhs, sizeof(GblSourceLocation));
    }

    const char*    getFilePath(void) const { return pFile; }
    const char*    getFileName(void) const { return pFile; }
    const char*    getFunctionName(void) const { return pFunc; }
    GblSize        getLineNumber(void) const { return line; }
    GblSize        getColumn(void) const { return column; }

    bool                isValid(void) const;
    std::string         toPrettyString(void) const;

    //C++20 std::source_location compat
    //comparison operators (check if further in same file/function)

};

GBL_CHECK_C_CPP_TYPE_COMPAT(SourceLocation, GblSourceLocation);


class CallRecord: public GblCallRecord {
public:

    template<typename R>
        requires (std::is_constructible_v<Result, R> && !std::same_as<Result, R>)
    CallRecord(R resultValue, const char* pMessage=nullptr, GblObject* pObject=nullptr, SourceLocation srcLoc={}) noexcept:
        CallRecord(Result(resultValue), pMessage, pObject, std::move(srcLoc)) {}

    CallRecord(Result result, const char* pMessage=nullptr, GblObject* pObject=nullptr, SourceLocation srcLoc={}) noexcept;

    CallRecord(const GblCallRecord& other) {
        memcpy(this, &other, sizeof(GblCallRecord));
    }

    Handle*             getHandle(void) const;
    Result                getResult(void) const { return this->result; }
    const SourceLocation&          getSource(void) const {
        return static_cast<const SourceLocation&>(srcLocation);
    }


    const char*         getMessage(void) const { return message; }
    std::string         getSourceString(void) const;

    std::string         toString(void) const;

};

GBL_CHECK_C_CPP_TYPE_COMPAT(CallRecord, GblCallRecord);



class StackFrame: public GblStackFrame {
public:
                StackFrame(GblObject* pObject=nullptr, GBL_RESULT initialResult=GBL_RESULT_UNKNOWN, SourceLocation entryLoc=SourceLocation()) {
                    GBL_API_STACK_FRAME_CONSTRUCT(this, pObject, initialResult, entryLoc);

                }
                //StackFrame(const GblStackFrame& rhs);
/*
                StackFrame& operator=(const GblStackFrame& rhs) {
                    memcpy(this, &rhs, sizeof(GblStackFrame));
                    return *this;
                }
*/
    auto        getSourceCurrent(void) const -> const SourceLocation&;
    auto        getSourceEntry(void) const -> const SourceLocation& { return *static_cast<const SourceLocation*>(&sourceEntry);}
    auto        getCallRecord(void) const -> const CallRecord& { return *static_cast<const CallRecord*>(&record); }
    auto        getCallRecord(void) -> CallRecord&;
    GblObject*  getObject(void) const;
    GblContext* getContext(void) const;
    void*       getHandleUd(void) const;
    void*       getContextUd(void) const;

    bool        isValid(void) const;
};

GBL_CHECK_C_CPP_TYPE_COMPAT(StackFrame, GblStackFrame);


inline CallRecord::CallRecord(Result result, const char* pMessage, GblObject* pObject, SourceLocation srcLoc) noexcept {
    GBL_CALL_RECORD_CONSTRUCT(this, pObject, result.getCode(), srcLoc, "%s", pMessage? pMessage : result.toString());
}



}

#endif // GIMBAL_CALL_STACK_HPP
