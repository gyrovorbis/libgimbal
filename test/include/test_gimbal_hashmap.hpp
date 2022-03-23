#ifndef TEST_GIMBAL_HASH_HPP
#define TEST_GIMBAL_HASH_HPP


#include <gimbal/containers/gimbal_hashmap.hpp>
#include "test_gimbal.hpp"

namespace gimbal::test {


//error strings?
class HashMap: public UnitTestSet {
Q_OBJECT
private:

private slots:

    void init(void);
    void constructDefault(void);
    void constructInitializer(void);
};

inline void HashMap::init(void) {
    gblThreadStackFramePush(nullptr, nullptr);
}


inline void HashMap::constructDefault(void) {
    gimbal::HashMap<const char*, int> hash(pCtx());
    QCOMPARE(hash.size(), 0);
    QVERIFY(hash.bucket_size() >= sizeof(std::pair<const char*, int>));
    QCOMPARE(hash.context(), hCtx());
    QVERIFY(hash.empty());
}

inline void HashMap::constructInitializer(void) {
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

    hash.clear();
    QCOMPARE(hash.size(), 0);
    QVERIFY(hash.empty());
}



}



#endif // TEST_GIMBAL_HASH_HPP
