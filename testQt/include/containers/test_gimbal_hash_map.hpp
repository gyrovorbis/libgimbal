#ifndef TEST_GIMBAL_HASH_HPP
#define TEST_GIMBAL_HASH_HPP

#include <gimbal/containers/gimbal_hash_map.hpp>
#include "test_gimbal.hpp"
#include <unordered_map>

#define GBL_TEST_HASH_MAP_BENCHMARK_READ_ENTRIES    10000
#define GBL_TEST_HASH_MAP_BENCHMARK_KEY_SIZE        30

namespace gimbal::test {


//error strings?
class HashMap: public UnitTestSet {
Q_OBJECT
private:

private slots:

    void init(void);
    void constructDefault(void);
    void sanityChecks(void);
    void benchmarkReadStd(void);
    void benchmarkReadQt(void);
    void benchmarkReadGimbal(void);
    void benchmarkWriteStd(void);
    void benchmarkWriteQt(void);
    void benchmarkWriteGimbal(void);
};

inline void HashMap::init(void) {
   // GblThread_stackFramePush(nullptr, nullptr);
}


inline void HashMap::constructDefault(void) {
    gimbal::HashMap<const char*, int> hash(pCtx());
    QCOMPARE(hash.size(), 0);
    QVERIFY(hash.bucket_size() >= sizeof(std::pair<const char*, int>));
    QCOMPARE(hash.context(), hCtx());
    QVERIFY(hash.empty());
}

inline void HashMap::sanityChecks(void) {
    gimbal::HashMap<const char*, int> hash({
                             { "Hello", 3 },
                             { "Bitch", 4 },
                             { "Bitcher", 5}
                         }, 0, pCtx());


    QCOMPARE(hash.size(), 3);
    QVERIFY(!hash.empty());

    QCOMPARE(hash.at("Bitch"), 4);
    QCOMPARE(hash.at("Bitcher"), 5);

    QCOMPARE(hash["Bitch"], 4);

    hash.insert({"Bitchrietta", 60});
    QCOMPARE(hash["Bitchrietta"], 60);

    QVERIFY(hash.contains("Bitchrietta"));
    QCOMPARE(hash.size(), 4);
    hash.erase("Bitchrietta");
    QVERIFY(!hash.contains("Bitchrietta"));
    QCOMPARE(hash.size(), 3);

    hash["Bitch"] = 42;
    QCOMPARE(hash["Bitch"], 42);

    hash["Briggsby"] = -333;
    QCOMPARE(hash.size(), 4);
    QCOMPARE(hash["Briggsby"], -333);

    auto it = hash.find("Briggsby");
    QVERIFY(it != hash.end());
    QCOMPARE((*it).first, "Briggsby");
    QCOMPARE((*it).second, -333);

    auto it2 = hash.find("NOPE");
    QVERIFY(it2 == hash.end());

    hash.for_each([](const std::pair<const char*, int>& item) -> bool {
        qDebug() << QString("<%1, %2>").arg(item.first).arg(item.second);
        return true;
    });

    for(Size i = 0; i < hash.bucket_count(); ++i) {
        std::pair<const char*const , int>* pPair = hash.probe(i);
        if(pPair) {
            qDebug() << QString("[%1]: <%2, %3>").arg(i).arg(pPair->first).arg(pPair->second);
        }
    }

    for(auto it = hash.next(nullptr); it != hash.end(); it = hash.next(&it)) {
        qDebug() << QString("Next.K= %1, Next.V = %2").arg(it->first).arg(it->second);
    }

    for(auto it = hash.begin(); it != hash.end(); ++it) {
        qDebug() << QString("It->first = %1, It->second = %2").arg(it->first).arg(it->second);
    }

    hash.clear();
    QCOMPARE(hash.size(), 0);
    QVERIFY(hash.empty());
}


inline void HashMap::benchmarkReadStd(void) {
    std::unordered_map<const char*, int> stdhash;

    for(unsigned i = 0; i < GBL_TEST_HASH_MAP_BENCHMARK_READ_ENTRIES; ++i)
        stdhash[Q_CSTR(elysian::generateRandomString(GBL_TEST_HASH_MAP_BENCHMARK_KEY_SIZE))] = 33;

    QBENCHMARK {
        auto val = stdhash[Q_CSTR(elysian::generateRandomString(GBL_TEST_HASH_MAP_BENCHMARK_KEY_SIZE))];
        GBL_UNUSED(val);
    }
}

inline void HashMap::benchmarkReadQt(void) {
    QHash<const char*, int> qhash;

    for(unsigned i = 0; i < GBL_TEST_HASH_MAP_BENCHMARK_READ_ENTRIES; ++i)
        qhash[Q_CSTR(elysian::generateRandomString(GBL_TEST_HASH_MAP_BENCHMARK_KEY_SIZE))] = 33;

    QBENCHMARK {
        auto val = qhash[Q_CSTR(elysian::generateRandomString(GBL_TEST_HASH_MAP_BENCHMARK_KEY_SIZE))];
        GBL_UNUSED(val);
    }
}

inline void HashMap::benchmarkReadGimbal(void) {
    gimbal::HashMap<const char*, int> hash(pCtx());
    for(unsigned i = 0; i < GBL_TEST_HASH_MAP_BENCHMARK_READ_ENTRIES; ++i)
        hash[Q_CSTR(elysian::generateRandomString(GBL_TEST_HASH_MAP_BENCHMARK_KEY_SIZE))] = 33;

    QBENCHMARK {
        auto val = hash[Q_CSTR(elysian::generateRandomString(GBL_TEST_HASH_MAP_BENCHMARK_KEY_SIZE))];
        GBL_UNUSED(val);
    }
}

inline void HashMap::benchmarkWriteStd(void) {
    std::unordered_map<const char*, int> stdhash;
    QBENCHMARK {
        stdhash[Q_CSTR(elysian::generateRandomString(GBL_TEST_HASH_MAP_BENCHMARK_KEY_SIZE))] = 33;
    }
}

inline void HashMap::benchmarkWriteQt(void) {
    QHash<const char*, int> qhash;

    QBENCHMARK {
        qhash[Q_CSTR(elysian::generateRandomString(GBL_TEST_HASH_MAP_BENCHMARK_KEY_SIZE))] = 33;
    }
}

inline void HashMap::benchmarkWriteGimbal(void) {
    gimbal::HashMap<const char*, int> hash(pCtx());
    QBENCHMARK {
        hash[Q_CSTR(elysian::generateRandomString(GBL_TEST_HASH_MAP_BENCHMARK_KEY_SIZE))] = 33;
    }
}



}



#endif // TEST_GIMBAL_HASH_HPP
