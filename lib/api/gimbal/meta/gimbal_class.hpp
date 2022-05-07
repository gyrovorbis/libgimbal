#ifndef GIMBAL_CLASS_HPP
#define GIMBAL_CLASS_HPP

#include "gimbal_class.h"
#include "../types/gimbal_typedefs.hpp"
#include "gimbal_type.hpp"
#include <concepts>

namespace gimbal {

class Class: public GblClass {
public:

    const char*     getName(void) const;
    Type            getType(void) const;
    RefCount        getRefCount(void) const;
};
#if o
template<typename C>
   // requires std::is_base_of_v<Class, C>
class ClassRef {
private:
    C*      pClass_ = nullptr;

    static Class* toClass(C* pPtr) { return reinterpret_cast<Class*>(pPtr); }

public:
    ClassRef(C* pClass):
        pClass_(pClass? Type::classReference(toClass(pClass)) : nullptr) {}

    ~ClassRef(void) {
        if(pClass_) pClass_->unref();
    }

    bool isValid(void) const { return pClass_; }
    C* operator->(void) { return pClass_; }

};
#endif


inline const char* Class::getName(void) const {
    return GblType_name(getType());
}
inline Type Class::getType(void) const {
    return GblType_fromClass(this);
}
inline RefCount Class::getRefCount(void) const {
    return GblClass_refCountFromType(getType());
}




}

#endif // GIMBAL_CLASS_HPP
