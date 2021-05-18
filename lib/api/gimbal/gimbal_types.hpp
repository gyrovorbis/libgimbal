#ifndef GIMBAL_TYPES_HPP
#define GIMBAL_TYPES_HPP

#include "gimbal_api.h"
#include "gimbal_object.h"
#include "gimbal_generics.hpp"
#include <type_traits>
#include <string_view>
#include <stdexcept>
#include <cstring>
#include <string>


namespace gimbal {

    using Bool      = ::GblBool;
    using Size      = ::GblSize;
    using Real      = ::GblReal;
    using Int       = ::GblInt;
    using Enum      = ::GblEnum;
    using Hash      = ::GblHash;


GBL_ENUM_TABLE_DECLARE_CPP(GBL_META_LOG_LEVEL_TABLE);

GBL_ENUM_TABLE_DECLARE_CPP(GBL_META_OBJECT_TYPE_TABLE);

    class Version: public PrimitiveBase<GblVersion> {
    public:
        Version(void) = default;
        Version(GblVersion version): PrimitiveBase(version) {}
        Version(GblVersionInfo info): Version(GBL_VERSION_MAKE(info.major, info.minor, info.patch)) {}
        Version(uint8_t major, uint16_t minor, uint8_t patch): Version(GblVersionInfo{ major, minor, patch }) {}
        Version(std::string_view string);

        constexpr uint8_t   getMajor(void) const { return GBL_VERSION_EXTRACT_MAJOR(*this); }
        constexpr uint16_t  getMinor(void) const { return GBL_VERSION_EXTRACT_MINOR(*this); }
        constexpr uint8_t   getPatch(void) const { return GBL_VERSION_EXTRACT_PATCH(*this); }

        constexpr GblVersion getValue(void) const { return getPrimitiveValue(); }
        constexpr GblVersionInfo getInfo(void) const {
            return { getMajor(), getMinor(), getPatch() };
        }

        std::string toString(void) const {
            char buffer[GBL_VERSION_STRING_SIZE_MAX] = { 0 };
            const auto info = getInfo();
            const GBL_RESULT result = gblVersionInfoString(&info, buffer, sizeof(buffer));
           // GBL_ASSERT(GBL_RESULT_SUCCESS(result));
            return buffer;
        }


        //overload spaceship operator
    };

    GBL_CHECK_C_CPP_TYPE_COMPAT(Version, GblVersion);

    class SourceLocation: public GblSourceLocation {
    public:
        SourceLocation(void);
        SourceLocation(const GblSourceLocation& rhs) {
            memcpy(this, &rhs, sizeof(GblSourceLocation));
        }

        std::string_view    getFilePath(void) const { return pFile; }
        std::string_view    getFileName(void) const { return pFile; }
        std::string_view    getFunctionName(void) const { return pFunc; }
        GblSize             getLineNumber(void) const { return line; }
        GblSize             getColumn(void) const { return column; }

        bool                isValid(void) const;
        std::string         toPrettyString(void) const;

        //C++20 std::source_location compat
        //comparison operators (check if further in same file/function)

    };

    GBL_CHECK_C_CPP_TYPE_COMPAT(SourceLocation, GblSourceLocation);

    class ApiResult: public GblApiResult {
    public:
        ApiResult(GblHandle hHandle, GBL_RESULT result, std::string_view message={}) {
            resultCode = result;
            this->hHandle = hHandle;
            memset(this->message, 0, sizeof(this->message));
            message.copy(this->message, sizeof(this->message));


        }

        bool                isError(void) const;
        bool                isSuccess(void) const;
        bool                isPartialSuccess(void) const;

        GblHandle             getHandle(void) const;
        GBL_RESULT              getResult(void) const { return this->resultCode; }
        SourceLocation          getSource(void) const;


        std::string_view    getMessage(void) const;
        const char*         getResultString(void) const { return message; }
        std::string         getSourceString(void) const;

