#ifndef GIMBAL_TYPE_HPP
#define GIMBAL_TYPE_HPP

#include "gimbal_type.h"
#include "../types/gimbal_typedefs.hpp"
#include "../core/gimbal_api_generators.hpp"
#include "../types/gimbal_result.hpp"
#include "../objects/gimbal_context.hpp"
#include "../meta/gimbal_primitives.h"
#include "../meta/gimbal_enum.h"
#include "../meta/gimbal_flags.h"

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

       Type(GblType type=GBL_INVALID_TYPE);
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
        bool            isInterfaced(void) const;
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
        static Result   init(Context* pCtx                  =nullptr,
                             Size typeBuiltinInitialCount   =0,
                             Size typeTotalInitialCount     =0);
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
                .instanceSize = sizeof(I)
            }
        };
        return Type::registerStatic(P(), pName, info, flags);
    }

};

GBL_TYPE_DECLARE_CPP(InvalidType,   GBL_INVALID_TYPE);
GBL_TYPE_DECLARE_CPP(InterfaceType, GBL_INTERFACE_TYPE);
GBL_TYPE_DECLARE_CPP(IVariantType,  GBL_IVARIANT_TYPE);
GBL_TYPE_DECLARE_CPP(NilType,       GBL_NIL_TYPE);
GBL_TYPE_DECLARE_CPP(BoolType,      GBL_BOOL_TYPE);
GBL_TYPE_DECLARE_CPP(CharType,      GBL_CHAR_TYPE);
GBL_TYPE_DECLARE_CPP(Uint8Type,     GBL_UINT8_TYPE);
GBL_TYPE_DECLARE_CPP(Uint16Type,    GBL_UINT16_TYPE);
GBL_TYPE_DECLARE_CPP(Int16Type,     GBL_INT16_TYPE);
GBL_TYPE_DECLARE_CPP(Uint32Type,    GBL_UINT32_TYPE);
GBL_TYPE_DECLARE_CPP(Int32Type,     GBL_INT32_TYPE);
GBL_TYPE_DECLARE_CPP(Uint64Type,    GBL_UINT64_TYPE);
GBL_TYPE_DECLARE_CPP(Int64Type,     GBL_INT64_TYPE);
GBL_TYPE_DECLARE_CPP(EnumType,      GBL_ENUM_TYPE);
GBL_TYPE_DECLARE_CPP(FlagsType,     GBL_FLAGS_TYPE);
GBL_TYPE_DECLARE_CPP(FloatType,     GBL_FLOAT_TYPE);
GBL_TYPE_DECLARE_CPP(DoubleType,    GBL_DOUBLE_TYPE);
GBL_TYPE_DECLARE_CPP(PointerType,   GBL_POINTER_TYPE);
GBL_TYPE_DECLARE_CPP(StringType,    GBL_STRING_TYPE);


// ==== INLINEZ =====
inline Type::Type(GblType type): PrimitiveBase(type) {}
inline const char* Type::getName(void) const { return GblType_name(*this); }
inline Type Type::getParentType(void) const { return GblType_parent(*this); }
inline UInt Type::getDepth(void) const { return GblType_depth(*this); }
inline Type Type::getFundamentalType(void) const { return GblType_root(*this); }
inline const TypeInfo* Type::getInfo(void) const { return static_cast<const TypeInfo*>(GblType_info(*this)); }
inline bool Type::isA(Type base) const { return GblType_check(*this, base); }
inline RefCount Type::getClassRefCount(void) const { return GblType_classRefCount(*this); }
inline bool Type::isValid(void) const { return *this != GBL_INVALID_TYPE; }

inline bool Type::isClassed(void) const { return GBL_TYPE_CLASSED_CHECK(*this); }
inline bool Type::isInstantiable(void) const { return GBL_TYPE_INSTANTIABLE_CHECK(*this); }
inline bool Type::isDerivable(void) const { return GBL_TYPE_DERIVABLE_CHECK(*this); }
inline bool Type::isDeepDerivable(void) const { return GBL_TYPE_DEEP_DERIVABLE_CHECK(*this); }
inline bool Type::isAbstract(void) const { return GBL_TYPE_ABSTRACT_CHECK(*this); }
inline bool Type::isFinal(void) const { return GBL_TYPE_FINAL_CHECK(*this); }
inline bool Type::isInterfaced(void) const  { return GBL_TYPE_INTERFACED_CHECK(*this); }
inline bool Type::isRoot(void) const { return GBL_TYPE_ROOT_CHECK(*this); }

inline void* Type::instanceCreate      (void) const { return Type::instanceCreate(*this); }
inline void  Type::instanceConstruct   (void* pInstance) const { return Type::instanceConstruct(*this, pInstance); }
inline void  Type::instanceDestruct    (void* pInstance) const { Type::instanceDestruct(*this, pInstance); }
inline void  Type::instanceDestroy     (void* pInstance) const { Type::instanceDestroy(*this, pInstance); }

inline void* Type::classReference      (void) const { return Type::classReference(*this); }
inline void  Type::classUnreference    (void* pClass) const { return Type::classUnreference(*this, pClass); }

inline Result Type::init(Context* pCtx, Size initialBuiltinCount, Size initialTotalCount) {
    return GblType_init(pCtx, initialBuiltinCount, initialTotalCount);
}

inline Type Type::registerStatic(Type parentType,
                                 const char* pName,
                                 TypeInfo typeInfo,
                                 TypeFlags flags)
{
    return GblType_registerStatic(parentType,
                           pName,
                           &typeInfo,
                           flags);
}

inline void Type::unregister(Type type) {
    Exception::checkThrow(GblType_unregister(type));
}

inline Type Type::fromName(const char* pName) {
    return GblType_fromName(pName);
}
inline Size Type::count(void) {
    return GblType_registeredCount();
}

inline void* Type::instanceCreate      (Type type) { return GblInstance_create(type); }
inline void  Type::instanceConstruct   (Type type, void* pInstance) { GblInstance_construct(reinterpret_cast<GblInstance*>(pInstance), type); }
inline void  Type::instanceDestruct    (Type type, void* pInstance) { GblInstance_destruct(reinterpret_cast<GblInstance*>(pInstance)); }
inline void  Type::instanceDestroy     (Type type, void* pInstance) { GblInstance_destroy(reinterpret_cast<GblInstance*>(pInstance)); }

inline void* Type::classReference      (Type type) { return GblClass_refDefault(type); }
inline void  Type::classUnreference    (Type type, void* pClass) { GblClass_unrefDefault(reinterpret_cast<GblClass*>(pClass)); }

}

#endif // GIMBAL_TYPE_HPP
