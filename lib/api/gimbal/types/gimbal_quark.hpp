#ifndef GIMBAL_QUARK_HPP
#define GIMBAL_QUARK_HPP

#include "../core/gimbal_api_frame.hpp"
#include "gimbal/core/gimbal_api_generators.hpp"
#include "gimbal_quark.h"
#include "gimbal_string.hpp"
#include <string>

namespace gimbal {

// SHARE BASE CLASS AND OPERATORS WITH GBLSTRING!!!
    class Quark: PrimitiveBase<GblQuark> {

        Quark(auto&& v);

        static Quark fromStaticString(const char* pStatic);

        static const char* intern(auto value);
        static const char* internStatic(const char* pValue);

        void setStaticString(const char* pStatic);
        void setString(auto&& v);


        const Quark& operator=(GblQuark quark);
        const Quark& operator=(std::nullptr_t);
        const Quark& operator=(const char* pString);
        const Quark& operator=(const std::string& stdString);
        const Quark& operator=(const std::pmr::string& stdPmrString);
        const Quark& operator=(const std::string_view& stringView);
        const Quark& operator=(const String& gblString);
        const Quark& operator=(const StringView& gblStringView);



    };


    inline Quark::Quark(auto&& v) {
        *this = std::forward<decltype(v)>(v);
    }

}


callFunc(Quark::intern("fuckYou"));


#endif // GIMBAL_QUARK_HPP
