#ifndef GIMBAL_OBJECT_HPP
#define GIMBAL_OBJECT_HPP

#include "gimbal_object.h"
#include "gimbal_types.hpp"
//#include "gimbal_Varienter.hpp"

namespace gimbal {

struct Varienter{};

//check to see that they're related via MetaObject hierarchy traversal!
template<typename T>
T* gblobject_cast(auto& obj) {
    return nullptr;
}

class MetaObject;

class Object:
        public PrimitiveBase<GblObject> /*,
       public ReadWriteContiguousIndexable<Object, Varienter, Varienter>,*/
        //public RandomAccessIterable<Object, Varienter, Varienter>
{

public:
    using Invalid = std::nullptr_t;

                        Object(void) = default;
    virtual             ~Object(void) = default;

    const MetaObject*   getMetaObject(void) const;

    // map ReadWriteContiguousIndexable/RandomAccessIterable to these!
    Varienter             getProperty(const Varienter& key) const;
    bool                setProperty(const Varienter& key, Varienter value) const;
    auto                nextProperty(const Varienter& prevKey) const -> std::pair<Varienter, Varienter>;

    template<typename R>
    R                   virtualCall(gimbal::Enum methodId, auto&&... args) const;

    template<typename R>
    R                   virtualCallParent(gimbal::Enum methodId, auto&&... args) const;





    const Varienter getElement_(const Varienter& key) const {
        return getProperty(key);
    }
/*
    Varienter& getElement_(Varienter key) {
        return getProperty()
    }
*/
    void setElement_(const Varienter& key, Varienter value) {
        setProperty(key, std::move(value));
    }

    size_t getElementCount_(void) const {
        return 0; //?
    }





};


}


#endif // GIMBAL_OBJECT_HPP
