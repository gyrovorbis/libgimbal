#ifndef GIMBAL_STRING_HPP
#define GIMBAL_STRING_HPP

#include <gimbal/strings/gimbal_string_buffer.h>
#include <gimbal/containers/gimbal_vector.hpp>
#include <iostream>
#include <unordered_set>
#include <compare>

namespace gimbal {
namespace tags {
    struct StringBase {};
}

template<typename T, typename SFINAE>
struct StringTraits {
    template<typename F=T>
    static F&& stringify(F&& value) { return std::forward<F>(value); }
};


template<typename T>
concept string_base = std::is_base_of_v<gimbal::tags::StringBase, T>;

class String;

//make this all compatible with gimbal::Vector<char>

//support string ?
template<typename CRTP>
class StringViewBase:
    public tags::StringBase,
    public ReadWriteContiguousIndexable<StringViewBase<CRTP>, size_t, char>,
    public RandomAccessIterable<StringViewBase<CRTP>, size_t, char>
{
public:
    using StringViewType    = StringViewBase<CRTP>;
    using Derived           = CRTP;
    using Iterable          = RandomAccessIterable<StringViewType, size_t, char>;
    using iterator          = typename Iterable::iterator;
    using const_iterator    = typename Iterable::const_iterator;
protected:
    decltype(auto) str_(void) const {
        return static_cast<const CRTP*>(this)->getString_();
    }

    decltype(auto) str_(void) {
        return static_cast<CRTP*>(this)->getString_();
    }

public:

    operator const GblStringBuffer*() const { return str_(); }


    const char& getElement_(size_t index) const {
        return getCString()[index];
    }

    char& getElement_(size_t index) {
        return getCString()[index];
    }

    void setElement_(size_t index, char value) {
        const_cast<char*>(getCString())[index] = value;
    }

    size_t getElementCount_(void) const {
        return getLength();
    }

// Accessor wrappers

    Size getStackBytes(void) const {
        Size size = GblStringBuffer_stackBytes(str_());
        return size;
    }

    constexpr Size getLength(void) const {
        return GblStringBuffer_length(str_());
    }

    constexpr Size getCapacity(void) const {
        return GblStringBuffer_capacity(str_());
    }

    Context* getContext(void) const {
        GblContext* pCtx = GblStringBuffer_context(str_());
        return pCtx == nullptr? nullptr : Context::fromGblObj(pCtx);
    }

    constexpr bool isEmpty(void) const {
        GblBool empty = GblStringBuffer_empty(str_());
        return empty;
    }

    constexpr bool isStack(void) const {
        GblBool stack = GblStringBuffer_stack(str_());
        return stack;
    }

    // C++-specific utils

    constexpr bool isHeap(void) const {
        return !isStack();
    }

    const char* getCString(void) const {
        const char* pCStr = GblStringBuffer_cString(str_());
        return pCStr;
    }

    char* getCString(void) {
        const char* pStr = GblStringBuffer_cString(str_());
        return const_cast<char*>(pStr);
    }

    std::string toStdString(void) const {
        return std::string(getCString());
    }

    constexpr std::string_view toStringView(void) const {
        return getLength()?
            std::string_view(getCString(), getLength()) : std::string_view();
    }
    friend constexpr bool operator==(const Derived& lhs, const char* pRhs) noexcept {
        return (lhs.toStringView() == std::string_view(pRhs));
    }
    friend constexpr decltype(auto) operator<=>(const Derived& lhs, const char* pRhs) noexcept {
        return (lhs.toStringView() <=> std::string_view(pRhs));
    }
    friend constexpr bool operator==(const Derived& lhs, const std::string& rhs) noexcept {
        return (lhs.toStdString() == rhs);
    }
    friend constexpr decltype(auto) operator<=>(const Derived& lhs, const std::string& rhs) noexcept {
        return (lhs.toStdString() <=> rhs);
    }
#ifdef GBL_PMR_STRING
    friend constexpr bool operator==(const Derived& lhs, const std::pmr::string& rhs) noexcept {
        return (lhs == rhs.c_str());
    }
    friend constexpr decltype(auto) operator<=>(const Derived& lhs, const std::pmr::string& rhs) noexcept {
        return (lhs.toStdString() <=> rhs);
    }
#endif
    friend constexpr bool operator==(const Derived& lhs, std::string_view rhs) noexcept {
        return (lhs.toStringView() == rhs);
    }
    friend constexpr decltype(auto) operator<=>(const Derived& lhs, std::string_view rhs) noexcept {
        return (lhs.toStringView() <=> rhs);
    }
    friend constexpr bool operator==(const Derived& lhs, const string_base auto& rhs) {
        gimbal::Int result = GblStringBuffer_compare(&lhs, &rhs);
        return result == 0;
    }
    friend constexpr decltype(auto) operator<=>(const Derived& lhs, const string_base auto& rhs) {
        return (lhs <=> rhs);
    }
    friend std::ostream& operator<<(std::ostream& output, const Derived& s) {
        output << s.getCString();
        if(output.fail()) Exception::checkThrow(Result::ErrorFileWrite);
        return output;
    }
};

class StringView final: public StringViewBase<StringView> {
private:
    const GblStringBuffer* pGblStr_ = nullptr;
public:
    StringView(void) = default;

