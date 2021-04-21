#ifndef GIMBAL_TYPES_HPP
#define GIMBAL_TYPES_HPP

#include "gimbal_api.h"
#include <type_traits>
#include <string_view>
#include <exception>
#include <cstring>

#define GBL_CHECK_C_CPP_TYPE_COMPAT(CppType, CType) \
    static_assert(sizeof(CppType) == sizeof(CType), "sizeof(" #CppType ") != sizeof(" #CType")")

namespace gimbal {

template<typename P, typename CRTP>
class PrimitiveCompatible {
private:
    using ThisType = PrimitiveCompatible<P, CRTP>;
    const CRTP* derived(void) const { return static_cast<const CRTP*>(this); }
    CRTP* derived(void) { return static_cast<CRTP*>(this); }
public:

    template<typename T>
    constexpr static std::enable_if_t<std::is_same_v<T, P>, P*> primitive_cast(CRTP* pDerived) { return &*static_cast<ThisType*>(pDerived); }

    constexpr const P* primitive_cast(void) const { return reinterpret_cast<const P*>(derived()->getPrimitiveAddress()); }

    constexpr operator P() const { return reinterpret_cast<P>(derived()->getPrimitiveValue()); }
    constexpr P* operator&(void) { return reinterpret_cast<P*>(derived()->getPrimitiveAddress()); }
    constexpr const P* operator&(void) const { return reinterpret_cast<const P*>(derived()->getPrimitiveAddress()); }
};

template<typename P>
class PrimitiveBase: public PrimitiveCompatible<P, PrimitiveBase<P>> {
protected:
    P primitive_={};
public:
    using PrimitiveType = P;

    constexpr PrimitiveBase(void) = default;
    constexpr PrimitiveBase(P p): primitive_(std::move(p)) {}

    constexpr P getPrimitiveValue(void) const { return primitive_; }
    constexpr const P* getPrimitiveAddress(void) const { return &primitive_; }
    constexpr P* getPrimitiveAddress(void) { return &primitive_; }
};


    using Bool      = ::GblBool;
    using Size      = ::GblSize;
    using Real      = ::GblReal;
    using Int       = ::GblInt;
    using Enum      = ::GblEnum;
    using Hash      = ::GblHash;

    enum class LogLevel: Enum {
        Debug   = GBL_LOG_LEVEL_DEBUG,
        Verbose = GBL_LOG_LEVEL_VERBOSE,
        Info    = GBL_LOG_LEVEL_INFO,
        Warning = GBL_LOG_LEVEL_WARNING,
        Error   = GBL_LOG_LEVEL_ERROR,
        Count   = GBL_LOG_LEVEL_COUNT
    };


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
        SourceLocation(const GblSourceLocation& rhs);

        std::string_view    getFilePath(void) const;
        std::string_view    getFileName(void) const;
        std::string_view    getFunctionName(void) const;
        GblSize             getLineNumber(void) const;
        GblSize             getColumn(void) const;

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

        auto        getSourceCurrent(void) const -> const SourceLocation&;
        auto        getSourceEntry(void) const -> const SourceLocation&;
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

/*
    // Check if there's an error message in getLastError to fetch
    tryCallHandle(handle, function, args);
    tryCall(function, args);
    //construct shit yourself
    tryResult(result, str, source)
*/

    class Result: public PrimitiveBase<GBL_RESULT> {
    public:
        Result(GBL_RESULT code): PrimitiveBase(code) {}

        std::string_view toString(void) const { return gblResultString(getPrimitiveValue()); }

        static Result throwException(GBL_RESULT result) {
            throw StdResultException<std::exception>(result);
            return result;
        }

        static Result tryThrow(GBL_RESULT result) {
            if(GBL_RESULT_ERROR(result)) {
                return throwException(result);
            }
            return result;
        }

    };

#define GBL_RESULT_CATCH(code) \
    catch(const ResultException& resultException) {     \
        code = resultException.getResult();  \
    }   \
    catch(...) {    \
        code = GBL_RESULT_UNKNOWN;  \
    }

}

#endif // GIMBAL_TYPES_HPP