        std::string         toString(void) const;


    };

    GBL_CHECK_C_CPP_TYPE_COMPAT(ApiResult, GblApiResult);



    class StackFrame: public GblStackFrame {
    public:
                    StackFrame(void) = default;
                    StackFrame(GblHandle hHandle, GBL_RESULT initialResult, SourceLocation entryLoc) {
                        GBL_API_STACK_FRAME_CONSTRUCT(this, hHandle, initialResult, entryLoc);

                    }
                    StackFrame(const GblStackFrame& rhs);

                    StackFrame& operator=(const GblStackFrame& rhs) {
                        memcpy(this, &rhs, sizeof(GblStackFrame));
                        return *this;
                    }

        auto        getSourceCurrent(void) const -> const SourceLocation&;
        auto        getSourceEntry(void) const -> const SourceLocation& { return *static_cast<const SourceLocation*>(&sourceEntry);}
        auto        getApiResult(void) const -> const ApiResult&;
        auto        getApiResult(void) -> ApiResult&;
        GblHandle     getHandle(void) const;
        GblContext    getContext(void) const;
        void*       getHandleUd(void) const;
        void*       getContextUd(void) const;

        bool        isValid(void) const;
    };

    GBL_CHECK_C_CPP_TYPE_COMPAT(StackFrame, GblStackFrame);



    class ResultException: public ApiResult {
    public:
        using ApiResult::ApiResult;
    };

    //construct a cpp exception from C error
    //construct a c error from a cpp exception

    template<typename E>
    class StdResultException: public ResultException, public E {
    public:

        StdResultException(GBL_RESULT result) noexcept: ResultException(nullptr, result, gblResultString(result)) {}
        StdResultException(const ResultException&) noexcept;
        StdResultException& operator=(const ResultException&) noexcept;

        StdResultException(ApiResult result);

        virtual ~StdResultException(void) override = default;

        virtual const char* what() const noexcept override {
            return ResultException::getResultString();
        }
    };



    namespace INTERNAL {
        GBL_ENUM_TABLE_DECLARE_CPP(GBL_META_RESULT_TABLE);
    }


    class Result: public INTERNAL::Result {
    public:
        using INTERNAL::Result::Result;
        constexpr Result(void) noexcept: INTERNAL::Result(GBL_RESULT_UNKNOWN) {}
        constexpr Result(bool success) noexcept: INTERNAL::Result(success? GBL_RESULT_SUCCESS : GBL_RESULT_ERROR) {}

        std::string_view toString(void) const noexcept { return gblResultString(getCode()); }

        constexpr bool isUnknown(void) const noexcept { return GBL_RESULT_UNKNOWN(getCode()); }
        constexpr bool isSuccess(void) const noexcept { return GBL_RESULT_SUCCESS(getCode()); }
        constexpr bool isPartial(void) const noexcept { return GBL_RESULT_PARTIAL(getCode()); }
        constexpr bool isError(void) const noexcept { return GBL_RESULT_ERROR(getCode()); }
        constexpr bool isUnavailable(void) const noexcept { return GBL_RESULT_UNAVAILABLE(getCode()); }

        constexpr operator bool() const noexcept { return isSuccess(); }

        constexpr bool wouldThrow(void) const noexcept { return isError(); }

        Result checkThrow(void) const {
            if(wouldThrow()) GBL_UNLIKELY {
                return throwException(*this);
            }
            return *this;
        }

        // Statics


        static Result throwException(Result result) {
            throw StdResultException<std::exception>(result);
            return result;
        }

        static Result tryThrow(Result result) {
            return result.checkThrow();
        }

    };

#define GBL_RESULT_CATCH(code) \
    catch(const ResultException& resultException) {     \
        code = resultException.getResult();  \
    }   \
    catch(...) {    \
        code = Result(Result::ErrorUnhandledException);  \
    }

}

#endif // GIMBAL_TYPES_HPP
