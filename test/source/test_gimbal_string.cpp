#define GBL_VECTOR_STACK_BUFFER_DEFAULT_SIZE 8
#include <gimbal/types/gimbal_string.hpp>
#pragma GBL_PRAGMA_MACRO_PUSH("GBL_VECTOR_STACK_BUFFER_DEFAULT_SIZE");
#undef GBL_VECTOR_STACK_BUFFER_DEFAULT_SIZE
#define GBL_VECTOR_STACK_BUFFER_DEFAULT_SIZE 8
#include <test_gimbal_string.hpp>
#include <sstream>

namespace gimbal::test {


using namespace gimbal;

void String::verifyString_(const gimbal::String& str, const char* pCStr) {
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

void String::verifyStack_(const gimbal::String& str, gimbal::Size stackSize) {
    //QCOMPARE(str.getCapacity(), stackSize);
    //QCOMPARE(str.getStackBytes(), stackSize);
    //QVERIFY(str.isStack());
    //QVERIFY(!str.isHeap());
    //QCOMPARE(str.getCString(), str.data.stackBuffer);
}

void String::verifyHeap_(const gimbal::String& str) {
    QVERIFY(!str.isStack());
    QVERIFY(str.isHeap());
    QVERIFY(str.getCString() != GblStringBuffer_stackBuffer(&str));
    QVERIFY(str.getCapacity() > str.getStackBytes());
}

void String::cleanup(void) {
    QCOMPARE(getContext()->getAllocTracker().getActiveAllocationCount(), 0);
}

void String::createDefault(void) {
    const auto test = [&](MonitorableContext* pCtx=nullptr) {
        gimbal::String string(pCtx);
        QCOMPARE(string.getContext(), pCtx);
        verifyString_(string, "");
        verifyStack_(string);
    };
    test();
    test(pCtx());
}

void String::createNullptr(void) {
    const auto test = [&](MonitorableContext* pCtx=nullptr) {
        gimbal::String string(nullptr, pCtx);
        QCOMPARE(string.getContext(), pCtx);
        verifyString_(string, "");
        verifyStack_(string);
    };
    test();
    test(pCtx());
}

void String::createStackCString(void) {

    const auto test = [&](MonitorableContext* pCtx=nullptr) {
        gimbal::String string("Fuck", pCtx);
        QCOMPARE(string.getContext(), pCtx);
        verifyString_(string, "Fuck");
        verifyStack_(string);
    };
    test();
    test(pCtx());
}

void String::createStackLiteral(void) {
    auto string = "Literal"_gstr;
    QCOMPARE(string.getContext(), nullptr);
    verifyString_(string, "Literal");
    verifyStack_(string);
}

void String::createStackStdString(void) {
    const auto test = [&](MonitorableContext* pCtx=nullptr) {
        gimbal::String string(std::string("CppStr"), pCtx);
        QCOMPARE(string.getContext(), pCtx);
        verifyString_(string, "CppStr");
        QCOMPARE(string, "CppStr"_gstr);
        verifyStack_(string);
    };
    test();
    test(pCtx());
}

void String::createStackStdPmrString(void) {
    const auto test = [&](MonitorableContext* pCtx=nullptr) {
        gimbal::String string(std::pmr::string("CppStr", pCtx), pCtx);
        QCOMPARE(string.getContext(), pCtx);
        verifyString_(string, "CppStr");
        QCOMPARE(string, "CppStr"_gstr);
        verifyStack_(string);
    };
    //test();
    test(pCtx());
}

void String::createStackStringView(void) {
    const auto test = [&](MonitorableContext* pCtx=nullptr) {
        gimbal::String string(std::string_view("StrView"), pCtx);
        QCOMPARE(string.getContext(), pCtx);
        verifyString_(string, "StrView");
        QCOMPARE("StrView"_gstr, string);
        verifyStack_(string);
    };
    test();
    test(pCtx());
}

void String::createStackCCopy(void) {
    const auto view = GBL_STRV("CGblStr");
    GblStringBuffer gStr;
    GblStringBuffer_construct(&gStr, view);

    const auto test = [&](MonitorableContext* pCtx=nullptr) {
        gimbal::String string(gStr, pCtx);
        QCOMPARE(string.getContext(), pCtx);
        verifyString_(string, "CGblStr");
        verifyStack_(string);
    };
    test();
    test(pCtx());

    GblStringBuffer_destruct(&gStr);
}

void String::createStackCMove(void) {
    const auto view = GBL_STRV("MovC");
    GblStringBuffer gStr;
    GBL_TEST_VERIFY_RESULT(GblStringBuffer_construct(&gStr, view));

    const auto test = [&](MonitorableContext* pCtx=nullptr) {
    gimbal::String string(std::move(gStr), pCtx);
        QCOMPARE(string.getContext(), pCtx);
        verifyString_(string, "MovC");
        verifyStack_(string);
    };
    test();
    test(pCtx());

    GBL_TEST_VERIFY_RESULT(GblStringBuffer_destruct(&gStr));
}

void String::createStackCppCopy(void) {
    gimbal::String str1("GblStr");

    const auto test = [&](MonitorableContext* pCtx=nullptr) {
        gimbal::String string(str1, pCtx);
        QCOMPARE(string.getContext(), pCtx);
        verifyString_(string, "GblStr");
        verifyStack_(string);
    };
    test();
    test(pCtx());
}

void String::createStackCppMove(void) {
    const auto test = [&](MonitorableContext* pCtx=nullptr) {
        gimbal::String string(gimbal::String{"GblStr"}, pCtx);
        QCOMPARE(string.getContext(), pCtx);
        verifyString_(string, "GblStr");
        verifyStack_(string);
    };
    test();
    test(pCtx());
}


void String::createHeapCString(void) {
    const auto test = [&](MonitorableContext* pCtx=nullptr) {
        gimbal::String string("123456789", pCtx);
        QCOMPARE(string.getContext(), pCtx);
        verifyString_(string, "123456789");
        verifyHeap_(string);
    };
    test();

    auto block = GBL_TEST_CASE_API_BLOCK(pCtx(), "createHeapCString") {
        test(pCtx());
    };
    QCOMPARE(block.getCountersDelta().getExt(ContextCounters::ApiExtCall::MemAlloc), 1);
    QCOMPARE(block.getCountersDelta().getExt(ContextCounters::ApiExtCall::MemFree), 1);
    QCOMPARE(block.getActiveAllocCount(), 0);

}

void String::assignHeapCCopy(void) {
    const char* pCStr = "CGblStrHeap";
    const auto view = GBL_STRING_VIEW(pCStr);
    GblStringBuffer gStr;

    GBL_TEST_VERIFY_RESULT(GblStringBuffer_construct(&gStr, view));

    const auto test = [&](MonitorableContext* pCtx=nullptr) {
        gimbal::String string(pCtx);
        string = gStr;
        verifyString_(string, pCStr);
        verifyHeap_(string);
    };
    test(pCtx());

    auto block = GBL_TEST_API_BLOCK() {
        test(pCtx());
    };
    QCOMPARE(block.getCountersDelta().getExt(ContextCounters::ApiExtCall::MemAlloc), 1);
    QCOMPARE(block.getCountersDelta().getExt(ContextCounters::ApiExtCall::MemFree), 1);
    QCOMPARE(block.getActiveAllocCount(), 0);

    GBL_TEST_VERIFY_RESULT(GblStringBuffer_destruct(&gStr));
}

void String::assignHeapCMove(void) {
    const char* pCStr = "CGblStrHeap";
    const auto view =  GBL_STRV(pCStr);

    const auto test = [&](GblContext* pCtx1=nullptr,
                          MonitorableContext* pCtx2=nullptr)
    {
        GblStringBuffer gStr;
        GBL_TEST_VERIFY_RESULT(GblStringBuffer_construct(&gStr, view, sizeof(GblStringBuffer), pCtx1));

        gimbal::String string(pCtx2);
        string = std::move(gStr);
        verifyString_(string, pCStr);
        verifyHeap_(string);
        StringView gblView(gStr);
        QVERIFY(gblView.getContext() == pCtx2? gblView.isStack() : gblView.isHeap());
        GBL_TEST_VERIFY_RESULT(GblStringBuffer_destruct(&gStr));
    };
    test();

    auto block = GBL_TEST_API_BLOCK() {
        test(nullptr, pCtx());
    };
    QCOMPARE(block.getActiveAllocCount(), 0);
    QCOMPARE(block.getCountersDelta().getExt(ContextCounters::ApiExtCall::MemAlloc), 1);
    QCOMPARE(block.getCountersDelta().getExt(ContextCounters::ApiExtCall::MemFree), 1);
    block = GBL_TEST_API_BLOCK() {
        test(gblCtx(), nullptr);
    };
    QCOMPARE(block.getActiveAllocCount(), 0);
    QCOMPARE(block.getCountersDelta().getExt(ContextCounters::ApiExtCall::MemAlloc), 1);
    QCOMPARE(block.getCountersDelta().getExt(ContextCounters::ApiExtCall::MemFree), 1);
    block = GBL_TEST_API_BLOCK() {
        test(gblCtx(), pCtx());
    };
    QCOMPARE(block.getActiveAllocCount(), 0);
    QCOMPARE(block.getCountersDelta().getExt(ContextCounters::ApiExtCall::MemAlloc), 1);
    QCOMPARE(block.getCountersDelta().getExt(ContextCounters::ApiExtCall::MemFree), 1);
}

void String::assignHeapCppCopy(void) {
    const auto test = [&](MonitorableContext* pCtx=nullptr) {
        gimbal::String str1(pCtx);
        auto str2 = "Test12345678"_gstr;
        str1 = str2;
        QCOMPARE(str1, str2);
        verifyString_(str1, "Test12345678");
        verifyString_(str2, "Test12345678");
        QVERIFY(str1.getCString() != str2.getCString());
        verifyHeap_(str2);
        verifyHeap_(str1);
    };
    test();

    auto block = GBL_TEST_API_BLOCK() {
        test(pCtx());
    };
    QCOMPARE(block.getActiveAllocCount(), 0);
    QCOMPARE(block.getCountersDelta().getExt(ContextCounters::ApiExtCall::MemAlloc), 1);
    QCOMPARE(block.getCountersDelta().getExt(ContextCounters::ApiExtCall::MemFree), 1);
}

void String::assignHeapCppMove(void) {
    const auto test = [&](MonitorableContext* pCtx1=nullptr,
                          MonitorableContext* pCtx2=nullptr)
    {
        gimbal::String str1(pCtx1);
        gimbal::String str2 { "Test12345678", pCtx2 };
        const char* pCStr = str2.getCString();
        str1 = std::move(str2);
        verifyString_(str1, "Test12345678");
        if(pCtx1 == pCtx2) {
            QVERIFY(str1.getCString() == pCStr);
            verifyStack_(str2);
            verifyHeap_(str1);
        } else {
            QVERIFY(str1.getCString() != pCStr);
            verifyHeap_(str2);
            verifyHeap_(str1);
        }
    };
    test();

    auto block = GBL_TEST_API_BLOCK() {
        test(nullptr, pCtx());
    };
    QCOMPARE(block.getActiveAllocCount(), 0);
    QCOMPARE(block.getCountersDelta().getExt(ContextCounters::ApiExtCall::MemAlloc), 1);
    QCOMPARE(block.getCountersDelta().getExt(ContextCounters::ApiExtCall::MemFree), 1);

    block = GBL_TEST_API_BLOCK() {
        test(pCtx(), nullptr);
    };
    QCOMPARE(block.getActiveAllocCount(), 0);
    QCOMPARE(block.getCountersDelta().getExt(ContextCounters::ApiExtCall::MemAlloc), 1);
    QCOMPARE(block.getCountersDelta().getExt(ContextCounters::ApiExtCall::MemFree), 1);

    block = GBL_TEST_API_BLOCK() {
        test(pCtx(), pCtx());
    };
    QCOMPARE(block.getActiveAllocCount(), 0);
    QCOMPARE(block.getCountersDelta().getExt(ContextCounters::ApiExtCall::MemAlloc), 1);
    QCOMPARE(block.getCountersDelta().getExt(ContextCounters::ApiExtCall::MemFree), 1);
}

void String::assignHeapValueTypes(void) {
    const auto test = [&](MonitorableContext* pCtx=nullptr) {
        gimbal::String str1("123456789");
        gimbal::String str2(str1, pCtx);
        verifyString_(str2, "123456789");
        verifyHeap_(str2);
        str2 = "HEAPITY HEAP!!!";
        verifyString_(str2, "HEAPITY HEAP!!!");
        verifyHeap_(str2);
        str2 = std::string("Fuckweazlerdddddddddddddddddddddddddddddddddddd");
        verifyString_(str2, "Fuckweazlerdddddddddddddddddddddddddddddddddddd");
        verifyHeap_(str2);
        str2 = nullptr;
        verifyString_(str2, "");
        verifyStack_(str2);
        str2 = std::string_view("VIEW TO A KILL");
        verifyString_(str2, "VIEW TO A KILL");
        verifyHeap_(str2);
    };
    test();

    auto block = GBL_TEST_API_BLOCK() {
        test(pCtx());
    };
    QCOMPARE(block.getActiveAllocCount(), 0);
    QCOMPARE(getContext()->getAllocTracker().getActiveAllocationCount(), 0);

}

void String::operatorSubscriptRead(void) {
    const char* pCStr1 = "123456789";
    const char* pCStr2 = "123";
    gimbal::String str1 { pCStr1, pCtx() };
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
    GBL_TEST_VERIFY_EXCEPTION_THROWN(c = str1.at(-1),  gimbal::Result::ErrorOutOfRange);
    GBL_TEST_VERIFY_EXCEPTION_THROWN(c = str1.at(9),   gimbal::Result::ErrorOutOfRange);
    GBL_TEST_VERIFY_EXCEPTION_THROWN(c = str2.at(3),   gimbal::Result::ErrorOutOfRange);
    GBL_TEST_VERIFY_EXCEPTION_THROWN(c = str3.at(0),   gimbal::Result::ErrorOutOfRange);
}

void String::operatorSubscriptWrite(void) {
    const char* pCStr = "000000000";
    gimbal::String string { pCStr, pCtx() };

    for(gimbal::Size c = 0; c < string.getLength(); ++c) {
        string[c] = 'a' + c;
    }

    verifyString_(string, "abcdefghi");
    GBL_TEST_VERIFY_EXCEPTION_THROWN(string.at(-1) = 0,  gimbal::Result::ErrorOutOfRange);
    GBL_TEST_VERIFY_EXCEPTION_THROWN(string.at(10) = 0,  gimbal::Result::ErrorOutOfRange);
}

void String::stdIteratorConst(void) {
    gimbal::String str1 { "1234567890", pCtx() };
    size_t index = 0;
    for(auto it = str1.cbegin(); it != str1.cend(); ++it) {
        QCOMPARE(*it, str1[index++]);
    }

    index = str1.getLength()-1;
    for(auto it = str1.crbegin(); it != str1.crend(); ++it) {
        QCOMPARE(*it, str1[index--]);
    }

    GBL_TEST_VERIFY_EXCEPTION_THROWN(*str1.cend(),          gimbal::Result::ErrorOutOfRange);
    GBL_TEST_VERIFY_EXCEPTION_THROWN(*(str1.cbegin()-1),    gimbal::Result::ErrorOutOfRange);
    GBL_TEST_VERIFY_EXCEPTION_THROWN(*str1.crend(),         gimbal::Result::ErrorOutOfRange);
    GBL_TEST_VERIFY_EXCEPTION_THROWN(*(str1.crbegin()-1),   gimbal::Result::ErrorOutOfRange);
}

void String::stdIterator(void) {
    const char* pCStr = "000000000";
    gimbal::String string { pCStr, pCtx() };

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

    GBL_TEST_VERIFY_EXCEPTION_THROWN(*string.end() = 0,         gimbal::Result::ErrorOutOfRange);
    GBL_TEST_VERIFY_EXCEPTION_THROWN(*(string.begin()-1) = 0,   gimbal::Result::ErrorOutOfRange);
    GBL_TEST_VERIFY_EXCEPTION_THROWN(*(string.rend()) = 0,      gimbal::Result::ErrorOutOfRange);
    GBL_TEST_VERIFY_EXCEPTION_THROWN(*(string.rbegin()-1),      gimbal::Result::ErrorOutOfRange);
}

void String::rangeBasedFor(void) {
    gimbal::String str1 { "1234567890", pCtx() };

    size_t index = 0;
    for(auto v : str1) {
        QCOMPARE(v, str1[index++]);
    }

    index = 0;
    for(auto& v : str1) {
        v = index;
        QCOMPARE(str1.at(index), index);
        ++index;
    }

}

void String::stdHash(void) {
    auto verifyHash = [&](const char* pCStr) {
        gimbal::String str1 { pCStr, pCtx() };
        QCOMPARE(std::hash<gimbal::String>{}(str1), std::hash<std::string>{}(std::string(pCStr)));
    };

    verifyHash("1234567890");
    verifyHash("");
    verifyHash("-------------------------------");
}

// NOT ACTUALLY IN NAMESPACE STD!!!
void String::stdSwap(void) {
    using namespace gimbal;
    const auto test = [&](MonitorableContext* pCtx1=nullptr,
                          MonitorableContext* pCtx2=nullptr)
    {
        {
            gimbal::String str1("HELLO1", pCtx1);
            gimbal::String str2("", pCtx2);
            swap(str1, str2);
            verifyString_(str1, "");
            verifyString_(str2, "HELLO1");
            QCOMPARE(str1.getContext(), pCtx2);
            QCOMPARE(str2.getContext(), pCtx1);
        }
        {
            gimbal::String str1("abgda", pCtx1);
            gimbal::String str2("0000000000", pCtx2);
            swap(str1, str2);
            verifyString_(str1, "0000000000");
            verifyString_(str2, "abgda");
            QCOMPARE(str1.getContext(), pCtx2);
            QCOMPARE(str2.getContext(), pCtx1);
        }
    };
    // FUCKED TILL FURTHER NOTICE
    //test(nullptr, nullptr);
    //test(nullptr, pCtx());
    //test(pCtx(), nullptr);
    //test(pCtx(), pCtx());
}

void String::stdOStream(void) {
    auto verifyOStream = [&](const char* pCStr) {
        std::stringstream ss;
        const gimbal::String gblStr(pCStr, pCtx());
        ss << gblStr;
        QCOMPARE(ss.str(), pCStr);
    };

    verifyOStream("");
    verifyOStream("2lkj2r23  VVsdkljrew");
    verifyOStream("a");
}

void String::stdIStream(void) {
    auto verifyIStream = [&](const char* pCStr) {
        std::stringstream ss;
        gimbal::String gblStr(pCtx());
        ss << pCStr;
        ss >> gblStr;
        QCOMPARE(gblStr, pCStr);
    };

    verifyIStream("");
    verifyIStream("2lkj2r23  VVsdkljrew");
    verifyIStream("a");
}

void String::clear(void) {
    {
        gimbal::String string("TROLO");
        string.clear();
        verifyString_(string, "");
        verifyStack_(string);
    }
    {
        gimbal::String string("1234567890", pCtx());
        string.clear();
        verifyString_(string, "");
        verifyStack_(string);
    }
}

void String::reserve(void) {
    gimbal::String string("TROLO", pCtx());
    string.reserve(3);
    verifyString_(string, "TROLO");
    verifyStack_(string);
    string.reserve(string.getStackBytes() + 1);
    verifyString_(string, "TROLO");
    verifyHeap_(string);
    string.reserve(string.getCapacity() + 256);
    verifyString_(string, "TROLO");
    verifyHeap_(string);
}

void String::resize(void) {
    gimbal::String string("TROLO", pCtx());
    string.resize(3);
    verifyString_(string, "TRO");
    verifyStack_(string);
    string.resize(GBL_VECTOR_STACK_BUFFER_DEFAULT_SIZE+1);
    QCOMPARE(string.getLength(), GBL_VECTOR_STACK_BUFFER_DEFAULT_SIZE+1);
    verifyHeap_(string);
    QCOMPARE(string.toStringView().substr(0, 3), "TRO");
}

void String::sprintf(void) {
    gimbal::String str1(pCtx());
    str1.sprintf("");
    verifyString_(str1, "");
    verifyStack_(str1);

    str1.sprintf("Hiya %d", 3);
    verifyString_(str1, "Hiya 3");
    verifyStack_(str1);
    str1.clear();
    str1.sprintf("String:%s Uint:%u Float:%.2f Char:%c", "Test", 12, -33.33f, 't');
    verifyString_(str1, "String:Test Uint:12 Float:-33.33 Char:t");
    verifyHeap_(str1);
}

void String::varArgs(void) {
    const auto string = gimbal::String("Hello %s %.2f %u %c", pCtx()).varArgs("Bitches", -33.23f, 12, 'f');
    verifyString_(string, "Hello Bitches -33.23 12 f");
    verifyHeap_(string);
}

void String::concat(void) {
    gimbal::String str(pCtx());
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

void String::operatorAdd(void) {
    GblStringBuffer cStr{};
    const GblStringView cView = GBL_STRV("gblCStr");
    GBL_TEST_VERIFY_RESULT(GblStringBuffer_construct(&cStr, cView));

    gimbal::String str(pCtx());
    str =               gimbal::String()                +
                        "cStr "                         +
                        std::string("cppStr ")          +
                        std::string_view("cppView ")    +
                        gimbal::String("TROLOLO ")      +
                        cStr                            +
                        " "                             +
                        "GStringLiteral"_gstr;

    verifyString_(str, "cStr cppStr cppView TROLOLO gblCStr GStringLiteral");
    verifyHeap_(str);

    GBL_TEST_VERIFY_RESULT(GblStringBuffer_destruct(&cStr));
}

#pragma GBL_PRAGMA_MACRO_POP("GBL_VECTOR_STACK_BUFFER_DEFAULT_SIZE");





}
