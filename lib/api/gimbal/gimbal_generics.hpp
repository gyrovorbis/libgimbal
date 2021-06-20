#ifndef GIMBAL_GENERICS_HPP
#define GIMBAL_GENERICS_HPP

#include <stdexcept>
#include <iterator>
#include "gimbal_api.hpp"

#define GBL_CHECK_C_CPP_TYPE_COMPAT(CppType, CType) \
    GBL_STATIC_ASSERT_MSG(sizeof(CppType) == sizeof(CType), "sizeof(" #CppType ") != sizeof(" #CType")")

#define GBL_ENUM_TUPLE_DECL_ENUM_CPP(cName, value, name, string) \
    name = value,
#if 0

template<typename E>
concept enum_type {
    requires(E e) {
        typename E::CppType;
        typename E::CType;
        {


    };

template<typename E, typename T>
concept EnumCompatible =
    std::is_same_v<E
        #endif


#define GBL_ENUM_TABLE_DECLARE_CPP_BEGIN(table) \
    class GBL_EVAL(GBL_META_ENUM_TYPE_PROPERTY(table, NAME)) : \
        public gimbal::PrimitiveBase<GBL_EVAL(GBL_META_ENUM_TYPE_PROPERTY(table, CNAME))> { \
        using CppType = GBL_EVAL(GBL_META_ENUM_TYPE_PROPERTY(table, NAME));  \
        using CType = GBL_EVAL(GBL_META_ENUM_TYPE_PROPERTY(table, CNAME));   \
        public: \
        enum Value { \
            GBL_MAP_TUPLES(GBL_ENUM_TUPLE_DECL_ENUM_CPP,  GBL_MAP_TUPLES(GBL_EVAL, GBL_META_ENUM_TUPLE_VALUE_ARRAY table))  \
        };  \
        constexpr GBL_EVAL(GBL_META_ENUM_TYPE_PROPERTY(table, NAME))(void) noexcept: gimbal::PrimitiveBase<CType>(static_cast<CType>(0)) {} \
        constexpr GBL_EVAL(GBL_META_ENUM_TYPE_PROPERTY(table, NAME))(Value value) noexcept: CppType(static_cast<CType>(value)) {}  \
        constexpr GBL_EVAL(GBL_META_ENUM_TYPE_PROPERTY(table, NAME))(CType code) noexcept: gimbal::PrimitiveBase<CType>(code) {}  \
        constexpr operator Value() const noexcept { return getValue(); } \
        constexpr CType getCode(void) const noexcept { return static_cast<CType>(getPrimitiveValue()); }    \
        constexpr Value getValue(void) const noexcept { return static_cast<Value>(getPrimitiveValue()); }   \
        std::string_view toString(void) const { \
            const char* pStr = GBL_EVAL(GBL_META_ENUM_TYPE_PROPERTY(table, STRINGIFIER))(getCode()); \
            if(strcmp(pStr, "") == 0) { \
                throw std::runtime_error(std::string("Unhandled Enum Value ") + std::to_string(getCode()));    \
            }   \
            return pStr; \
        } \
        constexpr GblEnum toInt(void) const noexcept { return static_cast<GblEnum>(getCode()); } \
        constexpr bool isInRange(void) const noexcept { return false; } \
        constexpr friend bool operator==(CppType rhs, Value lhs) {    \
            return rhs.getCode() == CppType(lhs);  \
        }   \
        constexpr friend bool operator!=(CppType rhs, Value lhs) {    \
            return !(rhs == lhs);   \
        }


#define GBL_ENUM_TABLE_DECLARE_CPP_END(table) \
    }; \
    GBL_CHECK_C_CPP_TYPE_COMPAT(GBL_EVAL(GBL_META_ENUM_TYPE_PROPERTY(table, NAME)), GBL_EVAL(GBL_META_ENUM_TYPE_PROPERTY(table, CNAME)))

#define GBL_ENUM_TABLE_DECLARE_CPP(table) \
    GBL_ENUM_TABLE_DECLARE_CPP_BEGIN(table) \
    GBL_ENUM_TABLE_DECLARE_CPP_END(table)


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
    SourceLocation(const char* pFile, const char* pFunc, GblSize line, GblSize col):
        GblSourceLocation{pFile, pFunc, line, col} {}

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


GBL_ENUM_TABLE_DECLARE_CPP_BEGIN(GBL_META_RESULT_TABLE)
public:
    enum class Type: uint8_t {
        Success,
        Partial,
        Error,
        Unknown,
        Count
    };

    constexpr Result(bool success) noexcept: Result(success? GBL_RESULT_SUCCESS : GBL_RESULT_ERROR) {}

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
GBL_ENUM_TABLE_DECLARE_CPP_END(GBL_META_RESULT_TABLE);

class Handle;


class CallRecord: public GblCallRecord {
public:

    template<typename R>
        requires (std::is_constructible_v<Result, R> && !std::is_same_v<Result, R>)
    CallRecord(R resultValue, const char* pMessage=nullptr, Handle* pHandle=nullptr, SourceLocation srcLoc={}) noexcept:
        CallRecord(Result(resultValue), pMessage, pHandle, std::move(srcLoc)) {}

    CallRecord(Result result, const char* pMessage=nullptr, Handle* pHandle=nullptr, SourceLocation srcLoc={}) noexcept;

    CallRecord(const GblCallRecord& other) {
        memcpy(this, &other, sizeof(GblCallRecord));
    }

    Handle*             getHandle(void) const;
    Result                getResult(void) const { return this->result; }
    const SourceLocation&          getSource(void) const {
        return static_cast<const SourceLocation&>(srcLocation);
    }


    std::string_view    getMessage(void) const { return message; }
    std::string         getSourceString(void) const;

    std::string         toString(void) const;

};

GBL_CHECK_C_CPP_TYPE_COMPAT(CallRecord, GblCallRecord);



class Exception: public CallRecord {
public:


    using CallRecord::CallRecord;
    Exception(const CallRecord& record) noexcept:
        CallRecord(record) {}
    Exception(CallRecord&& record) noexcept:
        CallRecord(std::move(record)) {}

    virtual const char* what(void) const noexcept {
        return getMessage().data();
    }

    virtual const std::exception&   asStdException(void) const = 0;
    virtual std::exception&         asStdException(void) = 0;

    static const CallRecord& throwException(const CallRecord& record);

    static const CallRecord& checkThrow(const CallRecord& record) {
        if(record.getResult().isError()) {
            return throwException(record);
        }
        return record;
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
        const char* getMessage(void) const { return getRecord().getMessage().data(); }

        operator bool() const { return getRecord().getResult().isSuccess(); }
    };
};


template<typename StdType>
class StdException: public Exception, public StdType {
public:

    template<typename V>
    requires std::is_constructible_v<CallRecord,V> && std::is_default_constructible_v<StdType>
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
        StdType(getMessage().data()) {}

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


template<typename CRTP, typename Index, typename Value>
class ReadWriteContiguousIndexable {
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
       if(index >= size()) Exception::checkThrow({Result::ErrorOutOfRange, "index > size"});
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

    namespace tags {
        struct RandomAccessIteratorBase {};
    }

    template<typename O, typename Index, typename Value, bool Reverse>
    struct RandomAccessIterator:
            public tags::RandomAccessIteratorBase,
            public std::iterator<std::random_access_iterator_tag, Value> {
        constexpr static inline bool
              reverse               = Reverse;
        using object_type           = O;
        using iterator_type         = RandomAccessIterator<O, Index, Value, Reverse>;
        using const_iterator_type   = RandomAccessIterator<std::add_const_t<O>, Index, Value, Reverse>;
        using nonconst_iterator_type  = RandomAccessIterator<std::remove_const_t<O>, Index, Value, Reverse>;
        using value_type            = Value;
        using index_type            = std::make_signed_t<Index>;
        using reference             = std::conditional_t<std::is_const_v<O>,
                                        std::add_lvalue_reference_t<std::add_const_t<Value>>,
                                        std::add_lvalue_reference_t<Value>>;
        using const_reference       = std::add_const_t<reference>;
        using pointer               = std::conditional_t<std::is_const_v<O>,
                                        std::add_pointer_t<std::add_const_t<Value>>,
                                        std::add_pointer_t<Value>>;
        using difference_type       = std::ptrdiff_t;
        using iterator_category     = std::random_access_iterator_tag;

        void checkBounds(Index index) const {
           if(index >= pObj_->getElementCount_()) Exception::checkThrow({Result::ErrorOutOfRange, "Index >= obj.elementCount"});
        }
#if 0
        template<typename T = reference>
        std::enable_if_t<!std::is_const_v<object_type>, T> operator* () {
            checkBounds(index_);
            return pObj_->getElement_(index_);
        }
#else
        reference operator* () {
            checkBounds(index_);
            return pObj_->getElement_(index_);
        }
#endif
        const_reference operator* () const {
            checkBounds(index_);
            return const_cast<const O*>(pObj_)->getElement_(index_);
        }

        pointer operator& () {
            checkBounds(index_);
            return &pObj_->getElement_(index_);
        }

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

        friend iterator_type operator+(const iterator_type& it, index_type offset) { auto temp(it); if constexpr(Reverse) temp.index_ -= offset; else temp.index_ += offset; return temp; }
        friend iterator_type operator+(index_type offset, const iterator_type& it) { if constexpr(!Reverse) return it + offset; else return it - offset; }
        friend iterator_type operator-(const iterator_type& it, index_type offset) { auto temp(it); if constexpr(Reverse) temp.index_ += offset; else temp.index_ -= offset; return temp; }
        iterator_type operator+(const iterator_type& rhs) const { if constexpr(!Reverse) return *this + rhs.index_; else return *this - rhs.index_; }
        //difference_type operator-(const iterator_type& rhs) const { auto temp(*this); if constexpr(Reverse) temp.index_ += rhs.index_; else temp.index_ -= rhs.index_; return temp.index_; }
        difference_type operator-(const iterator_type& rhs) const { auto temp(*this); temp.index_ -= rhs.index_; return temp.index_; }

        reference operator[](difference_type offset) const {
            return pObj_->getElement_(index_ + offset);
        }

        bool operator== (const iterator_type& rhs) const { return pObj_ == pObj_ && index_ == rhs.index_; }
        bool operator<=  (const iterator_type& rhs) const { return pObj_ == rhs.pObj_ && index_ <= rhs.index_; }
        bool operator<  (const iterator_type& rhs) const { return pObj_ == rhs.pObj_ && index_ < rhs.index_; }
        bool operator>  (const iterator_type& rhs) const { return pObj_ == rhs.pObj_ && index_ > rhs.index_; }
        bool operator>=  (const iterator_type& rhs) const { return pObj_ == rhs.pObj_ && index_ >= rhs.index_; }
        bool operator!= (const iterator_type& rhs) const { return !(*this == rhs); }

        RandomAccessIterator(void)=default;

        RandomAccessIterator(const const_iterator_type& rhs) requires(std::is_const_v<O>):
            pObj_(rhs.pObj_), index_(rhs.index_) {}

        RandomAccessIterator(O& objType, index_type index=-1):
            pObj_(&objType), index_(index) {}

        RandomAccessIterator(const nonconst_iterator_type& rhs):
            RandomAccessIterator(*rhs.pObj_, rhs.index_) {}

        O*    pObj_         =   nullptr;
        index_type index_   =   -1;
    };


template<typename CRTP, typename Index, typename Value>
class RandomAccessIterable {
public:

    using iterator                  = RandomAccessIterator<CRTP, Index, Value, false>;
    using reverse_iterator          = RandomAccessIterator<CRTP, Index, Value, true>;
    using const_iterator            = RandomAccessIterator<std::add_const_t<CRTP>, Index, Value, false>;
    using reverse_const_iterator    = RandomAccessIterator<std::add_const_t<CRTP>, Index, Value, true>;

    static_assert(std::random_access_iterator<iterator>,                "iterator_not STL-compatible!");
    static_assert(std::random_access_iterator<reverse_iterator>,        "reverse_iterator not STL-compatible!");
    static_assert(std::random_access_iterator<const_iterator>,          "const_iterator not STL-compatible!");
    static_assert(std::random_access_iterator<reverse_const_iterator>,  "reverse_const_iterator not STL-compatible!");

public:
    //return revierse iterators
   iterator                 begin(void) { return {*static_cast<CRTP*>(this), 0}; }
   auto                     begin(void) const { return cbegin(); }
   //reverse_iterator         rbegin(void) { return {*static_cast<CRTP*>(this), (int64_t)static_cast<const CRTP*>(this)->getElementCount_()-1 }; }
   auto         rbegin(void) { return std::make_reverse_iterator(end()); }
   auto                     rbegin(void) const { return crbegin(); }
   iterator                 end(void) { return {*static_cast<CRTP*>(this), (int64_t)static_cast<const CRTP*>(this)->getElementCount_()}; }
   auto                     end(void) const { return cend(); }
   //reverse_iterator         rend(void) { return {*static_cast<CRTP*>(this), -1}; }
   auto         rend(void) { return std::make_reverse_iterator(begin()); }
   auto                     rend(void) const { return crend(); }
   const_iterator           cbegin(void) const { return {*static_cast<const CRTP*>(this), 0}; }
   //reverse_const_iterator   crbegin(void) const { return {*static_cast<const CRTP*>(this), (int64_t)static_cast<const CRTP*>(this)->getElementCount_()-1}; }
   auto   crbegin(void) const { return std::make_reverse_iterator(cend()); }
   const_iterator           cend(void) const { return {*static_cast<const CRTP*>(this), (int64_t)static_cast<const CRTP*>(this)->getElementCount_()}; }
   //reverse_const_iterator   crend(void) const { return {*static_cast<const CRTP*>(this), -1}; }
   auto crend(void) const { return std::make_reverse_iterator(cbegin()); }
};

CallRecord Exception::tryCatchRecord(std::invocable auto fn, SourceLocation loc) noexcept {
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

}

#endif // GIMBAL_GENERICS_HPP
