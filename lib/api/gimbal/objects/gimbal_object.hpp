#ifndef GIMBAL_OBJECT_HPP
#define GIMBAL_OBJECT_HPP

#include "../meta/gimbal_instance.hpp"
#include "gimbal_object.h"

namespace gimbal {

template<typename CRTP>
class ObjectBase: InstanceBase<CRTP> {


};

class Object: public ObjectBase<Object> {
private:


    Object(void) = delete;


};




}

#endif // GIMBAL_OBJECT_HPP
