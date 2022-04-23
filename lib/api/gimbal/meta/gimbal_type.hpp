#ifndef GIMBAL_TYPE_HPP
#define GIMBAL_TYPE_HPP

#include "gimbal_type.h"
#include "../types/gimbal_typedefs.hpp"
#include "../core/gimbal_api_generators.hpp"
#include "../types/gimbal_result.hpp"
#include "../objects/gimbal_context.hpp"

#define GBL_TYPE_DECLARE_CPP(cppName, cName)    \
    class cppName final: public Type {          \
    public:                                     \
        cppName(void): Type(cName) {}           \
    }

namespace gimbal {

class Class;

class TypeFlags: public PrimitiveBase<GblTypeFlags> {
public:
    TypeFlags(unsigned value):
        TypeFlags(static_cast<GblTypeFlags>(value)) {}

    TypeFlags(GblTypeFlags flags=static_cast<GblTypeFlags>(0)):
        PrimitiveBase(flags) {}

};

class TypeInfo: public GblTypeInfo {
public:

};

class Type: public PrimitiveBase<GblType> {
public:

       Type(GblType type=GBL_TYPE_INVALID);
       Type(float) = delete;
       Type(double) = delete;
       Type(unsigned) = delete;

        const char*     getName(void) const;
        Type            getParentType(void) const;
        Type            getFundamentalType(void) const;
        const TypeInfo* getInfo(void) const;
        UInt            getDepth(void) const;

        bool            isValid(void) const;
        bool            isClassed(void) const;
        bool            isInstantiable(void) const;
        bool            isDerivable(void) const;
        bool            isDeepDerivable(void) const;
        bool            isAbstract(void) const;
        bool            isFinal(void) const;
        bool            isInterface(void) const;
        bool            isFundamental(void) const;

        bool            isA(Type base) const;
        bool            isFundamentallyA(Type base) const;
        RefCount        getClassRefCount(void) const;
#ifdef GBL_TYPE_DEBUG
        RefCount        getInstanceRefCount(void) const;
#endif

        void*           instanceCreate      (void) const;
        void            instanceConstruct   (void* pInstance) const;
        void            instanceDestruct    (void* pInstance) const;
        void            instanceDestroy     (void* pInstance) const;

        void*           classReference      (void) const;
        void            classUnreference    (void* pClass) const;

// static API
        static Result   reinit(Context* pCtx=nullptr);
        static Type     registerStatic(Type         parentType,
                                       const char*  pName,
                                       TypeInfo     typeInfo,
                                       TypeFlags    flags);
        static void     unregister(Type type);

        static Size     count(void);
        static Type     fromName(const char* pName);

        static void*    instanceCreate      (Type type);
        static void     instanceConstruct   (Type type, void* pInstance);
        static void     instanceDestruct    (Type type, void* pInstance);
        static void     instanceDestroy     (Type type, void* pInstance);

        static void*    classReference      (Type type);
        static void*    classPeek           (Type type);
        static void     classUnreference    (Type type, void* pClass);
};



template<typename P,
         typename C,
         typename I>
class StaticType: public Type {
public:
    using ParentType    = P;
    using ClassType     = C;
    using InstanceType  = I;

protected:
    StaticType(Type type): Type(std::move(type)) {}
#if 0
    ClassRef<ClassType> getClassRef(void) const {
        return Type::classRef();
    }
#endif

    InstanceType*   createInstance(void) const;
    void            destroyInstance(InstanceType* pInstance) const;

