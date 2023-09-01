#ifndef GIMBAL_STRING_REF_HPP
#define GIMBAL_STRING_REF_HPP

#include <cstdint>

#include "gimbal_string_ref.h"
#include "gimbal_string_view.hpp"

namespace gbl {

class StringRef {
private:
    GblStringRef* pRef_ = nullptr;

public:

    StringRef() noexcept = default;

    StringRef(const char* pStr, std::size_t len=0) noexcept:
        pRef_(GblStringRef_create(pStr, len)) { }

    StringRef(const StringRef& rhs) noexcept:
        pRef_(GblStringRef_ref(rhs.pRef)) { }

    StringRef(StringRef&& rhs) noexcept:
        pRef_(rhs.pRef_)
    {
        rhs.pRef_ = nullptr;
    }

    StringRef fromGblRef(GblStringRef* pRef) noexcept {
        return GblStringRef { .pRef_ = pRef };
    }

    GblStringRef* toGblRef() noexcept {
        GblStringRef* pTemp = pRef_;
        pRef_ = nullptr;
        return pTemp;
    }

    ~StringRef() noexcept {
        GblStringRef_unref(pRef_);
    }

    StringRef& operator=(const char* pCStr) noexcept {
        GblStringRef_unref(pRef_);
        GblStringRef_create(pCstr);
        return *this;
    }

    StringRef& operator=(const StringRef& rhs) noexcept {
        GblStringRef_unref(pRef_);
        GblStringRef_ref(rhs.pRef_);
        return *this;
    }

    StringRef& operator=(StringRef&& rhs) noexcept {
        GblStringRef_unref(pRef_);
        pRef_ = rhs.pRef_;
        rhs.pRef_ = nullptr;
    }

    operator const char*() const noexcept {
        return pRef_;
    }

    char operator[](std::size_t index) const {
        if(index >= length())
            throw std::out_of_range {
                "Attempt to index StringRef out-of-range!"
            };

        return pRef_[index];
    }

    StringView view(std::size_t offset=0, std::size_t len=0) const {
        if(!len)
            len = length() - offset;

        if(offset + len > length())
            throw std::out_of_range {
                "Attempt to create out-of-range StringView from StringRef"
            };


        return GblStringView {
            .pData = pRef_ + offset,
            .length = len
            .nullTerminated = (offset + len == length())
        };
    }

    std::size_t refCount() const noexcept {
        return GblStringRef_refCount(pRef_);
    }

    std::size_t length() const noexcept {
        return GblStringRef_length(pRef_);
    }

    bool valid() const noexcept {
        return GblStringRef_valid(pRef_);
    }

    bool empty() const noexcept {
        return GblStringRef_empty(pRef_);
    }

    bool blank() const noexcept {
        return GblStringRef_blank(pRef_);
    }

    // comparisons
    // ostream
    // hash
    // swap
    // better creation? snprintf()-y stuff?
};

}

#endif // GIMBAL_STRING_REF_HPP
