#ifndef GIMBAL_STRING_HPP
#define GIMBAL_STRING_HPP

#include <gimbal/types/gimbal_string.h>
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

//support std::pmr::string ?
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

    operator const GblString*() const { return str_(); }


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
        Size size = 0;
        Exception::checkThrow(gblStringStackBytes(str_(), &size));
        return size;
    }

    constexpr Size getLength(void) const {
        Size length = 0;
        Exception::checkThrow(gblStringLength(str_(), &length));
        return length;
    }

    constexpr Size getCapacity(void) const {
        Size capacity = 0;
        Exception::checkThrow(gblStringCapacity(str_(), &capacity));
        return capacity;
    }

    Context* getContext(void) const {
        GblContext* pCtx = nullptr;
        Exception::checkThrow(gblStringContext(str_(), &pCtx));
        return pCtx == nullptr? nullptr : Context::fromGblObj(pCtx);
    }

    constexpr bool isEmpty(void) const {
        GblBool empty = GBL_TRUE;
        Exception::checkThrow(gblStringIsEmpty(str_(), &empty));
        return empty;
    }

    constexpr bool isStack(void) const {
        GblBool stack = GBL_FALSE;
        Exception::checkThrow(gblStringIsStack(str_(), &stack));
        return stack;
    }

    // C++-specific utils

    constexpr bool isHeap(void) const {
        return !isStack();
    }

    const char* getCString(void) const {
        const char* pCStr = nullptr;
        Exception::checkThrow(gblStringCStr(str_(), &pCStr));
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
    friend constexpr bool operator==(const Derived& lhs, const std::pmr::string& rhs) noexcept {
        return (lhs == rhs.c_str());
    }
    friend constexpr decltype(auto) operator<=>(const Derived& lhs, const std::pmr::string& rhs) noexcept {
        return (lhs.toStdString() <=> rhs);
    }
    friend constexpr bool operator==(const Derived& lhs, std::string_view rhs) noexcept {
        return (lhs.toStringView() == rhs);
    }
    friend constexpr decltype(auto) operator<=>(const Derived& lhs, std::string_view rhs) noexcept {
        return (lhs.toStringView() <=> rhs);
    }
    friend constexpr bool operator==(const Derived& lhs, const string_base auto& rhs) {
        gimbal::Int result = GBL_FALSE;
        Exception::checkThrow(gblStringCompare(&lhs, &rhs, &result));
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
    const GblString* pGblStr_ = nullptr;
public:
    StringView(void) = default;

    StringView(const GblString& gblStr):
        pGblStr_(&gblStr) {}

    StringView(const gimbal::String& string);

    StringView(const GblString* pGblString):
        pGblStr_(pGblString) {}

    const GblString* getString_(void) const { return pGblStr_; }
    bool isValid(void) const { return pGblStr_; }
};

class String:
    public GblString,
    public StringViewBase<String>
{
public:

    const GblString* getString_(void) const {
        return static_cast<const GblString*>(this);
    }
// iterator constructors!
    GblString* getString_(void) {
        return static_cast<GblString*>(this);
    }

    //operator const GblString*() const { return getString_(); }

    // Default Ctors
    String(std::nullptr_t, Size size=sizeof(String)):
        String(static_cast<Context*>(nullptr), size) {}

    String(Context* pCtx=nullptr, Size size=sizeof(String)):
        String(std::string_view{}, pCtx, size) {}

    // Value Ctors
    String(const std::string& stdString, Context* pCtx=nullptr, Size size=sizeof(String)):
        String(std::string_view{stdString.c_str()}, pCtx, size) {}

    String(const std::pmr::string& stdPmrString, Context* pCtx=nullptr, Size size=sizeof(String)):
        String(std::string_view{stdPmrString.c_str()}, pCtx, size) {}

    String(const char* pCStr, Context* pCtx=nullptr, Size size=sizeof(String)):
        String(pCStr? std::string_view{pCStr} : std::string_view{}, pCtx, size) {}

    String(const std::string_view& stringView, Context* pCtx=nullptr, Size size=sizeof(String)) {
        GblStringView tempView = {stringView.data(), stringView.length()};
        Exception::checkThrow(gblStringConstruct(this,
                                            size,
                                            pCtx,
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
        Exception::checkThrow(gblStringDestruct(this));
    }

    static std::pair<char*, Size> take(GblString* pGblStr) {
        char* pCStr = nullptr;
        Size capacity = 0;
        Exception::checkThrow(gblStringTake(pGblStr, &pCStr, &capacity));
        return { pCStr, capacity };
    }

    auto take(void) {
        return take(this);
    }

    void give(std::pair<char*, Size> data) {
        Exception::checkThrow(gblStringGive(this, data.first, data.second));
    }

    void clear(void) {
        Exception::checkThrow(gblStringClear(this));
    }

    const String& operator=(std::string_view view) {
        GblStringView gview = { view.data(), view.size() };
        Exception::checkThrow(gblStringAssign(this, &gview));
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

    const String& operator=(const std::pmr::string& stdString) {
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
        Exception::checkThrow(gblStringReserve(this, capacity));
    }

    void resize(Size size) {
        Exception::checkThrow(gblStringResize(this, size));
    }

    void insert(const_iterator pos, const char* pString, GblSize count=0) {
        GBL_ASSERT(pString);
        if(!count) count = strlen(pString);
        const auto index = std::distance(cbegin(), pos);
        const GblStringView view{pString, count};
        Exception::checkThrow(gblStringInsert(this, index, &view));
      //  return iterator(*this, index);
    }

    void concat(std::string_view view) {
        //insert(cend(), view.data(), view.size());
        Exception::checkThrow(gblVectorConcat(&this->data, view.data(), view.size()));
    }

    String& vasprintf(const char* pFmt, va_list varArgs) {
        Exception::checkThrow(gblStringVaSprintf(this, pFmt, varArgs));
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
