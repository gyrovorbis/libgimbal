#ifndef GIMBAL_STRING_HPP
#define GIMBAL_STRING_HPP

#include <gimbal/gimbal_types.hpp>
#include <gimbal/gimbal_string.h>
#include <gimbal/gimbal_context.hpp>
#include <iostream>
#include <unordered_set>
#include <compare>

namespace gimbal {
// string literal operator overload ""
// string_view sv{ "Hello"sv };
//  "Hello"gs -> gimbal string literal
// hash overloads
// implement all operators and shit
/*
READ:
1. constant view, constant pointer
WRITE:
2. constant view, nonconst pointer
3. nonconst view, nonconst pointer
4. nonconst string, nonconst pointer
*/


namespace tags {
    struct StringBase {};
}

template<typename T>
concept string_base = std::is_base_of_v<gimbal::tags::StringBase, T>;

template<typename CTRP>
class StringViewBase;

class StringView;
class String;

template<gimbal::Size Size>
class FlexibleString;



auto operator<=>(const string_base auto& lhs, const std::string& rhs) noexcept;


//ADD CONST ITERATORS
template<typename CRTP>
class StringViewBase:
    public tags::StringBase,
    public ReadWriteIndexable<StringViewBase<CRTP>, size_t, char>,
    public RandomAccessIterable<StringViewBase<CRTP>, size_t, char>
{
public:
    using StringViewType    = StringViewBase<CRTP>;
    using Derived           = CRTP;
protected:
    decltype(auto) str_(void) const {
        return static_cast<const CRTP*>(this)->getString_();
    }

    decltype(auto) str_(void) {
        return static_cast<CRTP*>(this)->getString_();
    }

public:

    const char& getElement_(size_t index) const {
        if(index >= getLength()) Result::tryThrow(Result::ErrorOutOfBounds);
        return getCString()[index];
    }

    char& getElement_(size_t index) {
        if(index >= getLength()) Result::tryThrow(Result::ErrorOutOfBounds);
        return getCString()[index];
    }

    void setElement_(size_t index, char value) {
        if(index >= getLength()) Result::tryThrow(Result::ErrorOutOfBounds);
        const_cast<char*>(getCString())[index] = value;
    }

    size_t getElementCount_(void) const {
        return getLength();
    }

// Accessor wrappers

    Size getStackSize(void) const {
        Size size = 0;
        Result::tryThrow(gblStringStackSize(str_(), &size));
        return size;
    }

    constexpr Size getLength(void) const {
        Size length = 0;
        Result::tryThrow(gblStringLength(str_(), &length));
        return length;
    }

    constexpr Size getCapacity(void) const {
        Size capacity = 0;
        Result::tryThrow(gblStringCapacity(str_(), &capacity));
        return capacity;
    }

    Context* getContext(void) const {
        GblContext hCtx = GBL_HANDLE_INVALID;
        Result::tryThrow(gblStringContext(str_(), &hCtx));
        return hCtx == GBL_HANDLE_INVALID? nullptr : Context::fromHandle(hCtx);
    }

    constexpr bool isEmpty(void) const {
        GblBool empty = GBL_TRUE;
        Result::tryThrow(gblStringIsEmpty(str_(), &empty));
        return empty;
    }

    constexpr bool isStack(void) const {
        GblBool stack = GBL_FALSE;
        Result::tryThrow(gblStringIsStack(str_(), &stack));
        return stack;
    }

    // C++-specific utils

    constexpr bool isHeap(void) const {
        return !isStack();
    }

    const char* getCString(void) const {
        const char* pCStr = nullptr;
        Result::tryThrow(gblStringCStr(str_(), &pCStr));
        return pCStr;
    }

    char* getCString(void) {
        return str_()->data.pBuffer;
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
    friend constexpr bool operator==(const Derived& lhs, std::string_view rhs) noexcept {
        return (lhs.toStringView() == rhs);
    }
    friend constexpr decltype(auto) operator<=>(const Derived& lhs, std::string_view rhs) noexcept {
        return (lhs.toStringView() <=> rhs);
    }
    friend constexpr bool operator==(const Derived& lhs, const string_base auto& rhs) noexcept {
        return (lhs.toStringView() == rhs.toStringView());
    }
    friend constexpr decltype(auto) operator<=>(const Derived& lhs, const string_base auto& rhs) noexcept {
        return (lhs.toStringView() <=> rhs.toStringView());
    }

    friend std::ostream& operator<<(std::ostream& output, const Derived& s) {
        return output << s.getCString();
    }

};

class StringView final: public StringViewBase<StringView> {
private:
    const GblString* pGblStr_ = nullptr;
public:
    StringView(void) = default;

    StringView(const GblString& gblStr):
        pGblStr_(&gblStr) {}

    StringView(const gimbal::String& string);

    const GblString* getString_(void) const {
        return pGblStr_;
    }

    bool isValid(void) const { return pGblStr_; }
};

// add nonconst iterators

class String:
    public GblString,
    public StringViewBase<String>
{
public:

    const GblString* getString_(void) const {
        return static_cast<const GblString*>(this);
    }

    GblString* getString_(void) {
        return static_cast<GblString*>(this);
    }

    // Default Ctors
    String(std::nullptr_t, Size size=sizeof(String)):
        String(static_cast<Context*>(nullptr), size) {}

    String(Context* pCtx=nullptr, Size size=sizeof(String)):
        String(std::string_view{}, pCtx, size) {}

    // Value Ctors
    String(const std::string& stdString, Context* pCtx=nullptr, Size size=sizeof(String)):
        String(std::string_view{stdString.c_str()}, pCtx, size) {}

    String(const char* pCStr, Context* pCtx=nullptr, Size size=sizeof(String)):
        String(pCStr? std::string_view{pCStr} : std::string_view{}, pCtx, size) {}

    String(const std::string_view& stringView, Context* pCtx=nullptr, Size size=sizeof(String)) {
        GblStringView tempView = {stringView.data(), stringView.length()};
        Result::tryThrow(gblStringConstruct(this,
                                            size,
                                            pCtx? static_cast<GblContext>(*pCtx) : nullptr,
                                            &tempView));
    }

    // Copy Ctor
    String(const String& rhs, Context* pCtx=nullptr, Size size=sizeof(String)):
        String(StringView(rhs), pCtx, size) {}

    String(StringView other, Context* pCtx=nullptr, Size size=sizeof(String)):
        String(other.toStringView(), pCtx? pCtx : other.getContext(), size) {}

    // Move Ctor
    String(String&& rhs, Context* pCtx=nullptr, Size size=sizeof(String)):
        String(static_cast<GblString&&>(rhs), pCtx, size) {}

    String(GblString&& rhs, Context* pCtx=nullptr, Size size=sizeof(String)):
        String(pCtx, size)
    {
        *this = rhs;
    }

    ~String(void) {
        Result::tryThrow(gblStringDestruct(this));
    }

    static std::pair<char*, Size> take(GblString* pGblStr) {
        char* pCStr = nullptr;
        Size capacity = 0;
        Result::tryThrow(gblStringTake(pGblStr, &pCStr, &capacity));
        return { pCStr, capacity };
    }

    auto take(void) {
        return take(this);
    }

    void give(std::pair<char*, Size> data) {
        Result::tryThrow(gblStringGive(this, data.first, data.second));
    }

    void clear(void) {
        Result::tryThrow(gblStringClear(this));
    }

    const String& operator=(std::string_view view) {
        GblStringView gview = { view.data(), view.size() };
        Result::tryThrow(gblStringAssign(this, &gview));
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

    const String& operator=(const char* pCStr) {
        if(pCStr) return *this = std::string_view(pCStr);
        else return *this = nullptr;
    }

    const String& operator=(std::nullptr_t) {
        clear();
        return *this;
    }

    const String& operator=(String&& rhs) {
        return *this = static_cast<GblString&&>(rhs);
    }

    const String& operator=(GblString&& rhs) {
        StringView rhsView(rhs);
        Context* pCtx = getContext()? getContext() : rhsView.getContext();

        if(pCtx == rhsView.getContext() && rhsView.isHeap()) {
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
        Result::tryThrow(gblStringReserve(this, capacity));
    }

    void resize(Size size) {
        Result::tryThrow(gblStringResize(this, size));
    }

    void concat(std::string_view view) {
        const GblStringView gblView{ view.data(), view.size() };
        Result::tryThrow(gblStringCat(this, &gblView));
    }

    void vasprintf(const char* pFmt, va_list varArgs) {
        Result::tryThrow(gblStringVaSprintf(this, pFmt, varArgs));
    }

    //return the actual return value?
    void sprintf(const char* pFmt, ...) {
        va_list varArgs;
        va_start(varArgs, pFmt);
        vasprintf(pFmt, varArgs);
        va_end(varArgs);
    }

    // CHECK ERROR BITS
    friend std::istream& operator>>(std::istream& input, String &s) {
        std::string line;
        std::getline(input, line);
        s = line;
        return input;
    }

    friend void swap(String& lhs, String& rhs) {
        String temp = lhs;
        lhs = std::move(rhs);
        rhs = std::move(temp);
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
    StringView(static_cast<const GblString&>(string)) {}

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
