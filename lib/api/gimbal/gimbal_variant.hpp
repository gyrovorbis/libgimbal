#ifndef GIMBAL_VARIANT_HPP
#define GIMBAL_VARIANT_HPP


#include "gimbal_variant.h"
#include "gimbal_types.hpp"

namespace gimbal {



GBL_API gblVariantType(const GblVariant* pVariant, GBL_VARIANT_TYPE* pType);
GBL_API gblVariantTypeString(GBL_VARIANT_TYPE type, const char* pBuff, GblSize* pSize);
GBL_API gblVariantTypeClass(GBL_VARIANT_TYPE type, GBL_VARIANT_TYPE_CLASS* pClass);
GBL_API gblVariantMetaType(GBL_VARIANT_TYPE type, const char* pTypeName, GblMetaType** ppMetaType);
GBL_API gblVariantCompatibleTypes(GBL_VARIANT_TYPE type1, GBL_VARIANT_TYPE type2, GblBool* pResult);
GBL_API gblVariantCommonType(const GblVariant* pLhs,
                             const GblVariant* pRhs,
                             GBL_VARIANT_TYPE* pType);
GBL_API gblVariantIsNil(const GblVariant* pVariant,   GblBool*    pValue);
GBL_API gblVariantCopy(GblVariant* pVariant, const GblVariant* pRhs);

// Get (Actual) Value
GBL_API gblVariantGetb(const GblVariant* pVariant,    GblBool*     pValue);
GBL_API gblVariantGets(const GblVariant* pVariant,    const char** ppValue);
GBL_API gblVariantGetv(const GblVariant* pVariant,    void**       pValue);
GBL_API gblVariantGett(const GblVariant* pVariant,    GblTable**   ppTable);
GBL_API gblVariantGeti(const GblVariant* pVariant,    GblInt*      pValue);
GBL_API gblVariantGetf(const GblVariant* pVariant,    GblReal*    pValue);

// Set Value
GBL_API gblVariantSetNil(GblVariant* pVariant);
GBL_API gblVariantSetb(GblVariant* pVariant,    GblBool     value);
GBL_API gblVariantSets(GblVariant* pVariant,    const char* pValue);
GBL_API gblVariantSetv(GblVariant* pVariant,    void*       pValue);
GBL_API gblVariantSett(GblVariant* pVariant,    const GblTable* pTable);
GBL_API gblVariantSeti(GblVariant* pVariant,    GblInt      value);
GBL_API gblVariantSetf(GblVariant* pVariant,    GblReal       value);

// (MAYBE) Convert Value
GBL_API gblVariantNil(const GblVariant* pVariant,  GblBool*    pValue);
GBL_API gblVariantb(const GblVariant* pVariant,    GblBool*    pValue);
GBL_API gblVariants(const GblVariant* pVariant,    const char* pBuffer, GblSize* pSize);
GBL_API gblVariantv(const GblVariant* pVariant,    void**       pValue);
GBL_API gblVarianti(const GblVariant* pVariant,    GblInt*     pValue);
GBL_API gblVariantf(const GblVariant* pVariant,    GblReal*    pValue);

GBL_API gblVariantCompare(const GblVariant* pLhs,
                          const GblVariant* pRhs,
                          GBL_VARIANT_OP compOp,
                          GblBool* pResult); //does strcmp too?

//LuaVariant compat
//std::variant compat
//QVariant compat
//std::any compat
//std::string vs std::string_view
//exceptions for reest (maybe configurable?)
//asserts for reest? (maybe configurable)
//std::floating_point, std::integral concepts?
    class Variant: public GblVariant {
    public:

        static_assert(sizeof(Variant) == sizeof(GblVariant), "C and C++ types are not the same size!");

        enum class Type: Enum {
            Nil             = GBL_VARIANT_TYPE_NIL,
            Bool            = GBL_VARIANT_TYPE_BOOL,
            Int             = GBL_VARIANT_TYPE_INT,
            Float           = GBL_VARIANT_TYPE_FLOAT,
            String          = GBL_VARIANT_TYPE_STRING,
            Function        = GBL_VARIANT_TYPE_FUNCTION,
            VoidPtr         = GBL_VARIANT_TYPE_VOID_PTR,
            Table           = GBL_VARIANT_TYPE_TABLE,
            Userdata        = GBL_VARIANT_TYPE_USERDATA,
            BuiltinCount    = Handle + 1,
            UserType = BuiltinCount
        };

        constexpr Variant(void);


        template<typename T>
        constexpr T     getValue(void) const;

        template<typename T>
        constexpr void setValue(T&& value) const;

        //SPACE SHIP FUCKING OPERATOR

        template<typename T>
        constexpr bool operator==(T&& value) const;

        template<typename T>
        constexpr bool operator!=(T&& value) const;

        Variant operator const();

        constexpr Type        getType(void) const;
        const char*           getTypeName(void) const;
        constexpr bool        isNil(void) const;
        constexpr bool        isRefType(void) const;
        constexpr bool        isIndexible(void) const;
    };

}

#endif // GIMBAL_VARIANT_HPP
