#ifndef GIMBAL_PATTERN_HPP
#define GIMBAL_PATTERN_HPP

#include "gimbal_pattern.h"
#include <string>
#include <iostream>

namespace gbl {

class Pattern {
private:
    const GblPattern* pPattern_ = nullptr;

public:
    Pattern(const char* pRegExp) noexcept:
        pPattern_(GblPattern_create(pRegExp)) {}

    Pattern(const GblPattern* pPattern) noexcept:
        pPattern_(pPattern) {}

    Pattern(const Pattern& rhs) noexcept:
        pPattern_(GblPattern_ref(rhs.pPattern_)) {}

    Pattern(Pattern&& rhs) noexcept:
        pPattern_(rhs.pPattern_)
    {
        rhs.pPattern_ = nullptr;
    }

    ~Pattern() noexcept {
        GblPattern_unref(pPattern_);
    }

    bool isValid() const noexcept { return pPattern_; }

    std::size_t refCount() const noexcept { return GblPattern_refCount(pPattern_); }

    std::size_t bytes() const noexcept { return GblPattern_bytes(pPattern_); }

    std::string string() const noexcept;

    operator const GblPattern*() const noexcept { return pPattern_; }

    bool match(const char*    pString,
               GblStringView* pView=nullptr,
               int*           pCount=nullptr) const noexcept
    {
        return GblPattern_match(*this, pString, pView, pCount);
    }

    bool matchNot(const char*    pString,
                  GblStringView* pView=nullptr,
                  int*           pCount=nullptr) const noexcept
    {
        return GblPattern_matchNot(*this, pString, pView, pCount);
    }

    bool matchExact(const char* pString) const noexcept {
        return GblPattern_matchExact(*this, pString);
    }

    bool matchCount(const char* pString) const noexcept {
        return GblPattern_matchCount(*this, pString);
    }
};

inline auto operator<=>(const Pattern& lhs, const GblPattern* pRhs) noexcept {
    const auto result = GblPattern_compare(lhs, pRhs);

    if(result < 0)
        return std::strong_ordering::less;
    else if(result > 0)
        return std::strong_ordering::greater;
    else
        return std::strong_ordering::equal;
}

}

#endif // GIMBAL_PATTERN_HPP
