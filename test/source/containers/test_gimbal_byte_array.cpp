#include "test_gimbal_byte_array.hpp"

namespace gimbal::test {

struct StringByteArray: public gimbal::ByteArray {

    StringByteArray(void) = default;

    StringByteArray(QString string):
        gimbal::ByteArray(string.length()+1, Q_CSTR(string)) {}


    bool operator==(StringByteArray rhs) const {
        return *static_cast<const gimbal::ByteArray*>(this) == *static_cast<const gimbal::ByteArray*>(&rhs);
    }

    bool operator!=(StringByteArray rhs) const {
        return !(*this == rhs);
    }

    bool memcmp(const char* pData, int length) const {
        gimbal::ByteArray temp(length, pData);
        return *this == temp;
    }

    QString getString(void) const {
        return QString(reinterpret_cast<const char*>(getData()));
    }

    QString getSubString(Size offset, Size bytes) const {
        char* pCharBuff = reinterpret_cast<char*>(GBL_ALLOCA(bytes));
        readData(pCharBuff, offset, bytes);
        return QString(pCharBuff);
    }

    void setSubString(QString str, Size offset) {
        writeData(Q_CSTR(str), offset, str.length());
    }

    void insertString(QString str, Size offset) {
        insert(Q_CSTR(str), offset, str.length());
    }

    void appendString(QString str) {
        append(Q_CSTR(str), str.length());
    }

