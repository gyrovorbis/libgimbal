#ifndef GIMBAL_CLASS_HPP
#define GIMBAL_CLASS_HPP

#include "gimbal_class.h"

#include <type_traits.h>

namespace gimbal {

template<typename CRTP,
         typename SelfType,
         typename CClassType=GblClass,
         typename Base=std::false_type>
    class Class: public Base<CRTP, SelfType, CClassType, Base> {
private:
    CClassType* pClass = nullptr;
public:
    Class(void) = default;
    Class(const Class& other);
    Class(Class&& other);
    ~Class(void);

    Class& operator=(const Class& other);
    Class& operator=(const Class&& other);

    operator bool() const;

    bool isValid() const;
    bool isDefault() const;
    bool isOverridden() const;
    bool isInterface() const;
    bool isInterfaceImpl() const;
    bool isOwned() const;
    bool isInPlace() const;

    Type typeOf();

    size_t refCount(void) const;

    template<typename T>
    Class<T> super() const;

    template<typename T>
    Class<T> defaulted() const;

    template<typename T, typename C>
    T* privateData();

    friend bool operator==(const Class& other);
    friend bool operator!=(const Class& other);

};

template<typename SelfType,
         typename CClassType=GblInterface>
class Interface: public Class<Interface, GblInterface> {
public:

    bool isValid() { return /* TRY INTERFACE */ && Class::isValid() }

    template<typename T>
    T* outerClass();

    template<typename T>
    T* outerMostClass();

};


#endif // GIMBAL_CLASS_HPP
