#ifndef TEST_GIMBAL_STRING_HPP
#define TEST_GIMBAL_STRING_HPP

#define GBL_STRING_BUFFER_BASE_STACK_SIZE_CACHE GBL_STRING_BUFFER_BASE_STACK_SIZE
#undef GBL_STRING_BUFFER_BASE_STACK_SIZE
#define GBL_STRING_BUFFER_BASE_STACK_SIZE 8

#include "test_gimbal.hpp"
#include <gimbal/gimbal_string.hpp>
#include <sstream>
namespace gimbal::test {

/* Different types:
1) StringView
2) GblString
3) String

Different axes:
1) Stack size
2) Stack vs Heap
3) Context vs No Context

*/

class String: public elysian::UnitTestSet {
Q_OBJECT
public:
    String(gimbal::Context* pCtx=nullptr):
        pCtx_(pCtx) {}
private:

    gimbal::Context* pCtx_ = nullptr;

    void verifyString_(const gimbal::String& str, const char* pString);
    void verifyStack_(const gimbal::String& str, gimbal::Size stackSize=GBL_STRING_BUFFER_BASE_STACK_SIZE);
    void verifyHeap_(const gimbal::String& str);
private slots:
    void createDefault(void);
    void createNullptr(void);

    void createStackCString(void);
    void createStackLiteral(void);
    void createStackStdString(void);
    void createStackStringView(void);
    void createStackCCopy(void);
    void createStackCMove(void);
    void createStackCppCopy(void);
    void createStackCppMove(void);
    void createHeapCString(void);

    void assignHeapCCopy(void);
    void assignHeapCMove(void);
    void assignHeapCppCopy(void);
    void assignHeapCppMove(void);
    void assignHeapValueTypes(void);

    void operatorSubscriptRead(void);
    void operatorSubscriptWrite(void);

    void stdIteratorConst(void);
    void stdIterator(void);
    void rangeBasedFor(void);
    void stdHash(void);
    void stdSwap(void);
    void stdOStream(void);
    void stdIStream(void);

    void clear(void);
    void reserve(void);
    void resize(void);

    void sprintf(void);
    void concat(void);

