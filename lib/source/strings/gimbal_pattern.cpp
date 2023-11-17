#include <gimbal/strings/gimbal_pattern.hpp>
#include <gimbal/strings/gimbal_string_buffer.h>

namespace gbl {

std::string Pattern::string() const noexcept {
    GblStringBuffer buff;

    GblStringBuffer_construct(&buff);

    std::string cppStr (GblPattern_string(*this, &buff));

    GblStringBuffer_destruct(&buff);

    return cppStr;
}

}
