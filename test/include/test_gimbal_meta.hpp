#ifndef TEST_GIMBAL_META_HPP
#define TEST_GIMBAL_META_HPP

#include <gimbal/meta/gimbal_meta.hpp>
#include "test_gimbal.hpp"

template<>
struct gimbal::MetaTypeVTable<QString>: public gimbal::MetaTypeVTable<void> {

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
        QString* pQStr = new (pData) QString();
        const gimbal::Variant* pCppVariant = static_cast<const gimbal::Variant*>(pVariant);
        if(pCppVariant) {
            gimbal::String str = pCppVariant->toValue<GblString>();
            *pQStr = str.getCString();
        }
        GBL_API_END();
    }

    static GBL_RESULT destruct(void* pData, GblContext hCtx) {
        GBL_API_BEGIN(hCtx);
        reinterpret_cast<QString*>(pData)->~QString();
        GBL_API_END();
    }

    static GBL_RESULT convert(const void* pData, GblContext hCtx, GblVariant* pVariant) {
        GBL_API_BEGIN(hCtx);
        gimbal::Variant* pCppVariant = static_cast<gimbal::Variant*>(pVariant);
        const QString* pQStr = reinterpret_cast<const QString*>(pData);
        switch(pVariant->type) {
        case GBL_VARIANT_TYPE_NIL:
            pCppVariant->setValue((GblBool)pQStr->isEmpty()); break;
        case GBL_VARIANT_TYPE_BOOL:
            pCppVariant->setValue((GblBool)!pQStr->isEmpty()); break;;
        case GBL_VARIANT_TYPE_STRING: {
            gimbal::String str(pQStr->toStdString().c_str());
            pCppVariant->setValue(static_cast<const GblString*>(&str)); break;
        }
        case GBL_VARIANT_TYPE_INT:
            pCppVariant->setValue(pQStr->toInt()); break;
        case GBL_VARIANT_TYPE_FLOAT:
            pCppVariant->setValue(pQStr->toFloat()); break;
        case GBL_VARIANT_TYPE_VOID_PTR:
        //    pCppVariant->setValue(reinterpret_cast<void*>(pQStr)); break;
        default:
            GBL_API_RECORD_SET(GBL_RESULT_ERROR_INVALID_OPERATION);
        }
        GBL_API_END();
    }

    static GBL_RESULT compare(const void* pData, GblContext hCtx, const GblVariant* pVariant, int* pResult) {
        GBL_API_BEGIN(hCtx);
        const gimbal::Variant* pCppVariant = static_cast<const gimbal::Variant*>(pVariant);
        const QString* pQStr = reinterpret_cast<const QString*>(pData);
        gimbal::String str = pCppVariant->toValue<GblString>();
        *pResult = strcmp(pQStr->toStdString().c_str(), str.getCString());
        GBL_API_END();
    }

    static GBL_RESULT serialize(const void* pData, GblContext hCtx, void* pBuffer, GblSize* pSize) {
        const QString* pQStr = reinterpret_cast<const QString*>(pData);
        GBL_API_BEGIN(hCtx);
        if(pBuffer) {
            GBL_API_VERIFY_ARG(*pSize >= pQStr->length());
            strncpy(reinterpret_cast<char*>(pBuffer), pQStr->toStdString().c_str(), *pSize);
        }
        *pSize = pQStr->length()+1;
        GBL_API_END();
    }

    static GBL_RESULT deserialize(void* pData, GblContext hCtx, const void* pBuffer, GblSize* pSize) {
        QString* pQStr = reinterpret_cast<QString*>(pData);
        GBL_API_BEGIN(hCtx);
        GBL_API_VERIFY_ARG(*pSize >= pQStr->length());
        *pQStr = reinterpret_cast<const char*>(pBuffer);
        *pSize = pQStr->length()+1;
        GBL_API_END();
    }
};

namespace gimbal::test {


template<typename T,
         typename Meta=gimbal::MetaType<T>,
         typename VTable=gimbal::MetaTypeVTable<T>>
class MetaTypeDerived: public UnitTestSet {
public:
    using Type                  = T;
    using MetaType              = Meta;
    using MetaTypeVTableType    = VTable;

protected:
    Meta metaDefault_ = {};
    VTable vtableDefault_;


    void delete_(T* pValue) {
        GBL_API_BEGIN(hCtx());
        if(pValue) {
            pValue->~T();
            GBL_API_FREE(pValue);
        }
        GBL_API_END_BLOCK();
    }

    T* create_(const gimbal::Variant& constructValue) {
        T* pData = nullptr;
        create(constructValue, NULL, gimbal::Result::Success, &pData);
        if(!pData) {
            throw "Create didn't return a valid pointer!";
        }
        return pData;
    }

    void conditionalDestroy_(T* pData, T** ppOut) {
        if(ppOut) *ppOut = pData;
        else getMetaType().destroy(pCtx(), pData);
    }

public:

