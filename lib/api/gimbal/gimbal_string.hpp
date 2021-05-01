#ifndef GIMBAL_STRING_HPP
#define GIMBAL_STRING_HPP

#include <gimbal/gimbal_types.hpp>
#include <gimbal/gimbal_string.h>
#include <gimbal/gimbal_context.hpp>
#include <iostream>


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

//ADD CONST ITERATORS
template<typename CRTP>
class StringViewBase {
public:
    using StringViewType = StringViewBase<CRTP>;
protected:
    decltype(auto) str_(void) const {
        return static_cast<const CRTP*>(this)->getString_();
    }

public:

// Accessor wrappers

    const char* getCStr(void) const {
        const char* pCstr = nullptr;
        Result::tryThrow(gblStringCStr(str_(), &pCstr));
        return pCstr ? pCstr : "";
    }

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
#if 0
    constexpr bool isNull(void) const {
        GblBool null = GBL_TRUE;
        Result::tryThrow(gblStringIsNull(str_(), &null));
        return null;
    }
#endif

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

    std::string toStdString(void) const {
        return std::string(getCStr());
    }

    constexpr std::string_view toStringView(void) const {
        return getLength()?
            std::string_view(getCStr(), getLength()) : std::string_view();
    }

    char operator[](size_t index) const {
        if(index >= getLength()) Result::tryThrow(Result::ErrorOutOfBounds);
        return getCStr()[index];
    }

    template<typename Other_CRTP>
    friend constexpr decltype(auto) operator<=>(const StringViewType& lhs, const StringViewBase<Other_CRTP>& rhs) {
        return lhs.toStringView() <=> rhs.toStringView();
    }

    friend constexpr decltype(auto) operator<=>(const StringViewType& lhs, const std::string& rhs) {
        return lhs <=> rhs.c_str();
    }

    friend constexpr decltype(auto) operator<=>(const StringViewType& lhs, const char* pRhs) {
        return lhs <=> std::string_view(pRhs);
    }

    friend constexpr decltype(auto) operator<=>(const StringViewType& lhs, const std::string_view& rhs) {
        return lhs.toStringView() <=> rhs;
    }

    friend std::ostream &operator<<(std::ostream& output, const StringViewType& s) {
        return output << s.getCStr();
    }

#if 0
    struct iterator
       {
           using value_type = T ;
           using reference = T& ;
           using pointer = T* ;
           using difference_type = std::ptrdiff_t ;
           using iterator_category	= std::forward_iterator_tag ;

           reference operator* () { return *curr ; }
           // pointer operator& () { return &**this ; }
           pointer operator-> () { return &**this ; } // *** EDIT

           iterator& operator++ () { ++curr ; return *this ; }
           iterator operator++ (int) { const auto temp(*this) ; ++*this ; return temp ; }

           bool operator== ( const iterator& that ) const { return curr == that.curr ; }
           bool operator!= ( const iterator& that ) const { return !(*this==that) ; }

           iterator& next_row()
           {
               if( std::distance( curr, end ) >= difference_type(ncols) ) std::advance( curr, ncols ) ;
               else curr = end ;
               return *this ;
           }

           typename std::vector<T>::iterator curr ;
           typename std::vector<T>::iterator end ;
           std::size_t ncols ;

       };

       iterator begin() { return { std::begin(items), std::end(items), ncols } ; }
       iterator end() { return { std::end(items), std::end(items), ncols } ; }
#endif
};

class StringView final: public StringViewBase<StringView> {
private:
    const GblString* pGblStr_ = nullptr;
public:
    StringView(const GblString* pGblStr = nullptr):
        pGblStr_(pGblStr) {}

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
protected:

    std::pair<char*, Size> take(void) {
        char* pCStr = nullptr;
        Size capacity = 0;
        Result::tryThrow(gblStringTake(this, &pCStr, &capacity));
        return { pCStr, capacity };
    }

    void give(std::pair<char*, Size> data) {
        Result::tryThrow(gblStringGive(this, data.first, data.second));
    }

public:

    const GblString* getString_(void) const {
        return static_cast<const GblString*>(this);
    }

