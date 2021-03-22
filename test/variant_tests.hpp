#ifndef VARIANT_TESTS_HPP
#define VARIANT_TESTS_HPP





// Top-level Utilities
GBL_API gblVariantType(const gblVariant* pVariant, GIM_VARIANT_TYPE* pType);
GBL_API gblVariantTypeString(GIM_VARIANT_TYPE type, const char* pBuff, GimSize* pSize);
GBL_API gblVariantTypeClass(GIM_VARIANT_TYPE type, GIM_VARIANT_TYPE_CLASS* pClass);
GBL_API gblVariantCompatibleTypes(GIM_VARIANT_TYPE type1, GIM_VARIANT_TYPE type2, GimBool* pResult);
GBL_API gblVariantCommonType(const gblVariant* pLhs,
                             const gblVariant* pRhs,
                             GIM_VARIANT_TYPE* pType);
GBL_API gblVariantIsNil(const gblVariant* pVariant,   GimBool*    pValue);
GBL_API gblVariantCopy(gblVariant* pVariant, const gblVariant* pRhs);

// Get (Actual) Value
GBL_API gblVariantGetb(const gblVariant* pVariant,    GimBool*     pValue);
GBL_API gblVariantGets(const gblVariant* pVariant,    const char** ppValue);
GBL_API gblVariantGetv(const gblVariant* pVariant,    void**       pValue);
GBL_API gblVariantGett(const gblVariant* pVariant,    GimTable**   ppTable);
GBL_API gblVariantGeti(const gblVariant* pVariant,    GimInt*      pValue)
GBL_API gblVariantGetf(const gblVariant* pVariant,    GimFloat*    pValue);

// Set Value
GBL_API gblVariantSetNil(gblVariant* pVariant);
GBL_API gblVariantSetb(gblVariant* pVariant,    GimBool     value);
GBL_API gblVariantSets(gblVariant* pVariant,    const char* pValue);
GBL_API gblVariantSetv(gblVariant* pVariant,    void*       pValue);
GBL_API gblVariantSett(gblVariant* pVariant,    const GimTable* pTable);
GBL_API gblVariantSeti(gblVariant* pVariant,    GimInt      value)
GBL_API gblVariantSetf(gblVariant* pVariant,    GimFloat       value);

// (MAYBE) Convert Value
GBL_API gblVariantb(const gblVariant* pVariant,    GimBool*    pValue);
GBL_API gblVariants(const gblVariant* pVariant,    const char* pBuffer, GimSize* pSize);
GBL_API gblVariantv(const gblVariant* pVariant,    void**       pValue);
GBL_API gblVarianti(const gblVariant* pVariant,    GimInt*     pValue);
GBL_API gblVariantf(const gblVariant* pVariant,    GimFloat*    pValue);


namespace gimbal {
//extend to LuaVariant shit.... somehow... and QVariant...
    class Variant: public gblVariant {
    public:
        Variant(void) = default;
        Variant(GimBool value);
        Variant(GimInt  value);
        Variant(GimFloat value);
        Variant(const char* pValue);
        Variant(void* pValue);
        Variant(GimTable* pValue);
        Variant(const gblVariant& other);
        Variant(gblVariant&& other);


        GIM_VARIANT_TYPE getType(void) const;

        bool isNil(void) const;






    };


}


class VariantTestSet: public UnitTestSet {
public slots:
    void setNil();
    void setBool();
    void test3();

};

inline void DemoTestSet::test1() {
    QVERIFY(true == true);
}

inline void DemoTestSet::test2() {
    QCOMPARE(QString("hello").toUpper(),
             QString("HELLO"));
}

inline void DemoTestSet::test3() {
    QEXPECT_FAIL("", "Example of making the next shit fail acceptably", Continue);
    QCOMPARE(1, -1);
}



#endif // VARIANT_TESTS_HPP
