#ifndef GIMBAL_GENERICS_HPP
#define GIMBAL_GENERICS_HPP

#include <stdexcept>

#define GBL_CHECK_C_CPP_TYPE_COMPAT(CppType, CType) \
    GBL_STATIC_ASSERT_MSG(sizeof(CppType) == sizeof(CType), "sizeof(" #CppType ") != sizeof(" #CType")")

#define GBL_ENUM_TUPLE_DECL_ENUM_CPP(cName, value, name, string) \
    name = value,

#define GBL_ENUM_TABLE_DECLARE_CPP(table) \
    class GBL_EVAL(GBL_META_ENUM_TYPE_PROPERTY(table, NAME)) : \
        public gimbal::PrimitiveBase<GBL_EVAL(GBL_META_ENUM_TYPE_PROPERTY(table, CNAME))> { \
        using CppType = GBL_EVAL(GBL_META_ENUM_TYPE_PROPERTY(table, NAME));  \
        using CType = GBL_EVAL(GBL_META_ENUM_TYPE_PROPERTY(table, CNAME));   \
        public: \
        enum Value { \
            GBL_MAP_TUPLES(GBL_ENUM_TUPLE_DECL_ENUM_CPP,  GBL_MAP_TUPLES(GBL_EVAL, GBL_META_ENUM_TUPLE_VALUE_ARRAY table))  \
        };  \
        constexpr GBL_EVAL(GBL_META_ENUM_TYPE_PROPERTY(table, NAME))(void) noexcept = default; \
        constexpr GBL_EVAL(GBL_META_ENUM_TYPE_PROPERTY(table, NAME))(Value value) noexcept: CppType(static_cast<CType>(value)) {}  \
        constexpr GBL_EVAL(GBL_META_ENUM_TYPE_PROPERTY(table, NAME))(CType code) noexcept: gimbal::PrimitiveBase<CType>(code) {}  \
        constexpr operator Value() const noexcept { return getValue(); } \
        constexpr CType getCode(void) const noexcept { return static_cast<CType>(getPrimitiveValue()); }    \
        constexpr Value getValue(void) const noexcept { return static_cast<Value>(getPrimitiveValue()); }   \
        constexpr std::string_view toString(void) const { \
            GBL_ENUM_TABLE_TO_STRING(table, getCode());   \
            throw std::runtime_error(std::string("Unhandled Error Code: ") + std::to_string(getCode()));    \
            return "Unhandled";    \
        } \
        constexpr GblEnum toInt(void) const noexcept { return static_cast<GblEnum>(getCode()); } \
        constexpr bool isInRange(void) const noexcept { return false; } \
        constexpr friend bool operator==(CppType rhs, Value lhs) {    \
            return rhs.getCode() == CppType(lhs);  \
        }   \
        constexpr friend bool operator!=(CppType rhs, Value lhs) {    \
            return !(rhs == lhs);   \
        }   \
    }; \
    GBL_CHECK_C_CPP_TYPE_COMPAT(GBL_EVAL(GBL_META_ENUM_TYPE_PROPERTY(table, NAME)), GBL_EVAL(GBL_META_ENUM_TYPE_PROPERTY(table, CNAME)))


namespace gimbal {


template<typename P, typename CRTP>
class PrimitiveCompatible {
private:
    using ThisType = PrimitiveCompatible<P, CRTP>;
    const CRTP* derived(void) const { return static_cast<const CRTP*>(this); }
    CRTP* derived(void) { return static_cast<CRTP*>(this); }
public:

    template<typename T>
    constexpr static std::enable_if_t<std::is_same_v<T, P>, P*> primitive_cast(CRTP* pDerived) noexcept { return &*static_cast<ThisType*>(pDerived); }

    constexpr const P* primitive_cast(void) const noexcept { return reinterpret_cast<const P*>(derived()->getPrimitiveAddress()); }

    constexpr operator P() const noexcept { return reinterpret_cast<P>(derived()->getPrimitiveValue()); }
    constexpr P* operator&(void) noexcept { return reinterpret_cast<P*>(derived()->getPrimitiveAddress()); }
    constexpr const P* operator&(void) const noexcept { return reinterpret_cast<const P*>(derived()->getPrimitiveAddress()); }
};

template<typename P>
class PrimitiveBase: public PrimitiveCompatible<P, PrimitiveBase<P>> {
protected:
    P primitive_={};
public:
    using PrimitiveType = P;

    constexpr PrimitiveBase(void) noexcept = default;
    constexpr PrimitiveBase(P p) noexcept: primitive_(std::move(p)) {}

    constexpr P getPrimitiveValue(void) const noexcept { return primitive_; }
    constexpr const P* getPrimitiveAddress(void) const noexcept { return &primitive_; }
    constexpr P* getPrimitiveAddress(void) noexcept { return &primitive_; }
};

class SourceLocation: public GblSourceLocation {
public:
    SourceLocation(void) {
        memset(this, 0, sizeof(SourceLocation));
    }
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

namespace INTERNAL {
    GBL_ENUM_TABLE_DECLARE_CPP(GBL_META_RESULT_TABLE);
}


class Result: public INTERNAL::Result {
public:
    enum class Type: uint8_t {
        Success,
        Partial,
        Error,
        Unknown,
        Count
    };

    using INTERNAL::Result::Result;
    constexpr Result(void) noexcept: INTERNAL::Result(GBL_RESULT_UNKNOWN) {}
    constexpr Result(bool success) noexcept: INTERNAL::Result(success? GBL_RESULT_SUCCESS : GBL_RESULT_ERROR) {}

    std::string_view toString(void) const noexcept { return gblResultString(getCode()); }
    constexpr Type getType(void) const noexcept {
        if(isSuccess()) return Type::Success;
        else if(isPartial()) return Type::Partial;
        else if(isError()) return Type::Error;
        else if(isUnknown()) return Type::Unknown;
        else GBL_ASSERT(false, "Not typed!");
        return Type::Unknown;
    }

    constexpr bool isUnknown(void) const noexcept { return GBL_RESULT_UNKNOWN(getCode()); }
    constexpr bool isSuccess(void) const noexcept { return GBL_RESULT_SUCCESS(getCode()); }
    constexpr bool isPartial(void) const noexcept { return GBL_RESULT_PARTIAL(getCode()); }
    constexpr bool isError(void) const noexcept { return GBL_RESULT_ERROR(getCode()); }
    constexpr bool isUnavailable(void) const noexcept { return GBL_RESULT_UNAVAILABLE(getCode()); }
    constexpr bool isType(Type type) const noexcept { return type == getType(); }

    constexpr operator bool() const noexcept { return isSuccess(); }

    constexpr bool wouldThrow(void) const noexcept { return isError(); }

    Result checkThrow(void) const {
        if(wouldThrow()) GBL_UNLIKELY {
            return throwException(*this);
        }
        return *this;
    }

    // Statics


    static Result throwException(Result result);

    static Result tryThrow(Result result) {
        return result.checkThrow();
    }

};

class Handle;

class CallRecord: public GblCallRecord {
public:
    CallRecord(GblHandle hHandle, GBL_RESULT result, std::string_view message={}) {

         //GBL_CALL_RECORD_CONSTRUCT(&pFrame->record, hHandle, initialResult, entryLoc, gblResultString(initialResult));
        this->result = result;
        this->hHandle = hHandle;
        memset(this->message, 0, sizeof(this->message));
        message.copy(this->message, sizeof(this->message));
    }


    Handle*             getHandle(void) const;
    Result                getResult(void) const { return this->result; }
    SourceLocation          getSource(void) const;


    std::string_view    getMessage(void) const { return message; }
    std::string         getSourceString(void) const;

    std::string         toString(void) const;
/*
    static CallRecord& tryThrow(const CallRecord& record) {
        return result.checkThrow();
    }
*/

};

GBL_CHECK_C_CPP_TYPE_COMPAT(CallRecord, GblCallRecord);

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
    auto        getCallRecord(void) const -> const CallRecord& { return *static_cast<const CallRecord*>(&record); }
    auto        getCallRecord(void) -> CallRecord&;
    GblHandle     getHandle(void) const;
    GblContext    getContext(void) const;
    void*       getHandleUd(void) const;
    void*       getContextUd(void) const;

    bool        isValid(void) const;
};

GBL_CHECK_C_CPP_TYPE_COMPAT(StackFrame, GblStackFrame);



class Exception: public CallRecord, virtual std::exception {
public:
    using CallRecord::CallRecord;

    virtual const char* what(void) const noexcept override {
        return getMessage().data();
    }

};


template<typename E>
class StdException: public Exception, virtual E {
public:

    StdException(GBL_RESULT result) noexcept: Exception(nullptr, result, Result(result).toString()) {}
    StdException(const Exception&) noexcept;
    StdException& operator=(const Exception&) noexcept;

    StdException(CallRecord record);

    virtual ~StdException(void) override = default;

    virtual const char* what() const noexcept override {
        return Exception::getMessage().data();
    }
};


#if 0
class E1 : public std::exception {};
class E2 : public std::exception {};

int main() {
    try {
        throw E2();
    }
    catch( ... ) {
        try {
            throw;
        }
        catch( const E1 & e ) {
            std::cout << "E1\n";
        }
        catch( const E2 & e ) {
            std::cout << "E2\n";
        }
    }
}
#endif


#define GBL_RESULT_CATCH(code) \
catch(const Exception& resultException) {     \
    code = resultException.getResult();  \
}   \
catch(...) {    \
    code = Result(Result::ErrorUnhandledException);  \
}
/*

CPP_EXCEPTION_TABLE {
    std::bad_alloc,
    std::bad_cast,
    std::bad_typeid,
    std::bad_exception,
    std::logic_failure,
    std::runtime_error,
    std::domain_error;
    std::invalid_argument;
    std::length_error;
    std::out_of_range
    std::overflow_error
    std::range_error
    std::underflow_error
}*/


#if 0

void someFunc(gimbal::Context* pCtx) {
    GBL_API_BEGIN(*pCtx);


    try {
       // maybeThrowShit();

    } catch(...) {
        Result result(Result::ErrorUnhandledException);
        std::string message;

        try {
            throw;
        } catch(const std::exception& except) {
            message = except.what();
        }

        try {
            throw;
        } catch(const gimabl::Exception& gblExcept) {
            result = gblExcept.getResult();
        } catch(const std::bad_alloc& except) {
            result = Result(Result::ErrorMemAlloc);
        }

        GBL_API_RECORD_SET(result, message);
        GBL_API_DONE();
    }

    GBL_API_END();

}

#endif


template<typename CRTP, typename Index, typename Value>
class ReadWriteIndexable {
private:

    class IndexableProxy {
    private:
        using ThisType = IndexableProxy;

        CRTP* pObject_{};
        Index index_{};

    public:
        IndexableProxy(CRTP* pObject, Index index):
            pObject_(pObject), index_(index) {}

        operator Value() const {
            return pObject_->getElement_(index_);
        }

        template<typename RValue>
         ThisType operator=(RValue&& rhs) {
            pObject_->setElement_(index_, std::forward<RValue>(rhs));
            return *this;
        }

    };

public:
    void checkBounds(Index index) const {
       if(index >= size()) Result::tryThrow(Result::ErrorOutOfBounds);
    }

    Value operator[](Index index) const {
        return static_cast<const CRTP*>(this)->getElement_(index);
    }

    IndexableProxy operator[](Index index) {
        return IndexableProxy { static_cast<CRTP*>(this), index };
    }

    Index size(void) const {
        return static_cast<const CRTP*>(this)->getElementCount_();
    }

    Index length(void) const { return size(); }

    Index empty(void) const {
        return size() == 0;
    }

    Value at(Index index) const {
        checkBounds(index);
        return static_cast<const CRTP*>(this)->getElement_(index);
    }

    IndexableProxy at(Index index) {
        checkBounds(index);
        return IndexableProxy { static_cast<CRTP*>(this), index };
    }

    Value front(Index index) const {
        return *this[index];
    }

    IndexableProxy front(void) {
        return IndexableProxy { static_cast<CRTP*>(this), 0 };
    }

    Value back(void) const {
        return *this[size() - 1];
    }

    IndexableProxy back(void) {
        return IndexableProxy { static_cast<CRTP*>(this), size() - 1 };
    }
};


template<typename CRTP, typename Index, typename Value>
class RandomAccessIterable {
protected:

    template<typename O, bool Reverse>
    struct iterator_base {
        constexpr static inline bool
              reverse               = Reverse;
        using object_type           = O;
        using iterator_type         = iterator_base<O, Reverse>;
        using value_type            = Value;
        using index_type            = std::make_signed_t<Index>;
        using reference             = std::conditional_t<std::is_const_v<O>,
                                        std::add_lvalue_reference_t<std::add_const_t<Value>>,
                                        std::add_lvalue_reference_t<Value>>;
        using pointer               = std::conditional_t<std::is_const_v<O>,
                                        std::add_pointer_t<std::add_const_t<Value>>,
                                        std::add_pointer_t<Value>>;
        using difference_type       = std::ptrdiff_t;
        using iterator_category     = std::random_access_iterator_tag;

        void checkBounds(Index index) const {
           if(index >= obj_.getElementCount_()) Result::tryThrow(Result::ErrorOutOfBounds);
        }
        reference operator* () {
            checkBounds(index_);
            return obj_.getElement_(index_) ;
        }
        pointer operator& () {
            checkBounds(index_);
            return &obj_.getElement_(index_);
        }
        //pointer operator-> () { return &**this ; } // *** EDIT

        iterator_type& operator++ () {
            if constexpr(Reverse) --index_;
            else ++index_ ;
            return *this;
        }
        iterator_type operator++ (int) {
            const auto temp(*this);
            if constexpr(Reverse) ++*this;
            else --*this;
            return temp;
        }
        iterator_type& operator--() {
            if constexpr(Reverse) ++index_;
            else --index_;
            return *this;
        }
        iterator_type operator--(int) {
            const auto temp(*this);
            if constexpr(Reverse) ++**this;
            else --**this;
            return temp;
        }
        iterator_type& operator+=(index_type offset) { if constexpr(Reverse) index_ -= offset; else index_ += offset; return *this; }
        iterator_type& operator-=(index_type offset) { if constexpr(Reverse) index_ += offset; else index_ -= offset; return *this; }
        iterator_type operator+(index_type offset) { auto temp(*this); if constexpr(Reverse) temp.index_ -= offset; else temp.index_ += offset; return temp; }
        iterator_type operator-(index_type offset) { auto temp(*this); if constexpr(Reverse) temp.index_ += offset; else temp.index_ -= offset; return temp; }

        bool operator== (const iterator_type& rhs) const { return &obj_ == &rhs.obj_ && index_ == rhs.index_; }
        bool operator!= (const iterator_type& rhs) const { return !(*this == rhs); }

        O&    obj_;
        index_type index_={};
    };
public:

    using iterator                  = iterator_base<CRTP, false>;
    using reverse_iterator          = iterator_base<CRTP, true>;
    using const_iterator            = iterator_base<std::add_const_t<CRTP>, false>;
    using const_reverse_iterator    = iterator_base<std::add_const_t<CRTP>, true>;

public:
   iterator                 begin(void) { return {*static_cast<CRTP*>(this), 0}; }
   reverse_iterator         rbegin(void) { return {*static_cast<CRTP*>(this), (int64_t)static_cast<const CRTP*>(this)->getElementCount_()-1 }; }
   iterator                 end(void) { return {*static_cast<CRTP*>(this), (int64_t)static_cast<const CRTP*>(this)->getElementCount_()}; }
   reverse_iterator         rend(void) { return {*static_cast<CRTP*>(this), -1}; }
   const_iterator           cbegin(void) const { return {*static_cast<const CRTP*>(this), 0}; }
   const_reverse_iterator   crbegin(void) const { return {*static_cast<const CRTP*>(this), (int64_t)static_cast<const CRTP*>(this)->getElementCount_()-1}; }
   const_iterator           cend(void) const { return {*static_cast<const CRTP*>(this), (int64_t)static_cast<const CRTP*>(this)->getElementCount_()}; }
   const_reverse_iterator   crend(void) const { return {*static_cast<const CRTP*>(this), -1}; }
};

inline Result Result::throwException(Result result){
       throw StdException<std::exception>(result);
       return result;
   }

}

#endif // GIMBAL_GENERICS_HPP
