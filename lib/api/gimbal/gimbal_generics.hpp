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
        bool isInRange(void) const noexcept { return false; } \
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


#define GBL_RESULT_CATCH(code) \
catch(const ResultException& resultException) {     \
    code = resultException.getResult();  \
}   \
catch(...) {    \
    code = Result(Result::ErrorUnhandledException);  \
}


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

}

#endif // GIMBAL_GENERICS_HPP
