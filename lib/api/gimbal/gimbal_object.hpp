#ifndef GIMBAL_OBJECT_HPP
#define GIMBAL_OBJECT_HPP

#include "gimbal_object.h"
#include "gimbal_types.hpp"

namespace gimbal {

class Object: public PrimitiveBase<GblObject>{

public:
    using Invalid = std::nullptr_t;

    void getMetaType(void) const;
    bool isValid(void) const;

};


}


#endif // GIMBAL_OBJECT_HPP
