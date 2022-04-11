#ifndef TEST_GIMBAL_BUFFER_HPP
#define TEST_GIMBAL_BUFFER_HPP


#include <gimbal/types/gimbal_buffer.hpp>
#include "test_gimbal.hpp"


namespace gimbal::test {

struct StringBuffer: public gimbal::Buffer {

    StringBuffer(void) = default;

    StringBuffer(QString string):
        StringBuffer(std::move(string), nullptr) {}

    StringBuffer(QString string, gimbal::Context* pContext):
        gimbal::Buffer(string.length()+1, Q_CSTR(string), pContext) {}


    bool operator==(StringBuffer rhs) const {
        return *static_cast<const gimbal::Buffer*>(this) == *static_cast<const gimbal::Buffer*>(&rhs);
    }

    bool operator!=(StringBuffer rhs) const {
        return !(*this == rhs);
    }

    bool memcmp(const char* pData, int length) const {
        gimbal::Buffer temp(length, pData, getContext());
        return *this == temp;
    }

    QString getString(void) const {
        return QString(reinterpret_cast<const char*>(getData()));
    }

    QString getSubString(Size offset, Size bytes) const {
        char* pCharBuff = reinterpret_cast<char*>(GBL_ALLOCA(bytes));
        readValue(pCharBuff, offset, bytes);
        return QString(pCharBuff);
    }