    void operatorAdd(void);
};

using namespace gimbal;


inline void String::verifyString_(const gimbal::String& str, const char* pCStr) {
    QCOMPARE(str.getCString(), pCStr);
    QCOMPARE(str, pCStr);
    QVERIFY(!(str != pCStr));

    QCOMPARE(str.toStdString(), pCStr);
    QCOMPARE(str, std::string(pCStr));
    QVERIFY(!(str != std::string(pCStr)));

    QCOMPARE(str.toStringView(), pCStr);
    QVERIFY(str == std::string_view(pCStr));
    QVERIFY(!(str != std::string_view(pCStr)));

    QCOMPARE(str, gimbal::String(pCStr));
    QVERIFY(!(str != gimbal::String(pCStr)));

    const auto strlen = std::strlen(pCStr);
    QVERIFY(str.getLength() == strlen);
    if(!strlen) QVERIFY(str.isEmpty());
    else QVERIFY(!str.isEmpty());
}

inline void String::verifyStack_(const gimbal::String& str, gimbal::Size stackSize) {
    QCOMPARE(str.getCapacity(), stackSize);
    QCOMPARE(str.getStackSize(), stackSize);
    QVERIFY(str.isStack());
    QVERIFY(!str.isHeap());
    QCOMPARE(str.getCString(), str.stackBuffer);
}

inline void String::verifyHeap_(const gimbal::String& str) {
    QVERIFY(!str.isStack());
    QVERIFY(str.isHeap());
    QVERIFY(str.getCString() != str.stackBuffer);
    QVERIFY(str.getCapacity() > str.getStackSize());
}

inline void String::createDefault(void) {
    gimbal::String string;
    QCOMPARE(string.getContext(), nullptr);
    verifyString_(string, "");
    verifyStack_(string);
}

inline void String::createNullptr(void) {
    gimbal::String string(nullptr);
    QCOMPARE(string.getContext(), nullptr);
    verifyString_(string, "");
    verifyStack_(string);
}

inline void String::createStackCString(void) {
    gimbal::String string("Fuck");
    QCOMPARE(string.getContext(), nullptr);
    verifyString_(string, "Fuck");
    verifyStack_(string);
}

inline void String::createStackLiteral(void) {
    auto string = "Literal"_gstr;
    QCOMPARE(string.getContext(), nullptr);
    verifyString_(string, "Literal");
    verifyStack_(string);
}

inline void String::createStackStdString(void) {
    gimbal::String string(std::string("CppStr"));
    QCOMPARE(string.getContext(), nullptr);
    verifyString_(string, "CppStr");
    QCOMPARE(string, "CppStr"_gstr);
    verifyStack_(string);
}

inline void String::createStackStringView(void) {
    gimbal::String string(std::string_view("StrView"));
    QCOMPARE(string.getContext(), nullptr);
    verifyString_(string, "StrView");
    QCOMPARE("StrView"_gstr, string);
    verifyStack_(string);
}

inline void String::createStackCCopy(void) {
    const auto view = GblStringView { "CGblStr" };
    GblString gStr;
    gblStringConstruct(&gStr, sizeof(GblString), nullptr, &view);

    gimbal::String string(gStr);
    QCOMPARE(string.getContext(), nullptr);
    verifyString_(string, "CGblStr");
    verifyStack_(string);
    gblStringDestruct(&gStr);
}

inline void String::createStackCMove(void) {
    const auto view = GblStringView { "MovC" };
    GblString gStr;
    GBL_TEST_VERIFY_RESULT(gblStringConstruct(&gStr, sizeof(GblString), nullptr, &view));

    gimbal::String string(std::move(gStr));
    QCOMPARE(string.getContext(), nullptr);
    verifyString_(string, "MovC");
    verifyStack_(string);
    GBL_TEST_VERIFY_RESULT(gblStringDestruct(&gStr));
}

inline void String::createStackCppCopy(void) {
    gimbal::String str1("GblStr");
    gimbal::String string(str1);
    QCOMPARE(string.getContext(), nullptr);
    verifyString_(string, "GblStr");
    verifyStack_(string);
}

inline void String::createStackCppMove(void) {
    gimbal::String string(gimbal::String{"GblStr"});
    QCOMPARE(string.getContext(), nullptr);
    verifyString_(string, "GblStr");
    verifyStack_(string);
}


inline void String::createHeapCString(void) {
    gimbal::String string("123456789");
    QCOMPARE(string.getContext(), nullptr);
    verifyString_(string, "123456789");
    verifyHeap_(string);
}

inline void String::assignHeapCCopy(void) {
    const char* pCStr = "CGblStrHeap";
    const auto view = GblStringView { pCStr };
    GblString gStr;

    GBL_TEST_VERIFY_RESULT(gblStringConstruct(&gStr, sizeof(GblString), nullptr, &view));

    gimbal::String string;
    string = gStr;
    verifyString_(string, pCStr);
    verifyHeap_(string);

    GBL_TEST_VERIFY_RESULT(gblStringDestruct(&gStr));
}

inline void String::assignHeapCMove(void) {
    const char* pCStr = "CGblStrHeap";
    const auto view = GblStringView { pCStr };
    GblString gStr;

    GBL_TEST_VERIFY_RESULT(gblStringConstruct(&gStr, sizeof(GblString), nullptr, &view));

    gimbal::String string;
    string = std::move(gStr);
    verifyString_(string, pCStr);
    verifyHeap_(string);
    StringView gblView(gStr);
    QVERIFY(gblView.isStack());

    GBL_TEST_VERIFY_RESULT(gblStringDestruct(&gStr));
}

inline void String::assignHeapCppCopy(void) {
    gimbal::String str1;
    auto str2 = "Test12345678"_gstr;
    str1 = str2;
    QCOMPARE(str1, str2);
    verifyString_(str1, "Test12345678");
    verifyString_(str2, "Test12345678");
    QVERIFY(str1.getCString() != str2.getCString());
    verifyHeap_(str2);
    verifyHeap_(str1);
}

inline void String::assignHeapCppMove(void) {
    gimbal::String str1;
    auto str2 = "Test12345678"_gstr;
    const char* pCStr = str2.getCString();
    str1 = std::move(str2);
    verifyString_(str1, "Test12345678");
    QVERIFY(str1.getCString() == pCStr);
    verifyStack_(str2);
    verifyHeap_(str1);
}

inline void String::assignHeapValueTypes(void) {
    gimbal::String str1("123456789");
    gimbal::String str2(str1);
    verifyString_(str2, "123456789");
    verifyHeap_(str2);
    str2 = "HEAPITY HEAP!!!";
    verifyString_(str2, "HEAPITY HEAP!!!");
    verifyHeap_(str2);
    str2 = std::string("Fuckweazler");
    verifyString_(str2, "Fuckweazler");
    verifyHeap_(str2);
    str2 = std::string_view("VIEW TO A KILL");
    verifyString_(str2, "VIEW TO A KILL");
    verifyHeap_(str2);
    str2 = nullptr;
    verifyString_(str2, "");
    verifyStack_(str2);
}

inline void String::operatorSubscriptRead(void) {
    const char* pCStr1 = "123456789";
    const char* pCStr2 = "123";
    gimbal::String str1 { pCStr1 };
    gimbal::String str2 { pCStr2 };
    gimbal::String str3;

    auto testElements = [&](const gimbal::String& str, const char* pCStr) {
        for(size_t c = 0; c < strlen(pCStr); ++c) {
            QCOMPARE(str[c], pCStr[c]);
        }
    };

    testElements(str1, pCStr1);
    testElements(str2, pCStr2);

    char c;
    GBL_TEST_VERIFY_EXCEPTION_THROWN(c = str1[-1],  gimbal::Result::ErrorOutOfBounds);
    GBL_TEST_VERIFY_EXCEPTION_THROWN(c = str1[9],   gimbal::Result::ErrorOutOfBounds);
    GBL_TEST_VERIFY_EXCEPTION_THROWN(c = str2[3],   gimbal::Result::ErrorOutOfBounds);
    GBL_TEST_VERIFY_EXCEPTION_THROWN(c = str3[0],   gimbal::Result::ErrorOutOfBounds);
}

inline void String::operatorSubscriptWrite(void) {
    const char* pCStr = "000000000";
    gimbal::String string { pCStr };

    for(gimbal::Size c = 0; c < string.getLength(); ++c) {
        string[c] = 'a' + c;
    }

    verifyString_(string, "abcdefghi");
    GBL_TEST_VERIFY_EXCEPTION_THROWN(string[-1] = 0,  gimbal::Result::ErrorOutOfBounds);
    GBL_TEST_VERIFY_EXCEPTION_THROWN(string[10] = 0,  gimbal::Result::ErrorOutOfBounds);
}

inline void String::stdIteratorConst(void) {
    gimbal::String str1 { "1234567890" };
    size_t index = 0;
    for(auto it = str1.cbegin(); it != str1.cend(); ++it) {
        QCOMPARE(*it, str1[index++]);
    }

    index = str1.getLength()-1;
    for(auto it = str1.crbegin(); it != str1.crend(); ++it) {
        QCOMPARE(*it, str1[index--]);
    }
}

inline void String::stdIterator(void) {
    const char* pCStr = "000000000";
    gimbal::String string { pCStr };

    uint32_t offset = 0;
    for(auto it = string.begin(); it != string.end(); ++it) {
        *it = 'a' + (offset++);
    }
    verifyString_(string, "abcdefghi");

    offset = 0;
    for(auto it = string.rbegin(); it!= string.rend(); ++it) {
        *it = 'a' + (offset++);
    }
    verifyString_(string, "ihgfedcba");
}


inline void String::rangeBasedFor(void) {
    gimbal::String str1 { "1234567890" };

    size_t index = 0;
    for(auto v : str1) {
        QCOMPARE(v, str1[index++]);
    }
}

inline void String::stdHash(void) {
    auto verifyHash = [&](const char* pCStr) {
        gimbal::String str1 { pCStr };
        QCOMPARE(std::hash<gimbal::String>{}(str1), std::hash<std::string>{}(std::string(pCStr)));
    };

    verifyHash("1234567890");
    verifyHash("");
    verifyHash("-------------------------------");
}

inline void String::stdSwap(void) {
    {
        gimbal::String str1("HELLO1");
        gimbal::String str2("");
        std::swap(str1, str2);
        verifyString_(str1, "");
        verifyString_(str2, "HELLO1");
    }
    {
        gimbal::String str1("abgda");
        gimbal::String str2("0000000000");
        std::swap(str1, str2);
        verifyString_(str1, "0000000000");
        verifyString_(str2, "abgda");
    }
}

inline void String::stdOStream(void) {
    auto verifyOStream = [&](const char* pCStr) {
        std::stringstream ss;
        const gimbal::String gblStr(pCStr);
        ss << gblStr;
        QCOMPARE(ss.str(), pCStr);
    };

    verifyOStream("");
    verifyOStream("2lkj2r23  VVsdkljrew");
    verifyOStream("a");
}

inline void String::stdIStream(void) {
    auto verifyIStream = [&](const char* pCStr) {
        std::stringstream ss;
        gimbal::String gblStr;
        ss << pCStr;
        ss >> gblStr;
        QCOMPARE(gblStr, pCStr);
    };

    verifyIStream("");
    verifyIStream("2lkj2r23  VVsdkljrew");
    verifyIStream("a");
}

inline void String::clear(void) {
    {
        gimbal::String string("TROLO");
        string.clear();
        verifyString_(string, "");
        verifyStack_(string);
    }
    {
        gimbal::String string("1234567890");
        string.clear();
        verifyString_(string, "");
        verifyStack_(string);
    }
}

inline void String::reserve(void) {
    gimbal::String string("TROLO");
    string.reserve(3);
    verifyString_(string, "TROLO");
    verifyStack_(string);
    string.reserve(GBL_STRING_BUFFER_BASE_STACK_SIZE+1);
    verifyString_(string, "TROLO");
    verifyHeap_(string);
    string.reserve(string.getCapacity() + 256);
    verifyString_(string, "TROLO");
    verifyHeap_(string);
}

inline void String::resize(void) {
    gimbal::String string("TROLO");
    string.resize(3);
    verifyString_(string, "TRO");
    verifyStack_(string);
    string.resize(GBL_STRING_BUFFER_BASE_STACK_SIZE+1);
    QCOMPARE(string.getLength(), GBL_STRING_BUFFER_BASE_STACK_SIZE+1);
    verifyHeap_(string);
    QCOMPARE(string.toStringView().substr(0, 3), "TRO");
}

inline void String::sprintf(void) {
    gimbal::String str1;
    str1.sprintf("");
    verifyString_(str1, "");
    verifyStack_(str1);

    str1.sprintf("Hiya %d", 3);
    verifyString_(str1, "Hiya 3");
    verifyStack_(str1);

    str1.sprintf("String:%s Uint:%u Float:%.2f Char:%c", "Test", 12, -33.33f, 't');
    verifyString_(str1, "String:Test Uint:12 Float:-33.33 Char:t");
    verifyHeap_(str1);
}

inline void String::concat(void) {
    gimbal::String str;
    str.concat("My ");
    verifyString_(str, "My ");
    verifyStack_(str);
    str += "shit ";
    str += std::string("is ");
    str += std::string_view("quite ");
    str += gimbal::String("pristine!");
    str += " ...and literal!"_gstr;
    verifyString_(str, "My shit is quite pristine! ...and literal!");
    verifyHeap_(str);
}

inline void String::operatorAdd(void) {
    GblString cStr{};
    const GblStringView cView { "gblCStr" };
    GBL_TEST_VERIFY_RESULT(gblStringConstruct(&cStr, sizeof(GblString), GBL_HANDLE_INVALID, &cView));

    gimbal::String str = gimbal::String()               +
                        "cStr "                         +
                        std::string("cppStr ")          +
                        std::string_view("cppView ")    +
                        gimbal::String("TROLOLO ")      +
                        cStr                            +
                        " "                             +
                        "GStringLiteral"_gstr;

    verifyString_(str, "cStr cppStr cppView TROLOLO gblCStr GStringLiteral");
    verifyHeap_(str);

    GBL_TEST_VERIFY_RESULT(gblStringDestruct(&cStr));
}







#undef GBL_STRING_BUFFER_BASE_STACK_SIZE
#define GBL_STRING_BUFFER_BASE_STACK_SIZE GBL_STRING_BUFFER_BASE_STACK_SIZE_CACHE
#undef GBL_STRING_BUFFER_BASE_STACK_SIZE_CACHE




}

#endif // TEST_GIMBAL_STRING_HPP