    StringView(const GblStringBuffer& gblStr):
        pGblStr_(&gblStr) {}

    StringView(const gimbal::String& string);

    StringView(const GblStringBuffer* pGblStringBuffer):
        pGblStr_(pGblStringBuffer) {}

    const GblStringBuffer* getString_(void) const { return pGblStr_; }
    bool isValid(void) const { return pGblStr_; }
};

class String:
    public GblStringBuffer,
    public StringViewBase<String>
{
public:

    const GblStringBuffer* getString_(void) const {
        return static_cast<const GblStringBuffer*>(this);
    }
// iterator constructors!
    GblStringBuffer* getString_(void) {
        return static_cast<GblStringBuffer*>(this);
    }

    //operator const GblStringBuffer*() const { return getString_(); }

    // Default Ctors
    String(std::nullptr_t, Size size=sizeof(String)):
        String(static_cast<Context*>(nullptr), size) {}

    String(Context* pCtx=nullptr, Size size=sizeof(String)):
        String(std::string_view{}, pCtx, size) {}

    // Value Ctors
    String(const std::string& stdString, Context* pCtx=nullptr, Size size=sizeof(String)):
        String(std::string_view{stdString.c_str()}, pCtx, size) {}

#ifdef GBL_PMR_STRING
    String(const string& stdPmrString, Context* pCtx=nullptr, Size size=sizeof(String)):
        String(std::string_view{stdPmrString.c_str()}, pCtx, size) {}
#endif

    String(const char* pCStr, Context* pCtx=nullptr, Size size=sizeof(String)):
        String(pCStr? std::string_view{pCStr} : std::string_view{}, pCtx, size) {}

    String(const std::string_view& stringView, Context* pCtx=nullptr, Size size=sizeof(String)) {
        Exception::checkThrow(GblStringBuffer_construct(this,
                                            GBL_STRING_VIEW(stringView.data(), stringView.length()),
                                            size,
                                            pCtx));
    }

    // Copy Ctor
    String(const String& rhs, Context* pCtx=nullptr, Size size=sizeof(String)):
        String(StringView(rhs), pCtx, size) {}

    String(StringView other, Context* pCtx=nullptr, Size size=sizeof(String)):
        String(other.toStringView(), pCtx? pCtx : other.getContext(), size) {}

    // Move Ctor
    String(String&& rhs, Context* pCtx=nullptr, Size size=sizeof(String)):
        String(static_cast<GblStringBuffer&&>(rhs), pCtx, size) {}

    String(GblStringBuffer&& rhs, Context* pCtx=nullptr, Size size=sizeof(String)):
        String(pCtx, size)
    {
        *this = rhs;
    }

    ~String(void) {
        Exception::checkThrow(GblStringBuffer_destruct(this));
    }

    static std::pair<char*, Size> take(GblStringBuffer* pGblStr) {
        char* pCStr = nullptr;
        Size capacity = 0;
        Exception::checkThrow(GblStringBuffer_release(pGblStr, &pCStr, &capacity));
        return { pCStr, capacity };
    }

    auto take(void) {
        return take(this);
    }

    void give(std::pair<char*, Size> data) {
        Exception::checkThrow(GblStringBuffer_acquire(this, data.first, data.second));
    }

    void clear(void) {
        Exception::checkThrow(GblStringBuffer_clear(this));
    }

    const String& operator=(std::string_view view) {
        Exception::checkThrow(GblStringBuffer_set(this, GBL_STRING_VIEW(view.data(), view.size())));
        return *this;
    }

    const String& operator=(const String& rhs) {
        return *this = StringView(rhs);
    }

    const String& operator=(StringView string) {
        return *this = string.toStringView();
    }

    const String& operator=(const std::string& stdString) {
        return *this = stdString.c_str();
    }

#ifdef GBL_PMR_STRING
    const String& operator=(const string& stdString) {
        return *this = stdString.c_str();
    }
#endif

    const String& operator=(const char* pCStr) {
        if(pCStr) return *this = std::string_view(pCStr);
        else return *this = nullptr;
    }

    const String& operator=(std::nullptr_t) {
        clear();
        return *this;
    }

    const String& operator=(String&& rhs) {
        return *this = static_cast<GblStringBuffer&&>(rhs);
    }

    const String& operator=(GblStringBuffer&& rhs) {
        StringView rhsView(rhs);

        if(getContext() == rhsView.getContext() && rhsView.isHeap()) {
            give(take(&rhs));
        } else { //cannot move construct!
            *this = rhsView.toStringView();
        }

        return *this;
    }

    const String& operator+=(std::string_view view) {
        concat(view);
        return *this;
    }

    const String& operator+=(StringView view) {
        return *this += view.toStringView();
    }

    void reserve(Size capacity) {
        Exception::checkThrow(GblStringBuffer_reserve(this, capacity));
    }

    void resize(Size size) {
        Exception::checkThrow(GblStringBuffer_resize(this, size));
    }

    void insert(const_iterator pos, const char* pString, GblSize count=0) {
        GBL_ASSERT(pString);
        const auto index = std::distance(cbegin(), pos);
        Exception::checkThrow(GblStringBuffer_insert(this, index, GBL_STRING_VIEW(pString, count)));
      //  return iterator(*this, index);
    }

    void concat(std::string_view view) {
        //insert(cend(), view.data(), view.size());
        Exception::checkThrow(GblVector_append(&this->data, view.data(), view.size()));
    }

    String& vasprintf(const char* pFmt, va_list varArgs) {
        Exception::checkThrow(GblStringBuffer_appendVPrintf(this, pFmt, varArgs));
        return *this;
    }

    //return the actual return value?
    String& sprintf(const char* pFmt, ...) {
        va_list varArgs;
        va_start(varArgs, pFmt);
        vasprintf(pFmt, varArgs);
        va_end(varArgs);
        return *this;
    }

    //stringification traits
    template<typename... Args>
    String& varArgs(Args&&... args) {
        String temp("", getContext());
        temp.sprintf(getCString(), std::forward<Args>(args)...);
        *this = temp;
        return *this;
    }

    // CHECK ERROR BITS
    friend std::istream& operator>>(std::istream& input, String &s) {
        std::string line;
        // Still failes gracefully, so whatever...
        //if(input.good()) {
        //    input.clear();
               std::getline(input, line);
        //     if(input.fail()) {
        //         Exception::checkThrow(Result::FileRead);
        //     }
            s = line;
        //}
        return input;
    }

    friend void swap(String& lhs, String& rhs) {
        void* pTemp = GBL_ALLOCA(sizeof(String));
        memcpy(pTemp, &lhs, sizeof(String));
        memcpy(&lhs, &rhs, sizeof(String));
        memcpy(&rhs, pTemp, sizeof(String));
    }
};





inline String operator+(const gimbal::StringView& lhs, const gimbal::StringView& rhs) {
    return String(lhs) += rhs;
}

inline String operator+(const gimbal::StringView& lhs, const std::string_view& rhs) {
    return String(lhs) += rhs;
}

inline String operator"" _gstr(const char* pLiteral, std::size_t length) {
   // static_assert(length < GBL_STRING_BUFFER_BASE_STACK_SIZE);
    return String(std::string_view{pLiteral, length});
}

/* Use variable-length array in C to increase stack size via alloca (but then I guess we gotta store it too)
 * use template argument in C++ to inherit and increase stack size...
 *
 */

// overflow when copying shit into the bitch without enough room on stack when no context is present
template<Size ExtraStackSize>
class FlexibleString final: public String {
public:
    using FlexibleStringType = FlexibleString<ExtraStackSize>;
    constexpr static const inline Size ExtraStackBufferSize = ExtraStackSize;
private:
    char extraStackBuffer_[ExtraStackSize] = { '0' };
public:

};

inline StringView::StringView(const gimbal::String& string):
    StringView(static_cast<const GblStringBuffer&>(string)) {}

}

namespace std {
    template<gimbal::string_base S>
    struct hash<S> {
        std::size_t operator()(const S& str) const {
            return std::hash<std::string_view>{}(std::string_view(str.getCString()));
        }
    };
}


#endif // GIMBAL_STRING_HPP
