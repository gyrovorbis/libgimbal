#ifndef TEST_GIMBAL_STRING_HPP
#define TEST_GIMBAL_STRING_HPP

#include <elysian_qtest.hpp>
#include <gimbal/gimbal_types.hpp>
#include <gimbal/gimbal_string.h>
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
            return static_cast<CRTP*>(this)->getString_();
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
            Context* pCtx = nullptr;
            Result::tryThrow(gblStringContext(str_(), &pCtx));
            return pCtx;
        }

        constexpr bool isEmpty(void) const {
            GblBool empty = GBL_TRUE;
            Result::tryThrow(gblStringIsEmpty(str_(), &empty));
            return empty;
        }

        constexpr bool isNull(void) const {
            GblBool null = GBL_TRUE;
            Result::tryThrow(gblStringIsNull(str_(), &null));
            return null;
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

        constexpr std::string toStdString(void) const {
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
        friend constexpr decltype(auto) operator<=>(const ThisType& lhs, const StringViewBase<Other_CRTP>& rhs) {
            return lhs.toStringView() <=> rhs.toStringView();
        }

        friend constexpr decltype(auto) operator<=>(const ThisType& lhs, const std::string& rhs) {
            return lhs <=> rhs.c_str();
        }

        friend constexpr decltype(auto) operator<=>(const ThisType& lhs, const char* pRhs) {
            return lhs <=> std::string_view(pRhs);
        }

        friend constexpr decltype(auto) operator<=>(const ThisType& lhs, const std::string_view& rhs) {
            return lhs.toStringView() <=> rhs;
        }
        
        friend std::ostream &operator<<(std::ostream& output, const StringViewType& s) {
            return output << s.getCStr();
        }
    };

    class StringView final: public StringViewBase<StringView> {
    private:
        const GblString* pGblStr_ = nullptr;
    public:
        StringView(const GblString* pGblStr = nullptr):
            pGblStr(pGblStr_) {}

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

        std::pair<const char*, Size> take(void) {
            auto data = std::pair()
            const char* pCStr = nullptr;
            Size capacity = 0;
            Result::tryThrow(gblStringTake(this, &pCstr, &capacity));
            return { pCstr, capacity };
        }

        void give(std::pair<const char*, Size> data) {
            Result::tryThrow(gblStringMove(this, data.first(), data.second()));
        }

    public:

        // Default Ctors
        constexpr String(std::nullptr_t, Size size=sizeof(String)):
            String(nullptr, size) {}

        constexpr String(Context* pCtx=nullptr, Size size=sizeof(String)): 
            String(std::string_view{}, pCtx, size) {}

        // Value Ctors
        constexpr String(const std::string& stdString, Context* pCtx=nullptr, Size size=sizeof(String))): 
            String(std::string_view{stdString.c_str()}, pCtx) {}

        constexpr String(const char* pCStr, Context* pCtx=nullptr, Size size=sizeof(String)): 
            String(pCStr? std::string_view{pCstr} ? std::string_view{}, pCtx, size) {}

        // Copy Ctors
        //Hopefully doing a regular-ass GblString will automatically instantiate a GblStringView here?
        template<typename T>
        constexpr String(const StringViewBase<T>& other, Context* pCtx=nullptr, Size size = sizeof(String)):
            String(other.toStringView(), pCtx? pCtx : other.getContext(), size);
        }

        constexpr String(const std::string_view& stringView, Context* pCtx=nullptr, Size size=sizeof(String)) {
            Result::tryThrow(gblStringConstruct(this,
                                                size,
                                                pCtx? static_cast<GblContext>(*pCtx) : nullptr,
                                                GblStringView{stringView.data(), stringView.length()})
                             );

        constexpr String(String&& rhs, Context* pCtx=nullptr, Size size=sizeof(String)):
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
            Result::tryThrow(gblStringAssign(this, { view.data(), view.size() }));
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
        const String& operator=(nullptr) {
            clear();
            return *this;
        }

        const String& operator+=(std::string_view view) {
            Result::tryThrow(gblStringCat(this, { view.data(), static_cast<GblSize>(view.size()) } ));
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

        void snprintf(const char* pFmt, ...) {
            va_list varArgs;
            va_start(varArgs, pFmt);
            vasnprintf(pFmt, varArgs);
            va_end(varArgs);
        }

        friend std::istream &operator>>(std::istream& input, String &s) {
            return input >> s.getCStr();
        }

        //overloaded index operator with proxy for setting value
    };



    /* Use variable-length array in C to increase stack size via alloca (but then I guess we gotta store it too)
     * use template argument in C++ to inherit and increase stack size...
     *
     */

// overflow when copying shit into the bitch without enough room on stack when no context is present
    template<Size ExtraStackSize>
    class StringExt final: public String {
    public:
        using StringExtType = String<ExtraStackSize>;
        constexpr static const inline Size ExtraStackBufferSize = ExtraStackSize;
    private:
        char extraStackBuffer_[ExtraStackSize] = { '0' };
    public:





    };




}


namespace gimbal::test {

class String: public elysian::UnitTestSet {


};

}

#endif // TEST_GIMBAL_STRING_HPP
