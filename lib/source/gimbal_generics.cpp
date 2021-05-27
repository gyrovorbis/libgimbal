#include <gimbal/gimbal_generics.hpp>
#include <gimbal/gimbal_handle.hpp>

namespace gimbal {

CallRecord::CallRecord(Result result, const char* pMessage, Handle* pHandle, SourceLocation srcLoc) noexcept {
    GBL_CALL_RECORD_CONSTRUCT(this, pHandle? static_cast<GblHandle>(*pHandle) : GBL_HANDLE_INVALID, result.getCode(), srcLoc, "%s", pMessage? pMessage : result.toString().data());
}


const CallRecord& Exception::throwException(const CallRecord& record) {
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