    // Default Ctors
    String(std::nullptr_t, Size size=sizeof(String)):
        String(static_cast<Context*>(nullptr), size) {}

    String(Context* pCtx=nullptr, Size size=sizeof(String)):
        String(std::string_view{}, pCtx, size) {}

    // Value Ctors
    String(const std::string& stdString, Context* pCtx=nullptr, Size size=sizeof(String)):
        String(std::string_view{stdString.c_str()}, pCtx) {}

    String(const char* pCStr, Context* pCtx=nullptr, Size size=sizeof(String)):
        String(pCStr? std::string_view{pCStr} : std::string_view{}, pCtx, size) {}

    // Copy Ctors
    //Hopefully doing a regular-ass GblString will automatically instantiate a GblStringView here?
    template<typename T>
    constexpr String(const StringViewBase<T>& other, Context* pCtx=nullptr, Size size = sizeof(String)):
        String(other.toStringView(), pCtx? pCtx : other.getContext(), size) {}

    String(const std::string_view& stringView, Context* pCtx=nullptr, Size size=sizeof(String)) {
        GblStringView tempView = {stringView.data(), stringView.length()};
        Result::tryThrow(gblStringConstruct(this,
                                            size,
                                            pCtx? static_cast<GblContext>(*pCtx) : nullptr,
                                            &tempView));
    }

    String(String&& rhs, Context* pCtx=nullptr, Size size=sizeof(String)):
        String(pCtx, size)
    {
        pCtx = pCtx? pCtx : rhs.getContext();

        if(pCtx == rhs.getContext() && rhs.isHeap()) {
            give(rhs.take());
        } else { //cannot move construct!
            *this = rhs.toStringView();
        }
    }

    ~String(void) {
        Result::tryThrow(gblStringDestruct(this));
    }

    void clear(void) {
        Result::tryThrow(gblStringClear(this));
    }

    const String& operator=(const std::string_view& view) {
        GblStringView gview = { view.data(), view.size() };
        Result::tryThrow(gblStringAssign(this, &gview));
        return *this;
    }

    const String& operator=(const String& string) {
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

    const String& operator+=(std::string_view view) {
        GblStringView gview = { view.data(), view.size() };
        Result::tryThrow(gblStringCat(this, &gview));
        return *this;
    }

    void reserve(Size capacity) {
        Result::tryThrow(gblStringReserve(this, capacity));
    }

    void resize(Size size) {
        Result::tryThrow(gblStringResize(this, size));
    }

    void vasnprintf(const char* pFmt, va_list varArgs) {
        Result::tryThrow(gblStringVaSnprintf(this, pFmt, varArgs));
    }

    //return the actual return value?
    void snprintf(const char* pFmt, ...) {
        va_list varArgs;
        va_start(varArgs, pFmt);
        //retval?
        vasnprintf(pFmt, varArgs);
        va_end(varArgs);
    }

    // CHECK ERROR BITS
    friend std::istream &operator>>(std::istream& input, String &s) {
        std::string line;
        //(input.rdState() & std::ifstream::failbit)
        std::getline(input, line);
        s = line;
        return input;
    }

    //overloaded index operator with proxy for setting value

    friend bool operator==(const String& lhs, const char* pCStr) {
        return (pCStr && strcmp(lhs.getCString(), pCStr) == 0);
    }

    friend std::weak_ordering operator<=>(const String& lhs, const std::string& str) {
        return lhs.toStringView() <=> std::string_view(str.c_str());
    }
};


/* Use variable-length array in C to increase stack size via alloca (but then I guess we gotta store it too)
 * use template argument in C++ to inherit and increase stack size...
 *
 */

// overflow when copying shit into the bitch without enough room on stack when no context is present
template<Size ExtraStackSize>
class StringExt final: public String {
public:
    using StringExtType = StringExt<ExtraStackSize>;
    constexpr static const inline Size ExtraStackBufferSize = ExtraStackSize;
private:
    char extraStackBuffer_[ExtraStackSize] = { '0' };
public:

};

}

#endif // GIMBAL_STRING_HPP
