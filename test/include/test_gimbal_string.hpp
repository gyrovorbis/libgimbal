#ifndef TEST_GIMBAL_STRING_HPP
#define TEST_GIMBAL_STRING_HPP

#include <elysian_qtest.hpp>
#include <gimbal/gimbal_types.hpp>
#include <gimbal/gimbal_string.h>

namespace gimbal {


GBL_API gblStringConstruct(GblString* pString, GblSize size, GblContext hCtx, const GblStringView* pView);

GBL_API gblStringSet(GblString* pStr, const GblStringView* pStrView)





typedef struct GblStringData {
    char*       pBuffer;
    GblSize     length;
    GblSize     capacity;
} GblStringData;

typedef struct GblString {
    GblContext hCtx;
    GblStringData data;
    char    stackBuffer[GIMBAL_STRING_BUFFER_STACK_SIZE];
} GblString;

    template<typename P, typename CRTP>
    class TypeCompatible {

    };

    /* Use variable-length array in C to increase stack size via alloca (but then I guess we gotta store it too)
     * use template argument in C++ to inherit and increase stack size...
     *
     */

// overflow when copying shit into the bitch without enough room on stack when no context is present
    template<GblSize ExtraStackSize=0>
    class String final: public GblString {
    public:
        using ThisType = String<ExtraStackSize>;
        constexpr static const inline ExtraStackSize = ExtraStackSize;
    private:
        char extraStackBuffer_[ExtraStackSize] = { '0' };
    public:
        constexpr String(Context* pCtx=nullptr): String(std::string_view{}, pCtx) {}
        //constexpr String(Context* pCtx, const char* pFmt, ...);

        constexpr String(const std::string& stdString, Context* pCtx=nullptr): String(std::string_view{stdString.c_str()}, pCtx) {}
        // make most shit go through here!
        constexpr String(std::string_view stringView, Context* pCtx=nullptr) {
            Result::tryThrow(gblStringConstruct(this, sizeof(String), pCtx? *pCtx : nullptr, GblStringView{stringView.data(), stringView.length()}));
        }
        // do it via conversion operator to std::string_view

        template<GblSize OtherStackSize>
        constexpr String(const String<OtherStackSize>& rhs, Context* pCtx=nullptr):
            String(getStringView(), pCtx? pCtx : rhs.getContext()) {}
// have to check stack sizes
        constexpr String(const GblString& rhs, Context* pCtx) {

            Result::tryThrow(gblStringConstruct(this, sizeof(String), ))
        }
        

        //template<GblSize OtherStackSize> do it via conversion operator to GblString&&
        //constexpr String(String<OtherStackSize>&& rhs);
// have to check stack sizes
        constexpr String(GblString&& gblString);

        ~String(void) {
            Result::tryThrow(gblStringDestruct(this));
        }

        constexpr bool operator==(const GblString& rhs) const {
            const char* pCStr = nullptr;
            Result::tryThrow(gblStringCStr(&rhs, &pCStr));
            return *this == pCstr;
        }

        constexpr bool operator==(std::string_view view) const {
            if(isNull()) {
                if(!view.length()) return true;
                else return false;
            } else {
                if(!view.length()) return false;
                else GBL_LIKELY return strncmp(getCStr(), view.data(), std::min(getLength(), view.size()));
            }
        }

        constexpr bool operator==(const char* pCStr) const {
            if(isNull()) {
                if(!pCStr) return true;
                else return false;
            } else {
                if(!pCStr) return false;
                else GBL_LIKELY return *this == std::string_view{ pCstr };
            }
        }

        constexpr bool operator==(const std::string& rhs) const {
            return *this == rhs.c_str();
        }

        const ThisType& operator+=(std::string_view view) {
            Result::tryThrow(gblStringCat(this, { view.data(), static_cast<GblSize>(view.size()) } ));
            return *this;
        }

        //std::string, GblString

        const char* getCStr(void) const {
            const char* pCstr = nullptr;
            Result::tryThrow(gblStringCStr(this, &pCstr));
            return pCstr;
        }

        Size getStackSize(void) const {
            GblSize size = 0;
            Result::tryThrow(gblStringStackSize(this, &size));
            return size;
        }

        constexpr Size getLength(void) const {
            Size length = 0;
            Result::tryThrow(gblStringLength(this, &length));
            return length;
        }
        constexpr GblSize getCapacity(void) const {
            Size capacity = 0;
            Result::tryThrow(gblStringCapacity(this, &capacity));
            return capacity;
        }

        Context* getContext(void) const {
            Context* pCtx = nullptr;
            Result::tryThrow(gblStringContext(pStr, &pCtx));
            return pCtx;
        }

        constexpr bool isValid(void) const {
            GblBool valid = GBL_FALSE;
            Result::tryThrow(gblStringIsValid(this, &valid));
            return valid;
        }
        constexpr bool isEmpty(void) const {
            GblBool empty = GBL_TRUE;
            Result::tryThrow(gblStringIsEmpty(this, &empty));
            return empty;
        }
        constexpr bool isNull(void) const {
            GblBool null = GBL_TRUE;
            Result::tryThrow(gblStringIsNull(this, &null));
            return null;
        }
        constexpr bool isStack(void) const {
            GblBool stack = GBL_FALSE;
            Result::tryThrow(gblStringIsStack(this, &stack));
            return stack;
        }
        constexpr bool isHeap(void) const {
            return !isStack();
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
            vasnprintf(pCtx, pFmt, varArgs);
            va_end(varArgs);
        }

    };

    std::ostream <<


}


namespace gimbal::test {

class String: public elysian::UnitTestSet {


};

}

#endif // TEST_GIMBAL_STRING_HPP
