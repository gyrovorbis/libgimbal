#ifndef GIMBAL_QUARK_HPP
#define GIMBAL_QUARK_HPP

#include "gimbal_quark.h"
#include <string>
#include <string_view>
#include <iostream>

namespace gbl {

class Quark {
private:
    GblQuark value_ = GBL_QUARK_INVALID;

public:

    Quark(void) = default;

    Quark(GblQuark cQuark):
        value_(cQuark) {}

    Quark(const char* pString, std::size_t length=0):
        Quark(GblQuark_tryString(pString, length)) {}

    Quark(const std::string& stdStr):
        Quark(stdStr.c_str()) {}

    Quark(std::string_view strv):
        Quark(strv.data(), strv.length()) {}

    bool isValid(void) const {
        return value_ != GBL_QUARK_INVALID;
    }

    const char* toString(void) const {
        return GblQuark_toString(value_);
    }

    std::size_t length(void) const {
        return isValid()?
                    strlen(GblQuark_toString(value_)) :
                    0;
    }

    operator GblQuark(void) const {
        return value_;
    }

    char operator[](std::size_t index) const {
        if(index >= length())
            throw std::out_of_range("Out-of-range GblQuark access!");

        return toString()[index];
    }

    Quark& operator=(const Quark& rhs) = default;

    Quark& operator=(GblQuark cQuark) {
        value_ = cQuark;
        return *this;
    }

    Quark& operator=(const char* pString) {
        value_ = GblQuark_tryString(pString);
        return *this;
    }

    Quark& operator=(const std::string& stdStr) {
        return *this = stdStr.c_str();
    }

    Quark& operator=(std::string_view strv) {
        value_ = GblQuark_tryString(strv.data(), strv.length());
        return *this;
    }

    static inline GblQuark Invalid;

    static Quark fromString(const char* pString, std::size_t length=0) {
        return GblQuark_fromString(pString, length);
    }

    static Quark fromString(const std::string& stdStr) {
        return fromString(stdStr.c_str());
    }

    static Quark fromString(std::string_view strv) {
        return fromString(strv.data(), strv.length());
    }

    static Quark fromStatic(const char* pString) {
        return GblQuark_fromStatic(pString);
    }

    static const char* internString(const char* pString, std::size_t length=0) {
        return GblQuark_internString(pString, length);
    }

    static const char* internStatic(const char* pString) {
        return GblQuark_internStatic(pString);
    }

    friend std::ostream& operator<<(std::ostream &os, Quark quark) {
        os << quark.toString();
        return os;
    }
};

Quark operator""_qrk(const char* pString, std::size_t size) {
    return GblQuark_fromStatic(pString);
}

}

#endif // GIMBAL_QUARK_HPP
