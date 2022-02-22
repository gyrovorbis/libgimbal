#ifndef GIMBAL_META_HPP
#define GIMBAL_META_HPP

#include "gimbal_meta.h"
#include "../types/gimbal_typedefs.hpp"
//#include "../types/gimbal_variant.hpp"

namespace gimbal {

template<typename T=void>
struct MetaTypeVTable: public GblMetaTypeVTable {
    MetaTypeVTable(void):
        GblMetaTypeVTable({}) {}
};

#if 0
#define CHECK_ADD_OVERRIDE(cFunc, cppFunc) \
    do {    \
        if constexpr(requires { this->pFn##cFunc = &Derived::cppFunc; }) {    \
            this->pFn##cFunc = &Derived::cppFunc;   \
        }   \
    } while(0)

template<typename Derived>
struct MetaTypeVTableStatic: public MetaTypeVTable, public Derived {
    using ThisType = void;
    using DerivedType = Derived;
    MetaTypeVTableVTableStatic(void) {
        CHECK_ADD_OVERRIDE(Construct, construct);
        CHECK_ADD_OVERRIDE(Destruct, destruct);
        CHECK_ADD_OVERRIDE(Convert, convert);
        CHECK_ADD_OVERRIDE(Compare, compare);
        CHECK_ADD_OVERRIDE(Serialize, serialize);
        CHECK_ADD_OVERRIDE(Deserialize, deserialize);
    }
    //static constexpr const MetaTypeVTable<T> defaultVTable = MetaTypeVTable<T>();
};
#endif



template<typename CRTP>
class DispatcherBase {
public:
    gimbal::Size getSize(void) const { return getDispatcher_().vTableSizeOf; }
    gimbal::Size getMethodCount(void) const { return getSize() / sizeof([](){}); }

    template<typename V,
             typename F,
             typename = std::enable_if_t<std::is_function_v<F>>>
    F vMethod(F V::*pMethod) const {
        const V* pVTable = getVTable<V>();
        return pVTable->*pMethod;
    }

    template<typename V,
             typename F,
             typename = std::enable_if_t<std::is_function_v<F>>>
    bool hasMethod(F V::*pMethod) const {
        return vMethod(pMethod) != nullptr;
    }

    template<typename V,
             typename F,
             typename = std::enable_if_t<std::is_function_v<F>>>
    decltype(auto) vCall(F V::*pMethod, auto&&... args) const {
        auto pFunc = vMethod(pMethod);
        GBL_ASSERT(pFunc);
        return pFunc(std::forward<decltype(args)...>);
    }

    template<typename V=const void*>
    const V* getVTable(void) const {
        if(sizeof(V) > getDispatcher_()->vTableSizeOf) return nullptr;
        return getDispatcher_()->pVTable();
    }

    operator const GblDispatcher*() const { return getDispatcher_();}

private:
    const GblDispatcher* getDispatcher_(void) const { return static_cast<const CRTP>(this)->getDispatcher(); }

};

class DispatcherView: public DispatcherBase<DispatcherView> {
public:
    DispatcherView(const GblDispatcher* pSource):
        pDispatcher_(pSource) {}

    const GblDispatcher* getDispatcher_(void) const { return pDispatcher_; }
private:
    const GblDispatcher* pDispatcher_ = nullptr;
};

class Dispatcher: public GblDispatcher,
                  public DispatcherBase<Dispatcher> {
public:


    operator GblDispatcher*() { return static_cast<GblDispatcher*>(this); }
    const GblDispatcher* getDispatcher_(void) const { return static_cast<const GblDispatcher*>(this); }
};




typedef struct GblMetaInstance {
    GblSize         sizeOf;
    GblSize         alignOf;
    GblDispatcher   dispatcher;
} GblMetaInstance;


template<typename CRTP>
class MetaInstanceBase {
public:
    gimbal::Size        getSizeOf(void) const { return getMetaInstance_()->sizeOf; }
    gimbal::Size        getAlignOf(void) const { return getMetaInstance_()->alignOf; }
    DispatcherView      getDispatcher(void) const { return &getMetaInstance_()->dispatcher; }

    operator const GblMetaInstance*() const {
        return getMetaInstance_();
    }

private:
    const GblMetaInstance* getMetaInstance_(void) const {
        return static_cast<CRTP>(this)->getMetaInstance_();
    }
};

class MetaInstanceView: public MetaInstanceBase<MetaInstanceView> {
public:
    MetaInstanceView(const GblMetaInstance* pSource):
        pInstance_(pSource) {}

    const GblMetaInstance* getMetaInstance_(void) const { return pInstance_; }
private:
    const GblMetaInstance* pInstance_ = nullptr;

};

class MetaInstance:
        public GblMetaInstance,
        public MetaInstanceBase<MetaInstance>
{
public:
    const GblMetaInstance* getMetaInstance_(void) const { return static_cast<const GblMetaInstance*>(this); }
    operator GblMetaInstance*() {
        return static_cast<GblMetaInstance*>(this);
    }

    void setSizeOf(gimbal::Size size) { this->sizeOf = size; }
    void setAlignOf(gimbal::Size align) { this->alignOf = align; }

};



template<typename T=void>
class MetaType: public GblMetaType {
public:
    //MetaType(void) = default;

    template<typename V=MetaTypeVTable<T>>
        requires std::is_base_of_v<GblMetaTypeVTable, V>
       MetaType(const V* pVTable, const char* pTypeName="Unknown"):
            GblMetaType({
                pTypeName,
                GBL_VARIANT_TYPE_NIL,
                GBL_META_TYPE_ID_INVALID,
                GBL_META_TYPE_FLAG_NONE,
                NULL,
                {
                    sizeof(T),
                    alignof(T),
                    {
                        sizeof(V),
                        pVTable
                    }
                }
        }) {}

    const char* getTypeName(void) const { return this->pTypeName; }
    MetaTypeId  getMetaTypeId(void) const { return this->typeId; }
    auto        getFlags(void) const { return this->flags; }
    Size        getSize(void) const { return this->metaInstance.sizeOf; }
    Size        getAlignment(void) const { return this->metaInstance.alignOf; }
    Size        getVTableSize(void) const { return this->metaInstance.dispatcher.vTableSizeOf; }
    //Size        getVTableEntries(void) const;
    //MetaObject* getMetaObject(void) const;

    T* create(Context* pCtx, const Variant& variant) const {
        T* pData = nullptr;
        Exception::checkThrow(gblMetaTypeCreate(this, *pCtx, &variant, reinterpret_cast<void**>(&pData)));
        return pData;
    }
    void construct(Context* pCtx, T* pData, const Variant& variant) const {
        Exception::checkThrow(gblMetaTypeConstruct(this, *pCtx, reinterpret_cast<void*>(pData), &variant));
    }
    void destroy(Context* pCtx, T* pData) const {
        Exception::checkThrow(gblMetaTypeDestroy(this, *pCtx, reinterpret_cast<void*>(pData)));
    }
    void destruct(Context* pCtx, T* pData) const {
        Exception::checkThrow(gblMetaTypeDestruct(this, *pCtx, reinterpret_cast<void*>(pData)));
    }
    void convert(Context* pCtx, const T* pData, Variant* pTo) const {
        Exception::checkThrow(gblMetaTypeConvert(this, *pCtx, reinterpret_cast<const void*>(pData), pTo));
    }
    gimbal::Int compare(Context* pCtx, const T* pData, const Variant& rhs) const {
        GblInt result = 1;
        Exception::checkThrow(gblMetaTypeCompare(this, *pCtx, reinterpret_cast<const void*>(pData), &rhs, &result));
        return result;
    }
    bool compare(Context* pCtx, const T* pData, const Variant& rhs, Variant::OpCmpType cmpType) const {
        gimbal::Int result = compare(pCtx, pData, rhs);
        return GBL_COMPARISON_VERIFY_OP_CMP_TYPE(result, cmpType);
    }
    bool serialize(Context* pCtx, const T* pData, void* pBuffer, Size* pSize) const {
        Exception::checkThrow(gblMetaTypeSerialize(this, *pCtx, reinterpret_cast<const void*>(pData), pBuffer, pSize));
        return true;
    }
    bool deserialize(Context* pCtx, T* pData, const void* pBuffer, Size* pSize) const {
        Exception::checkThrow(gblMetaTypeDeserialize(this, *pCtx, reinterpret_cast<void*>(pData), pBuffer, pSize));
        return true;
    }
};


}


#endif // GIMBAL_META_HPP
