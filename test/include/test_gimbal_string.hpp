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
/*
        template<GblSize OtherStackSize>
        constexpr String(const String<OtherStackSize>& rhs, Context* pCtx=nullptr);
        constexpr String(const GblString& rhs, Context* pCtx=nullptr);
        */

        template<GblSize OtherStackSize>
        constexpr String(String<OtherStackSize>&& rhs);

        constexpr String(GblString&& gblString);

        ~String(void) {
            Result::tryThrow(gblStringDestruct(this));
        }

        constexpr bool operator==(const GblString& rhs) const {
        }

        const String& operator+=(std::string_view view) {
            Result::tryThrow(gblStringCat(this, { view.data(), static_cast<GblSize>(view.size()) } ));
            return *this;
        }

        const char* getCStr(void) const {
            const char* pCstr = nullptr;
            Result::tryThrow(gblStringCStr(this, &pCstr));
            return pCstr;
        }

        GblSize getStackSize(void) const {
            GblSize size = 0;
            Result::tryThrow(gblStringStackSize(this, &size));
            return size;
        }

        constexpr GblSize getLength(void) const {
            GblSize length = 0;
            Result::tryThrow(gblStringLength(this, &length));
            return length;
        }
        constexpr GblSize getCapacity(void) const {
            GblSize capacity = 0;
            Result::tryThrow(gblStringCapacity(this, &capacity));
            return capacity;
        }

        constexpr Context* getContext(void) const {
            Context* pCtx;
            gblStringContext(pStr, &pCtx);
        }

        constexpr bool isValid(void) const {
            gblStringIsValid(const GblString* pStr, GblBool* pResult);
        }
        constexpr bool isEmpty(void) const {
            GBL_API gblStringIsEmpty(const GblString* pStr, GblBool* pResult);
        }
        constexpr bool isNull(void) const {
            gblStringIsNull(const GblString* pStr, GblBool* pResult);
        }
        constexpr bool isStack(void) const {
                    GBL_API gblStringIsStack(const GblString* pStr);
        }
        constexpr bool isHeap(void) const {
            return !isStack();
        }

        void reserve(GblSize capacity) {
            GblStringReserve(const GblString* pStr, GblSize capacity);
        }
        void resize(GblSize size) {
            gblStringResize(const GblString* pStr, GblSize length);
        }

        void vasnprintf(const char* pFmt, va_list varArgs) {
            GblStringVaSnprintf(GblString* pStr, pFmt, varArgs);
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
