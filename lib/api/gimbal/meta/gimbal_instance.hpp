#ifndef GIMBAL_INSTANCE_HPP
#define GIMBAL_INSTANCE_HPP

#include "gimbal_instance.h"
#include "gimbal_type.hpp"

namespace gimbal {

class ClassRef;

template<typename CRTP>
class ClassBase {
protected:
    GblClass* class_(void) {
        return static_cast<CRTP*>(this)->instancePtr_();
    }

    const GblClass* class_(void) const {
        return static_cast<const CRTP*>(this)->instancePtr_();
    }
public:
    Type getType(void) const {
        return GblType_fromClass(class_());
    }

    RefCount getRefCount(void) const {
        return GblClass_refCount(class_());
    }
};


class ClassRef: public ClassBase<ClassRef> {
private:
    GblClass* pClass_ = nullptr;
public:
    const GblClass* instancePtr_(void) const {
        return pClass_;
    }

    GblClass* instancePtr_(void) {
        return pClass_;
    }

    ClassRef(GblClass* pClass):
        pClass_(GblClass_ref(pClass)) {}

    ~ClassRef(void) {
        GblClass_unref(pClass_);
    }
};



template<typename CRTP>
class InstanceBase {
protected:
    GblInstance* instance_(void) {
        return static_cast<CRTP*>(this)->instancePtr_();
    }

    const GblInstance* instance_(void) const {
        return static_cast<const CRTP*>(this)->instancePtr_();
    }

public:

    Type getType(void) const {
        return GBL_INSTANCE_TYPE(instance_());
    }

    ClassRef getClass(void) const {
        return GblInstance_classOf(instance_());
    }
};

}

#endif // GIMBAL_INSTANCE_HPP
