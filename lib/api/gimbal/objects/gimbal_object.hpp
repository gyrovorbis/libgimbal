#ifndef GIMBAL_OBJECT_HPP
#define GIMBAL_OBJECT_HPP


#if 1

#include "gimbal_object.h"
#include "../types/gimbal_typedefs.hpp"
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


#else
#include "gimbal_object.h"
#include "../types/gimbal_typedefs.hpp"
#include "../meta/gimbal_meta.hpp"
//#include "../types/gimbal_variant.hpp"
#include <tuple>

namespace gimbal {

//check to see that they're related via MetaObject hierarchy traversal!
template<typename T>
T* gblobject_cast(auto& obj) {
    return nullptr;
}

class MetaObject;
class Object;
class ContextView;


using ObjectPropertyPair = std::pair<Variant, Variant>;


//ObjectView = ObjectStatic?


struct ObjectVTable: public GblObjectVTable {};


template<typename CType     = GblObject,
         typename CppType   = Object>
class ObjectView: public PrimitiveBase<GblObject> {
public:

    using           ObjectCType     = CType;
    using           ObjectCppType   = CppType;
    //using           ObjectVTable    = ObjectVTable;

                    ObjectView(CppType* pObj);
                    ObjectView(CType hObj);

    CType           getObjectHandle(void) const;
    bool            isValid(void) const;
    bool            isDynamicInstance(void) const;

    auto            getMetaObject(ContextView vCtx) const -> const MetaObject&;
    auto            getParentMetaObject(ContextView vCtx) const -> const MetaObject*;
    auto            getMetaInstance(ContextView vCtx) const -> const MetaType&;
    Size            getExtendedSize(ContextView vCtx) const;
    void*           getExtendedData(ContextView vCtx) const;
    Dispatcher*     getOverrideDispatcher(ContextView vCtx) const;
    Dispatcher*     getStaticDispatcher(ContextView vCtx) const;
    Dispatcher*     getParentDispatcher(ContextView vCtx) const;

    bool            compare(ContextView vCtx, const Variant& rhs, int cmpType) const;
    bool            convert(ContextView vCtx, Variant& rhs) const;
    bool            serialize(ContextView vCtx, void* pBuffer, Size* pSize) const;
    bool            serialize(ContextView vCtx, String& string) const;

    bool            deserialize(ContextView vCtx, const void* pBuffer, Size* pSize);
    bool            deserialize(ContextView vCtx, String str);

    Variant         getProperty(ContextView vCtx, const Variant& key) const;
    bool            setProperty(ContextView vCtx, const Variant& key, const Variant& value);
    bool            setProperty(ContextView vCtx, ObjectPropertyPair pair);
    bool            getNextProperty(ContextView vCtx, const Variant& prevKey, Variant& nextKey, Variant& nextValue) const;
    bool            getNextProperty(ContextView vCtx, const Variant& prevKey, ObjectPropertyPair& nextPair) const;
    auto            getNextProperty(ContextView vCtx, const Variant& prevKey) const -> ObjectPropertyPair;

    template<typename F>
    decltype(auto) vCall(F ObjectVTable::*pMethod, auto&&... args) {
       if(pMethod) {
            return pMethod(std::forward<decltype(args)>(args)...);
        } else {
            // call parent
        }
    }

     template<typename F>
     decltype(auto) vCallParent(F ObjectVTable::*pMethod, auto&&... args) {
         Dispatcher* pDispatch = getParentDispatcher();
         if(pDispatch) {
             auto* pVTable = pDispatch->getVTable<const ObjectVTable*>();
             if(pVTable->*pMethod) return pVTable->*pMethod(std::forward<decltype(args)>(args)...);
         }
        return nullptr;
    }
};


class Object: public ObjectView<GblObject, Object> {
public:
    using ObjectType        = Object;
    using MetaType          = MetaType<Object>;
    using MetaTypeVTable    = MetaTypeVTable<Object>;

    template<typename T=Object>
    static T*       create(ContextView vCtx, const Variant& variant=Variant(), const MetaType<T>* pMeta=nullptr, const ExtraSpaceInfo* pExtraStorage=nullptr, const MetaTypeVTable<T>* pVTable=nullptr);
    static void     destroy(ContextView vCtx, ObjectView vObj);

    static constexpr const ObjectVTable vTable = {


    };


protected:
    //overridden from MetaType
    virtual void construct_v    (ContextView vCtx, Variant value) = 0;
    virtual void destruct_v     (ContextView vCtx) = 0;
    virtual bool compare_v      (ContextView vCtx, const Variant& rhs, Variant::OpCmpType cmpType) = 0;
    virtual bool convert_v      (ContextView vCtx, Variant& rhs) const {
        Dispatcher* pDispatch = getParentDispatcher();
        if(pDispatch) {
            pDispatch->vCall(&MetaTypeVTable::pFnConvert, *this, vCtx, &rhs);
        }
    }
    virtual bool serialize_v    (ContextView vCtx, void* pBuffer, Size* pSize) const = {
        return vCallParent(&MetaTypeVTable::pFnSerialize, *this vCtx, pBuffer, pSize);
    }
    virtual bool deserialize_v  (ContextView vCtx, const void* pBuffer, Size* pSize) = 0;

