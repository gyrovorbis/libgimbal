#ifndef GIMBAL_EXCEPTION_HPP
#define GIMBAL_EXCEPTION_HPP

#include <exception>
#include <concepts>
#include "../core/gimbal_call_stack.hpp"
#include "../core/gimbal_api_generators.hpp"
#include "../types/gimbal_result.hpp"
#include "../core/gimbal_api_frame.h"

namespace gbl {

class Exception: public CallRecord {
public:


    using CallRecord::CallRecord;
    Exception(const CallRecord& record) noexcept:
        CallRecord(record) {}
    Exception(CallRecord&& record) noexcept:
        CallRecord(std::move(record)) {}

    virtual const char* what(void) const noexcept {
        return getMessage();
    }

    virtual const std::exception&   asStdException(void) const = 0;
    virtual std::exception&         asStdException(void) = 0;

    static const CallRecord& throwException(const CallRecord& record);

    static const CallRecord& checkThrow(const CallRecord& record) {
        if(record.getResult().isError()) {
            GBL_API_CLEAR_LAST_RECORD();
            return throwException(record);
        }
        return record;
    }

    static const CallRecord& checkThrow(std::invocable auto fn) {
        const GblCallRecord* pRecord = nullptr;
        GblThread_callRecordSet(NULL, NULL);
        fn();
        pRecord = GblThread_callRecord(NULL);
        return Exception::checkThrow(*pRecord);
    }


    static CallRecord tryCatchRecord(std::invocable auto fn, SourceLocation loc=SourceLocation(SRC_FILE, nullptr, SRC_LN, SRC_COL)) noexcept;

    class TryBlock {
    private:
        CallRecord record_;
    public:

        TryBlock(SourceLocation src=SourceLocation(SRC_FILE, "TryBlock::TryBlock()", SRC_LN, SRC_COL)):
            record_(Result::Success, "Success", nullptr, src) {}

        TryBlock& operator=(std::invocable auto fn) {
            record_ = Exception::tryCatchRecord(std::forward<decltype(fn)>(fn), std::move(getRecord().getSource()));
            return *this;
        }

        const CallRecord& getRecord(void) const { return record_; }
        Result getResult(void) const { return getRecord().getResult(); }
        const SourceLocation& getSource(void) const {return getRecord().getSource(); }
        const char* getMessage(void) const { return getRecord().getMessage(); }

        operator bool() const { return getRecord().getResult().isSuccess(); }
    };
};


template<typename StdType>
class StdException: public Exception, public StdType {
public:

    template<typename V>
    requires std::is_constructible_v<CallRecord,V> && (std::is_default_constructible_v<StdType> &&
        !std::is_constructible_v<StdType, const char*>)
    StdException(V&& v) noexcept:
        Exception(std::move(v)) {}

    template<typename V1, typename V2>
    requires std::is_constructible_v<CallRecord, V1> && std::is_constructible_v<StdType, V2>
    StdException(V1&& v1, V2&& v2) noexcept:
        Exception(std::move(v1)),
        StdType(std::forward<V2>(v2)) {}

    template<typename V>
    requires std::is_constructible_v<CallRecord,V> && std::is_constructible_v<StdType, const char*>
    StdException(V&& v) noexcept:
        Exception(std::move(v)),
        StdType(getMessage()) {}

    virtual ~StdException(void) override = default;

    virtual const char* what(void) const noexcept override {
        return Exception::what();
    }

    virtual const std::exception& asStdException(void) const override {
        return *this;
    }

    virtual std::exception& asStdException(void) override {
        return *this;
    }
};


inline CallRecord Exception::tryCatchRecord(std::invocable auto fn, SourceLocation loc) noexcept {
    try {
        fn();
    } catch(const Exception& gblExcept) {
        return gblExcept;
    } catch(const std::underflow_error& ex) {
        return { Result::ErrorUnderflow, ex.what() };
    } catch(const std::overflow_error& ex) {
        return { Result::ErrorOverflow, ex.what() };
    } catch(const std::out_of_range& ex) {
        return { Result::ErrorOutOfRange, ex.what() };
    } catch(const std::invalid_argument& ex) {
        return { Result::ErrorInvalidArg, ex.what() };
    } catch(const std::bad_alloc& ex) {
        return { Result::ErrorMemAlloc, ex.what() };
    } catch(const std::exception& stdEx) {
        return { Result::ErrorUnhandledException, stdEx.what() };
    } catch(...) {
        return { Result::ErrorUnhandledException, "Unknown Exception Type!" };
    }
    return Result::Success;
}

inline const CallRecord& Exception::throwException(const CallRecord& record) {
    switch(record.getResult().getValue()) {
    case Result::ErrorUnderflow:    throw StdException<std::underflow_error>(record);
    case Result::ErrorOverflow:     throw StdException<std::overflow_error>(record);
    case Result::ErrorOutOfRange:   throw StdException<std::out_of_range>(record);
    case Result::ErrorInvalidArg:   throw StdException<std::invalid_argument>(record);
    case Result::ErrorMemAlloc:
    case Result::ErrorMemRealloc:   throw StdException<std::bad_alloc>(record);
    default:                        throw StdException<std::exception>(record);
    }
    return record;
}



}


#endif // GIMBAL_EXCEPTION_HPP