    void prependString(QString str) {
        prepend(Q_CSTR(str), str.length());
    }
};


void ByteArray::init(void) {
  //  GblContext_globalSet(hCtx());
}

void ByteArray::constructDefault(void) {
    gimbal::ByteArray buffer;
    gimbal::ByteArray buffer2;

    QVERIFY(buffer.isEmpty());
    QCOMPARE(buffer.getSize(), 0);
    QCOMPARE(buffer.getData(), nullptr);
    QVERIFY(buffer == buffer2);
}

void ByteArray::constructBytes(void) {
    ContextScopeGuard<ContextActiveAllocMonitor> allocGuard(pCtx());
    {
        uint8_t cmpBuff[32] = { 0 };
        gimbal::ByteArray buffer(32);
        gimbal::ByteArray buffer2(32);
        QCOMPARE(allocGuard.getCurrentDelta(), 2);

        QVERIFY(!buffer.isEmpty());
        QCOMPARE(buffer.getSize(), 32);
        QCOMPARE(memcmp(buffer.getData(), cmpBuff, sizeof(cmpBuff)), 0);
        QVERIFY(buffer == buffer2);
    }
}

void ByteArray::constructData(void) {
    ContextScopeGuard<ContextActiveAllocMonitor> allocGuard(pCtx());
    {
        char srcBuff[128];
        strcpy(srcBuff, "Trololo");

        gimbal::ByteArray buffer(sizeof(srcBuff), srcBuff);
        gimbal::ByteArray buffer2(sizeof(srcBuff), srcBuff);
        QCOMPARE(allocGuard.getCurrentDelta(), 2);

        QCOMPARE(buffer.getSize(), sizeof(srcBuff));
        QCOMPARE(memcmp(buffer.getData(), srcBuff, sizeof(srcBuff)), 0);
        QVERIFY(buffer == buffer2);
    }
}

void ByteArray::constructCopy(void) {
    ContextScopeGuard<ContextActiveAllocMonitor> allocGuard(pCtx());
    {
        char srcBuff[128];
        strcpy(srcBuff, "Fuckrowaddlies");

        gimbal::ByteArray buffer1;
        gimbal::ByteArray buffer2(buffer1);

        QVERIFY(buffer1 == buffer2);

        gimbal::ByteArray  buffer3(sizeof(srcBuff), srcBuff);
        gimbal::ByteArray  buffer4(buffer3);
        gimbal::ByteArray  buffer5(buffer4);
        QCOMPARE(allocGuard.getCurrentDelta(), 1);

        QVERIFY(buffer3 == buffer4);
        QVERIFY(buffer4 == buffer5);

        QCOMPARE(memcmp(buffer4.getData(), srcBuff, sizeof(srcBuff)), 0);
    }
}

void ByteArray::constructMove(void) {
    ContextScopeGuard<ContextActiveAllocMonitor> allocGuard(pCtx());
    {
        StringByteArray srcBuff("SOURCERY");

        QCOMPARE(allocGuard.getCurrentDelta(), 1);
        gimbal::ByteArray dstBuff(std::move(srcBuff));
        QCOMPARE(allocGuard.getCurrentDelta(), 1);

        QCOMPARE(dstBuff.getData<const char*>(), "SOURCERY");

        QVERIFY(dstBuff != srcBuff);
        QVERIFY(srcBuff.isEmpty());
        QCOMPARE(srcBuff.getData(), nullptr);
        QCOMPARE(srcBuff.getSize(), 0);
    }
}

void ByteArray::assignCopy(void) {
    ContextScopeGuard<ContextActiveAllocMonitor> allocGuard(pCtx());
    {
        StringByteArray src1("xxxxx");
        QCOMPARE(allocGuard.getCurrentDelta(), 1);

        gimbal::ByteArray dst(src1);
        QCOMPARE(allocGuard.getCurrentDelta(), 2);

        StringByteArray src2("zzzzz");
        QCOMPARE(allocGuard.getCurrentDelta(), 3);

        dst = src2;
        QCOMPARE(allocGuard.getCurrentDelta(), 3);

        QCOMPARE(dst.getData<const char*>(), "zzzzz");
        QCOMPARE(dst, src2);
    }
}

void ByteArray::assignMove(void) {
    // construct RAII-style allocator monitor which tracks # of allocations while active
    ContextScopeGuard<ContextActiveAllocMonitor> allocGuard(pCtx());
    {
        // construct a buffer by value (+1 allocation)
        StringByteArray src1("xxxxx");
        QCOMPARE(allocGuard.getCurrentDelta(), 1);

        // copy construct from existing buffer (+1 allocation)
        gimbal::ByteArray dst(src1);
        QCOMPARE(allocGuard.getCurrentDelta(), 2);

        // construct another buffer by value (+1 allocation)
        StringByteArray src2("zzzzz");
        QCOMPARE(allocGuard.getCurrentDelta(), 3);

        // move contents of buffer2 into dst, freeing dst's previous contents (-1 allocation)
        dst = std::move(src2);
        QCOMPARE(allocGuard.getCurrentDelta(), 2);

        QCOMPARE(dst.getData<const char*>(), "zzzzz");
        QVERIFY(dst != src2);
        QVERIFY(src2.isEmpty());
    }
    // ensure all previous allocations were freed as buffers go out of scope
    QCOMPARE(allocGuard.getCurrentDelta(), 0);
    // allocGuard will automatically fail the test with a memory leak error if
    // any allocations persist as it goes out of scope.
}

void ByteArray::acquireRelease(void) {
    ContextScopeGuard<ContextActiveAllocMonitor> allocGuard(pCtx());
    {
        StringByteArray src("AssDoucher");
        StringByteArray dst("DickLicker");

        dst.acquire(src.release());

        QCOMPARE(allocGuard.getCurrentDelta(), 1);

        QCOMPARE(dst.getData<const char*>(), "AssDoucher");
        QVERIFY(src.isEmpty());
    }
}

void ByteArray::clear(void) {
    ContextScopeGuard<ContextActiveAllocMonitor> allocGuard(pCtx());
    StringByteArray buff("ASSLOLZER");
    buff.clear();
    QCOMPARE(allocGuard.getCurrentDelta(), 0);
}

void ByteArray::resize(void) {
    ContextScopeGuard<ContextActiveAllocMonitor> allocGuard(pCtx());
    {
        StringByteArray buffer1("CuntBezoar");
        StringByteArray buffer2(buffer1);
        buffer1.resize(buffer1.getSize());

        QCOMPARE(buffer1.getSize(), buffer2.getSize());
        QCOMPARE(buffer1, buffer2);

        buffer1.resize(4);
        QVERIFY(buffer1.memcmp("Cunt", 4));
        QCOMPARE(buffer1.getSize(), 4);

        buffer1.resize(10);
        QVERIFY(buffer1.memcmp("Cunt\0\0\0\0\0\0", 10));
        QCOMPARE(buffer1.getSize(), 10);

        QCOMPARE(allocGuard.getCurrentDelta(), 2);
    }
}

void ByteArray::grow(void) {

}

void ByteArray::shrink(void) {

}

void ByteArray::erase(void) {

}

void ByteArray::insert(void) {

}

void ByteArray::append(void) {

}

void ByteArray::prepend(void) {

}
void ByteArray::readValue(void) {


}

void ByteArray::writeValue(void) {

}


}