    //overridden from MetaObject
    virtual bool propertyGet_v  (ContextView vCtx, const Variant& key, Variant& value) const = 0;
    virtual bool propertySet_v  (ContextView vCtx, const Variant& key, Variant value)  = 0;
    virtual bool propertyNext_v (ContextView vCtx, const Variant& prevKey, Variant& nextKey, Variant& nextValue) const = 0;


private:

    template<>
    struct gimbal::MetaTypeVTable<Object>: public gimbal::MetaTypeVTable<void> {

        MetaTypeVTable(void) {
            this->pFnConstruct = &construct;
            this->pFnDestruct = &destruct;
            this->pFnConvert = &convert;
            this->pFnCompare = &compare;
            this->pFnSerialize = &serialize;
            this->pFnDeserialize = &deserialize;
        }

        static GBL_RESULT construct(void* pData, GblContext hCtx, const GblVariant* pVariant) {
            GBL_API_BEGIN(hCtx);
            Object* pObject = new (pData) Object();
            pObject->construct_v(hCtx, *pVariant);
            GBL_API_END();
        }

        static GBL_RESULT destruct(void* pData, GblContext hCtx) {
            GBL_API_BEGIN(hCtx);
            reinterpret_cst<Object*>(pData)->destruct_v(hCtx);
            reinterpret_cast<Object*>(pData)->~Object();
            GBL_API_END();
        }

        static GBL_RESULT convert(const void* pData, GblContext hCtx, GblVariant* pVariant) {
            GBL_API_BEGIN(hCtx);
            GBL_API_TRY {
                reinterpret_cast<const Object*>(pData)->convert_v(vCtx, *pVariant);
            } GBL_API_VERIFY_CATCH();
            GBL_API_END();
        }

        static GBL_RESULT compare(const void* pData, GblContext hCtx, const GblVariant* pVariant, int* pResult) {
            GBL_API_BEGIN(hCtx);
            reinterpret_cast<const Object*>(pData)->compare_v(vCtx, *pVariant, pResult);
            GBL_API_END();
        }

        static GBL_RESULT serialize(const void* pData, GblContext hCtx, void* pBuffer, GblSize* pSize) {
            GBL_API_BEGIN(hCtx);
            reinterpret_cast<const Object*>(pData)->serialize_v(vCtx, pBuffer, pSize);
            GBL_API_END();
        }

        static GBL_RESULT deserialize(void* pData, GblContext hCtx, const void* pBuffer, GblSize* pSize) {
            QString* pQStr = reinterpret_cast<QString*>(pData);
            GBL_API_BEGIN(hCtx);
            reinterpret_cast<const Object*>(pData)->deserialize_v(vCtx, pBuffer, pSize);
            GBL_API_END();
        }
    };


    Object(void);
    virtual ~Object(void);

};

class ContextObjectView: public ObjectView<GblContextObject, ContextObject> {
public:

    using ObjectView<GblContextObject, ContextObjec>::ObjectView;

    ContextView     getContext(void) const;
    auto            getMetaObject(void) const -> const MetaObject&;
    auto            getParentMetaObject(void) const -> const MetaObject*;
    auto            getMetaInstance(void) const -> const MetaInstance&;
    Size            getExtendedSize(void) const;
    void*           getExtendedData(void) const;
    Dispatcher*     getOverrideDispatcher(void) const;
    Dispatcher*     getStaticDispatcher(void) const;
    Dispatcher*     getParentDispatcher(void) const;

    bool            compare(const Variant& rhs, Variant::OpCmpType cmpType) const;
    bool            convert(Variant& rhs) const;
    Variant         toType(Variant::Type type) const;
    bool            serialize(void* pBuffer, Size* pSize) const;
    bool            serialize(String& string) const;

    bool            deserialize(const void* pBuffer, Size* pSize);
    bool            deserialize(String str);

    Variant         getProperty(const Variant& key) const;
    bool            setProperty(const Variant& key, Variant value); //move semantics?
    bool            setProperty(ObjectPropertyPair pair);
    bool            getNextProperty(const Variant& prevKey, Variant& nextKey, Variant& nextValue) const;
    bool            getNextProperty(const Variant& prevKey, ObjectPropertyPair& pair) const;
    auto            getNextProperty(const Variant& prevKey) const -> ObjectPropertyPair;

 //   friend constexpr auto operator<=>(const Variant& rhs) const;


    //Now we can add convenience shit!
    // 1) iterators
    // 3) [] property accessors + writeable proxies
    // 4) overloaded spaceship comparison operators
    // 5) overloaded stringification/serialization shit
};

class ContextObject: public ContextObjectView {

        static void     destroy(ContextObjectView vObj);
};



}

#endif


#endif // GIMBAL_OBJECT_HPP