    MetaTypeDerived(gimbal::test::MonitorableContext* pCtx=nullptr):
        UnitTestSet(pCtx? pCtx : new gimbal::test::TestContext()),
        metaDefault_(&vtableDefault_) {}

    virtual const VTable& getVTable(void) const { return vtableDefault_; }

    virtual const Meta& getMetaType(void) const {return metaDefault_; }


    void create(const gimbal::Variant& constructValue, const auto* pExpected=nullptr, gimbal::Result expectedResult=gimbal::Result::Success, T** ppOut=nullptr) {
        T* pData = nullptr;

        auto testBlk = GBL_TEST_CASE_API_BLOCK(pCtx(), "create") {
            auto blk = GBL_API_TRY_BLOCK {
                pData = getMetaType().create(pCtx(), constructValue);
            };
            QCOMPARE(blk.getRecord().getResult(), expectedResult);
        };
        if(expectedResult == gimbal::Result::Success || pData) {
            QCOMPARE(testBlk.getCountersDelta().getExt(ContextCounters::ApiExtCall::MemAlloc), 1);
            QVERIFY(pData);
            if(pExpected) QCOMPARE(*pData, *pExpected);
            if(ppOut) *ppOut = pData;
            else delete_(pData);
        }
    }

    void create(const gimbal::Variant& constructValue, std::nullptr_t, gimbal::Result expectedResult=gimbal::Result::Success, T** ppOut=nullptr) {
        create(constructValue, reinterpret_cast<const T*>(NULL), expectedResult, ppOut); // so fucking janky
    }

    void create(const gimbal::Variant& constructValue, T expected, gimbal::Result expectedResult=gimbal::Result::Success, T** ppOut=nullptr) {
        create(constructValue, &expected, expectedResult, ppOut);
    }

    void destroy(const gimbal::Variant& constructValue, gimbal::Result expectedResult=gimbal::Result::Success) {
        T* pData = create_(constructValue);
        auto testBlk = GBL_TEST_CASE_API_BLOCK(pCtx(), "destroy") {
            auto blk = GBL_API_TRY_BLOCK {
                getMetaType().destroy(pCtx(), pData);
            };
            QCOMPARE(blk.getRecord().getResult(), expectedResult);
        };
        if(expectedResult != gimbal::Result::Success) delete_(pData);
        else QCOMPARE(testBlk.getCountersDelta().getExt(ContextCounters::ApiExtCall::MemFree), 1);
    }

    void convert(const gimbal::Variant& constructValue, gimbal::Variant& toValue, const gimbal::Variant* pExpected=nullptr, gimbal::Result expectedResult=gimbal::Result::Success, T** ppOut=nullptr) {
        T* pData = create_(constructValue);
        auto blk = GBL_API_TRY_BLOCK {
            getMetaType().convert(pCtx(), pData, &toValue);
        };
        QCOMPARE(blk.getRecord().getResult(), expectedResult);
        if(pExpected) {
            QCOMPARE(toValue.getType(), pExpected->getType());
            QCOMPARE(toValue, *pExpected);
        }
        conditionalDestroy_(pData, ppOut);
    }

    void convert(const gimbal::Variant& constructValue, const gimbal::Variant& toValue, const gimbal::Variant* pExpected=nullptr, gimbal::Result expectedResult=gimbal::Result::Success, T** ppOut=nullptr) {
        gimbal::Variant tempValue = toValue;
        convert(constructValue, tempValue, pExpected, expectedResult, ppOut);
    }

    void compare(const gimbal::Variant& constructValue, const gimbal::Variant& rhs, gimbal::Variant::OpCmpType expectedCmp=gimbal::Variant::OpCmpType::Equal, gimbal::Result expectedResult=gimbal::Result::Success, T** ppOut=nullptr) {
        T* pData = create_(constructValue);
        int cmpResult = -1;
        auto blk = GBL_API_TRY_BLOCK {
            cmpResult = getMetaType().compare(pCtx(), pData, rhs);
        };
        QCOMPARE(blk.getRecord().getResult(), expectedResult);
        QVERIFY(GBL_COMPARISON_VERIFY_OP_CMP_TYPE(cmpResult, expectedCmp));
        conditionalDestroy_(pData, ppOut);
    }

