#include <gimbal/strings/gimbal_pattern.hpp>
#include <gimbal/strings/gimbal_string_buffer.h>

namespace gbl {

std::string Pattern::string() const noexcept {
    struct {
        GblStringBuffer buff;
        char            stackBytes[128];
    } str;

    GblStringBuffer_construct(&str.buff, GBL_STRV(""), sizeof(str));

    std::string cppStr = GblPattern_string(*this, &str.buff);

    GblStringBuffer_destruct(&str.buff);

    return cppStr;
}

}