    Type registerStatic(const char* pName, TypeFlags flags) {
        const TypeInfo info = {
            {
                .classSize = sizeof(C),
                .classAlign = alignof(C),
                .instanceSize = sizeof(I),
                .instanceAlign = alignof(I)
            }
        };
        return Type::registerStatic(P(), pName, info, flags);
    }

};

GBL_TYPE_DECLARE_CPP(InvalidType,   (GblType)GBL_TYPE_INVALID);
GBL_TYPE_DECLARE_CPP(InterfaceType, GBL_TYPE_INTERFACE);
GBL_TYPE_DECLARE_CPP(IVariantType,  GBL_TYPE_IVARIANT);
GBL_TYPE_DECLARE_CPP(NilType,       GBL_TYPE_NIL);
GBL_TYPE_DECLARE_CPP(BoolType,      GBL_TYPE_BOOL);
GBL_TYPE_DECLARE_CPP(CharType,      GBL_TYPE_CHAR);
GBL_TYPE_DECLARE_CPP(Uint8Type,     GBL_TYPE_UINT8);
GBL_TYPE_DECLARE_CPP(Uint16Type,    GBL_TYPE_UINT16);
GBL_TYPE_DECLARE_CPP(Int16Type,     GBL_TYPE_INT16);
GBL_TYPE_DECLARE_CPP(Uint32Type,    GBL_TYPE_UINT32);
GBL_TYPE_DECLARE_CPP(Int32Type,     GBL_TYPE_INT32);
GBL_TYPE_DECLARE_CPP(Uint64Type,    GBL_TYPE_UINT64);
GBL_TYPE_DECLARE_CPP(Int64Type,     GBL_TYPE_INT64);
GBL_TYPE_DECLARE_CPP(EnumType,      GBL_TYPE_ENUM);
GBL_TYPE_DECLARE_CPP(FlagsType,     GBL_TYPE_FLAGS);
GBL_TYPE_DECLARE_CPP(FloatType,     GBL_TYPE_FLOAT);
GBL_TYPE_DECLARE_CPP(DoubleType,    GBL_TYPE_DOUBLE);
GBL_TYPE_DECLARE_CPP(PointerType,   GBL_TYPE_POINTER);
GBL_TYPE_DECLARE_CPP(StringType,    GBL_TYPE_STRING);


// ==== INLINEZ =====
inline Type::Type(GblType type): PrimitiveBase(type) {}
inline const char* Type::getName(void) const { return gblTypeName(*this); }
inline Type Type::getParentType(void) const { return gblTypeParent(*this); }
inline UInt Type::getDepth(void) const { return gblTypeDepth(*this); }
inline Type Type::getFundamentalType(void) const { return gblTypeFundamental(*this); }
inline const TypeInfo* Type::getInfo(void) const { return static_cast<const TypeInfo*>(gblTypeInfo(*this)); }
inline bool Type::isA(Type base) const { return gblTypeIsA(*this, base); }
inline RefCount Type::getClassRefCount(void) const { return gblTypeClassRefCount(*this); }
inline bool Type::isValid(void) const { return *this != GBL_TYPE_INVALID; }

inline bool Type::isClassed(void) const { return GBL_TYPE_IS_CLASSED(*this); }
inline bool Type::isInstantiable(void) const { return GBL_TYPE_IS_INSTANTIABLE(*this); }
inline bool Type::isDerivable(void) const { return GBL_TYPE_IS_DERIVABLE(*this); }
inline bool Type::isDeepDerivable(void) const { return GBL_TYPE_IS_DEEP_DERIVABLE(*this); }
inline bool Type::isAbstract(void) const { return GBL_TYPE_IS_ABSTRACT(*this); }
inline bool Type::isFinal(void) const { return GBL_TYPE_IS_FINAL(*this); }
inline bool Type::isInterface(void) const  { return GBL_TYPE_IS_INTERFACE(*this); }
inline bool Type::isFundamental(void) const { return GBL_TYPE_IS_FUNDAMENTAL(*this); }

inline void* Type::instanceCreate      (void) const { return Type::instanceCreate(*this); }
inline void  Type::instanceConstruct   (void* pInstance) const { return Type::instanceConstruct(*this, pInstance); }
inline void  Type::instanceDestruct    (void* pInstance) const { Type::instanceDestruct(*this, pInstance); }
inline void  Type::instanceDestroy     (void* pInstance) const { Type::instanceDestroy(*this, pInstance); }

inline void* Type::classReference      (void) const { return Type::classReference(*this); }
inline void  Type::classUnreference    (void* pClass) const { return Type::classUnreference(*this, pClass); }

inline Result Type::reinit(Context* pCtx) {
    return gblTypeReinit(pCtx);
}

inline Type Type::registerStatic(Type parentType,
                                 const char* pName,
                                 TypeInfo typeInfo,
                                 TypeFlags flags)
{
    return gblTypeRegisterStatic(parentType,
                           pName,
                           &typeInfo,
                           flags);
}

inline void Type::unregister(Type type) {
    Exception::checkThrow(gblTypeUnregister(type));
}

inline Type Type::fromName(const char* pName) {
    return gblTypeFind(pName);
}
inline Size Type::count(void) {
    return gblTypeCount();
}

inline void* Type::instanceCreate      (Type type) { return gblTypeInstanceCreate(type); }
inline void  Type::instanceConstruct   (Type type, void* pInstance) { gblTypeInstanceConstruct(type, reinterpret_cast<GblInstance*>(pInstance)); }
inline void  Type::instanceDestruct    (Type type, void* pInstance) { gblTypeInstanceDestruct(reinterpret_cast<GblInstance*>(pInstance)); }
inline void  Type::instanceDestroy     (Type type, void* pInstance) { gblTypeInstanceDestroy(reinterpret_cast<GblInstance*>(pInstance)); }

inline void* Type::classReference      (Type type) { return gblTypeClassRef(type); }
inline void  Type::classUnreference    (Type type, void* pClass) { gblTypeClassUnref(reinterpret_cast<GblClass*>(pClass)); }

}

#endif // GIMBAL_TYPE_HPP