    void serialize(const gimbal::Variant& constructValue, const char* pExpectedString=nullptr, gimbal::Result expectedResult=gimbal::Result::Success, T**ppOut=nullptr, char** ppStringBuffOut=nullptr) {
        GBL_API_BEGIN(hCtx());
        T* pData = create_(constructValue);
        char* pStringBuff = nullptr;
        gimbal::Size bufferSize = 0;

        // First get required size
        auto blk = GBL_API_TRY_BLOCK {
            getMetaType().serialize(pCtx(), pData, nullptr, &bufferSize);
        };
        QCOMPARE(blk.getRecord().getResult(), expectedResult);
        // Next allocate buffer + recall function
        if(bufferSize > 0) {
            pStringBuff = static_cast<char*>(GBL_API_MALLOC(GBL_ALIGNED_ALLOC_SIZE(bufferSize)));
            gimbal::Size writeSize = bufferSize;
            auto blk = GBL_API_TRY_BLOCK {
                getMetaType().serialize(pCtx(), pData, reinterpret_cast<void*>(pStringBuff), &writeSize);
            };
            QCOMPARE(blk.getRecord().getResult(), expectedResult);
            QCOMPARE(bufferSize, writeSize);
        }
        if(pExpectedString) {
            QVERIFY(pStringBuff);
            QCOMPARE(pStringBuff, pExpectedString);
        }
        conditionalDestroy_(pData, ppOut);
        GBL_API_END_BLOCK();
        if(ppStringBuffOut) *ppStringBuffOut = pStringBuff;
        else GBL_API_FREE(pStringBuff);
    }

    void deserialize(const gimbal::Variant& constructValue, const gimbal::Variant* pExpectedValue=nullptr, char* pString=nullptr, gimbal::Result expectedResult=gimbal::Result::Success, T**ppOut=nullptr) {
        GBL_API_BEGIN(hCtx());
        char* pStringBuff = pString;
        T* pData;
        bool fromSerialized = false;
        if(!pStringBuff) { //then just take it from serialized
            serialize(constructValue, nullptr, gimbal::Result::Success, &pData, &pStringBuff);
            fromSerialized = true;
        } else {
            pData = create_(constructValue);
        }
        const gimbal::Size bytesIn = strlen(pStringBuff) + 1;
        gimbal::Size bytesOut = bytesIn;
        auto blk = GBL_API_TRY_BLOCK {
            getMetaType().deserialize(pCtx(), pData, reinterpret_cast<const void*>(pStringBuff), &bytesOut);
        };
        QCOMPARE(blk.getRecord().getResult(), expectedResult);
        if(fromSerialized) QCOMPARE(bytesOut, bytesIn);
        if(pExpectedValue) {
            int cmpResult = -1;
            auto blk = GBL_API_TRY_BLOCK {
                cmpResult = getMetaType().compare(pCtx(), pData, *pExpectedValue);
            };
            QCOMPARE(blk.getRecord().getResult(), gimbal::Result::Success);
            QVERIFY(GBL_COMPARISON_VERIFY_OP_CMP_TYPE(cmpResult, GBL_VARIANT_OP_CMP_EQUAL));
        }
        conditionalDestroy_(pData, ppOut);
        GBL_API_END_BLOCK();
        if(fromSerialized) GBL_API_FREE(pStringBuff);
    }
};

class MetaType: public MetaTypeDerived<QString> {
    Q_OBJECT
public:
    using Base = MetaTypeDerived<QString>;

    MetaType(gimbal::test::MonitorableContext* pCtx=nullptr):
        MetaTypeDerived(pCtx? pCtx : new gimbal::test::TestContext()) {}

private slots:
    void create(void) {
        Base::create(gimbal::Variant(12),              QString("12"));
        Base::create(gimbal::Variant(12.3f),           QString("12.300"));
        Base::create(gimbal::String("1234", pCtx()),   QString("1234"));
    }

    void destroy(void) {
        Base::destroy(gimbal::Variant(12));
        Base::destroy(gimbal::Variant(12.3f));
        Base::destroy(gimbal::String("124", pCtx()));
    }

    void convert(void) {
        Base::convert(12, gimbal::Variant(gimbal::Variant::Type::Int),      gimbal::constptr_cast(gimbal::Variant{12}));
        Base::convert(12, gimbal::Variant(gimbal::Variant::Type::String),   gimbal::constptr_cast(gimbal::Variant(gimbal::String("12", pCtx()))));
        Base::convert(12, gimbal::Variant(gimbal::Variant::Type::Float),    gimbal::constptr_cast(gimbal::Variant(12.0f)));
    }

    void compare(void) {
        Base::compare(12,           12);
        Base::compare(12,           gimbal::String("12", pCtx()));
        Base::compare("12.000",     12.0f);
    }

    void serialize(void) {
        Base::serialize(12,              "12");
        Base::serialize(12.0f,           "12.000");
        Base::serialize("Fucktweeze",    "Fucktweeze");
        Base::serialize(true,            "true");
        Base::serialize(nullptr,          "nil");
    }

    void deserialize(void) {
        Base::deserialize(12);
        Base::deserialize(12.0f);
        Base::deserialize("lulz");
        Base::deserialize(false);
        Base::deserialize(nullptr);
        Base::deserialize(reinterpret_cast<void*>(this));
    }
};


}

#endif // TEST_GIMBAL_META_HPP