    void setSubString(QString str, Size offset) {
        writeValue(Q_CSTR(str), offset, str.length());
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

//error strings?
class Buffer: public UnitTestSet {
Q_OBJECT
private:

private slots:
    void init(void);
    void constructDefault(void);
    void constructBytes(void);
    void constructData(void);
    void constructCopy(void);
    void constructMove(void);
    void assignCopy(void);
    void assignMove(void);
    void acquireRelease(void);
    void clear(void);
    void resize(void);
    void grow(void);
    void shrink(void);
    void erase(void);
    void insert(void);
    void append(void);
    void prepend(void);
    void readValue(void);
    void writeValue(void);
};

inline void Buffer::init(void) {
  //  GblContext_globalSet(hCtx());
}

inline void Buffer::constructDefault(void) {
    gimbal::Buffer buffer;
    gimbal::Buffer buffer2;

    QVERIFY(buffer.isEmpty());
    QCOMPARE(buffer.getSize(), 0);
    QCOMPARE(buffer.getData(), nullptr);
    QVERIFY(buffer == buffer2);
}

inline void Buffer::constructBytes(void) {
    ContextScopeGuard<ContextActiveAllocMonitor> allocGuard(pCtx());
    {
        uint8_t cmpBuff[32] = { 0 };
        gimbal::Buffer buffer(32, pCtx());
        gimbal::Buffer buffer2(32, pCtx());
        QCOMPARE(allocGuard.getCurrentDelta(), 2);

        QCOMPARE(buffer.getContext(), pCtx());
        QVERIFY(!buffer.isEmpty());
        QCOMPARE(buffer.getSize(), 32);
        QCOMPARE(memcmp(buffer.getData(), cmpBuff, sizeof(cmpBuff)), 0);
        QVERIFY(buffer == buffer2);
    }
}

inline void Buffer::constructData(void) {
    ContextScopeGuard<ContextActiveAllocMonitor> allocGuard(pCtx());
    {
        char srcBuff[128];
        strcpy(srcBuff, "Trololo");

        gimbal::Buffer buffer(sizeof(srcBuff), srcBuff, pCtx());
        gimbal::Buffer buffer2(sizeof(srcBuff), srcBuff, pCtx());
        QCOMPARE(allocGuard.getCurrentDelta(), 2);

        QCOMPARE(buffer.getSize(), sizeof(srcBuff));
        QCOMPARE(memcmp(buffer.getData(), srcBuff, sizeof(srcBuff)), 0);
        QVERIFY(buffer == buffer2);
    }
}

inline void Buffer::constructCopy(void) {
    ContextScopeGuard<ContextActiveAllocMonitor> allocGuard(pCtx());
    {
        char srcBuff[128];
        strcpy(srcBuff, "Fuckrowaddlies");

        gimbal::Buffer buffer1;
        gimbal::Buffer buffer2(buffer1);

        QVERIFY(buffer1 == buffer2);

        gimbal::Buffer  buffer3(sizeof(srcBuff), srcBuff);
        gimbal::Buffer  buffer4(buffer3);
        gimbal::Buffer  buffer5(buffer4, pCtx());
        QCOMPARE(allocGuard.getCurrentDelta(), 1);

        QVERIFY(buffer3 == buffer4);
        QVERIFY(buffer4 == buffer5);

        QCOMPARE(memcmp(buffer4.getData(), srcBuff, sizeof(srcBuff)), 0);
    }
}

inline void Buffer::constructMove(void) {
    ContextScopeGuard<ContextActiveAllocMonitor> allocGuard(pCtx());
    {
        StringBuffer srcBuff("SOURCERY", pCtx());

        QCOMPARE(allocGuard.getCurrentDelta(), 1);
        gimbal::Buffer dstBuff(std::move(srcBuff));
        QCOMPARE(allocGuard.getCurrentDelta(), 1);

        QCOMPARE(dstBuff.getData<const char*>(), "SOURCERY");

        QVERIFY(dstBuff != srcBuff);
        QVERIFY(srcBuff.isEmpty());
        QCOMPARE(srcBuff.getData(), nullptr);
        QCOMPARE(srcBuff.getSize(), 0);
    }
}

inline void Buffer::assignCopy(void) {
    ContextScopeGuard<ContextActiveAllocMonitor> allocGuard(pCtx());
    {
        StringBuffer src1("xxxxx", pCtx());
        QCOMPARE(allocGuard.getCurrentDelta(), 1);

        gimbal::Buffer dst(src1);
        QCOMPARE(allocGuard.getCurrentDelta(), 2);

        StringBuffer src2("zzzzz", pCtx());
        QCOMPARE(allocGuard.getCurrentDelta(), 3);

        dst = src2;
        QCOMPARE(allocGuard.getCurrentDelta(), 3);

        QCOMPARE(dst.getData<const char*>(), "zzzzz");
        QCOMPARE(dst, src2);
    }
}

inline void Buffer::assignMove(void) {
    // construct RAII-style allocator monitor which tracks # of allocations while active
    ContextScopeGuard<ContextActiveAllocMonitor> allocGuard(pCtx());
    {
        // construct a buffer by value (+1 allocation)
        StringBuffer src1("xxxxx", pCtx());
        QCOMPARE(allocGuard.getCurrentDelta(), 1);

        // copy construct from existing buffer (+1 allocation)
        gimbal::Buffer dst(src1, pCtx());
        QCOMPARE(allocGuard.getCurrentDelta(), 2);

        // construct another buffer by value (+1 allocation)
        StringBuffer src2("zzzzz", pCtx());
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

inline void Buffer::acquireRelease(void) {
    ContextScopeGuard<ContextActiveAllocMonitor> allocGuard(pCtx());
    {
        StringBuffer src("AssDoucher", pCtx());
        StringBuffer dst("DickLicker", pCtx());

        dst.acquire(src.release());

        QCOMPARE(allocGuard.getCurrentDelta(), 1);

        QCOMPARE(dst.getData<const char*>(), "AssDoucher");
        QVERIFY(src.isEmpty());
    }
}

inline void Buffer::clear(void) {
    ContextScopeGuard<ContextActiveAllocMonitor> allocGuard(pCtx());
    StringBuffer buff("ASSLOLZER");
    buff.clear();
    QCOMPARE(allocGuard.getCurrentDelta(), 0);
}

inline void Buffer::resize(void) {
    ContextScopeGuard<ContextActiveAllocMonitor> allocGuard(pCtx());
    {
        StringBuffer buffer1("CuntBezoar", pCtx());
        StringBuffer buffer2(buffer1);
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

inline void Buffer::grow(void) {

}

inline void Buffer::shrink(void) {

}

inline void Buffer::erase(void) {

}

inline void Buffer::insert(void) {

}

inline void Buffer::append(void) {

}

inline void Buffer::prepend(void) {

}

inline void Buffer::readValue(void) {


}

inline void Buffer::writeValue(void) {

}

}

#endif // TEST_GIMBAL_BUFFER_HPP
