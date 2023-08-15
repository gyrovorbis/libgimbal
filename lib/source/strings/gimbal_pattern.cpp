#include <gimbal/strings/gimbal_pattern.hpp>
#include <gimbal/strings/gimbal_string_buffer.h>

namespace gbl {

std::string Pattern::string() const noexcept {
    GblStringBuffer* pBuff = reinterpret_cast<GblStringBuffer*>(GBL_ALLOCA(sizeof(GblStringBuffer)));

    GblStringBuffer_construct(pBuff, GBL_STRV(""), sizeof(GblStringBuffer));

    std::string cppStr (GblPattern_string(*this, pBuff));

    GblStringBuffer_destruct(pBuff);

    return cppStr;
}

}
