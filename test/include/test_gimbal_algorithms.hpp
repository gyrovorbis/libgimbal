#ifndef TEST_GIMBAL_ALGORITHMS_H
#define TEST_GIMBAL_ALGORITHMS_H


#include <gimbal/algorithms/gimbal_numeric.h>
#include <gimbal/algorithms/gimbal_sort.h>
#include <gimbal/core/gimbal_api_frame.hpp>
#include <elysian_qtest.hpp>
#include <concepts>
#include <limits>
#include <gimbal/algorithms/gimbal_hash.h>

#define TEST_GIMBAL_ALGORITHMS_STRING_COUNT     10000
#define TEST_GIMBAL_ALGORITHMS_STRING_SIZE_MIN  10
#define TEST_GIMBAL_ALGORITHMS_STRING_SIZE_MAX  300
#define TEST_GIMBAL_ALGORITHMS_STRING_CHARS     (QString("abcdefghijklmnopqrstuvwxyz"))
#define TEST_GIMBAL_ALGORITHMS_SORT_COUNT       900

namespace gimbal::test {


//error strings?
class Algorithms: public elysian::UnitTestSet {
Q_OBJECT
public:
private:
    std::array<int, TEST_GIMBAL_ALGORITHMS_SORT_COUNT> randomArray_;
    QStringList randomStrings_;

    void verifyPow2NextTyped_(uint8_t base, uint64_t expected) {
        const decltype(base) pow2 = gblPow2Next_u8(base);
        QCOMPARE(pow2, expected);
    }

    void verifyPow2NextTyped_(uint16_t base, uint64_t expected) {
        const decltype(base) pow2 = gblPow2Next_u16(base);
        QCOMPARE(pow2, expected);
    }

    void verifyPow2NextTyped_(uint32_t base, uint64_t expected) {
        const decltype(base) pow2 = gblPow2Next_u32(base);
        QCOMPARE(pow2, expected);
    }

    void verifyPow2NextTyped_(uint64_t base, uint64_t expected) {
        const decltype(base) pow2 = gblPow2Next_u64(base);
        QCOMPARE(pow2, expected);
    }

    template<typename T>
    void verifyPow2NextGeneric_(T base, uint64_t expected) {
        if(expected <= std::numeric_limits<uint8_t>::max())
            verifyPow2NextTyped_(static_cast<uint8_t>(base), expected);
        if(expected <= std::numeric_limits<uint16_t>::max())
            verifyPow2NextTyped_(static_cast<uint16_t>(base), expected);
        if(expected <= std::numeric_limits<uint32_t>::max())
            verifyPow2NextTyped_(static_cast<uint32_t>(base), expected);
        if(expected <= std::numeric_limits<uint64_t>::max())
            verifyPow2NextTyped_(static_cast<uint64_t>(base), expected);
    }

    template<typename T>
    void verifyPow2Next_(T base, uint64_t expected) {
        if(base > 1) verifyPow2NextGeneric_(base-1, base);
        verifyPow2NextGeneric_(base, base);
        verifyPow2NextGeneric_(base+1, expected);
    }

    GblInt gcdRecursive_(GblInt a, GblInt b) {
        // Everything divides 0
        if (a == 0)
           return b;
        if (b == 0)
           return a;

        // base case
        if (a == b)
            return a;

        // a is greater
        if (a > b)
            return gcdRecursive_(a-b, b);
        return gcdRecursive_(a, b-a);
    }

    void verifySort_(GblSortFn pFnSorter) {
        std::array<int, 900> entries;
        QBENCHMARK {
            entries = randomArray_;
            pFnSorter(entries.data(), entries.size(), sizeof(int), [](const void* pA, const void* pB) {
                const int* pIA = static_cast<const int*>(pA);
                const int* pIB = static_cast<const int*>(pB);
                return *pIA - *pIB;
            });
        }

        for(unsigned i = 0; i < entries.size()-1; ++i) {
            QVERIFY(entries[i] <= entries[i+1]);
        }
    }

    void verifyHash_(std::invocable<const void*, GblSize> auto pFnHasher) {
        QHash<GblHash, QString> hashCache;

        for(auto& str : randomStrings_) {
            const auto hash = std::invoke(pFnHasher, Q_CSTR(str), str.length());

            if(auto it = hashCache.find(hash); it != hashCache.end()) {
                qWarning() << QString("Hash conflict %1 [%2, %3]").arg(hash)
                                                                  .arg(it.value())
                                                                  .arg(str);
            }
            hashCache[hash] = str;
        }

        qDebug() << QString("Hash conflicts: %1").arg(randomStrings_.size() - hashCache.size());
    }


private slots:
    void initTestCase(void);
    void seed(void);
    void random(void);
    void randomRange(void);
    void hashSip(void);
    void hashMurmur(void);
    void hashJenkins(void);
    void hashCrc(void);
    void hashXX(void);
    void pow2Next(void);
    void gcd(void);
    void ntohl_htonl(void);
    void sortQuick(void);
    void sortSelection(void);
    void sortInsertion(void);
    void sortShell(void);
    void sortBubble(void);
    void searchBinary(void);
};


inline void Algorithms::initTestCase(void) {
    for(unsigned i = 0; i < randomArray_.size(); ++i)
        randomArray_[i] = elysian::qrand();

    while(randomStrings_.size() < TEST_GIMBAL_ALGORITHMS_STRING_COUNT) {
        QString next = elysian::generateRandomString(-1,
                                                     TEST_GIMBAL_ALGORITHMS_STRING_SIZE_MAX,
                                                     TEST_GIMBAL_ALGORITHMS_STRING_SIZE_MIN,
                                                     TEST_GIMBAL_ALGORITHMS_STRING_CHARS);
        if(randomStrings_.contains(next)) {
            continue;
        }
        //qDebug() << QString("Random string: [%1]").arg(next);
        randomStrings_.push_back(std::move(next));
    }
    QCOMPARE(randomStrings_.size(), TEST_GIMBAL_ALGORITHMS_STRING_COUNT);
}

inline void Algorithms::seed(void) {
    std::array<uint64_t, GBL_SEED_COUNT> seeds = { 0 };
    // grab seeds
    for(unsigned s = 0; s < GBL_SEED_COUNT; ++s) {
        seeds[s] = gblSeed(s);
        qDebug() << "Seed[" << s << "]: " << seeds[s];
    }

    // verify no dupes
    for(unsigned s = 0; s < GBL_SEED_COUNT; ++s) {
        for(unsigned v = s+1; v < GBL_SEED_COUNT; ++v) {
            QVERIFY(seeds[s] != seeds[v]);
        }
    }

    // verify they are consistent
    std::array<uint64_t, GBL_SEED_COUNT> seeds2 = { 0 };
    for(unsigned s = 0; s < GBL_SEED_COUNT; ++s) {
        seeds2[s] = gblSeed(s);
        QCOMPARE(seeds[s], seeds2[s]);
    }
}

inline void Algorithms::random(void) {
    QList<int> gblnumbers;
    QList<int> qtnumbers;
    int tests = 0;

    auto testRand = [&](auto&& f, QList<int>& list) {
        auto num = f();

        if(!list.contains(num))
            list.push_back(num);

    };

    auto testFunc = [&](const char* pName, auto&& f, QList<int>& list) {
        list.clear();
        tests = 0;
        for(unsigned i = 0; i < 9999; ++i) {
            testRand(f, list);
            ++tests;
        }
        qDebug() << QString("%1 had %2 collisions after %3 runs.").arg(pName).arg(tests - list.size()).arg(tests);
    };

        testFunc("qtRand", elysian::qrand, qtnumbers);
        testFunc("gblRand", gblRand, gblnumbers);

}

inline void Algorithms::randomRange(void) {
    auto testRand = [&](int min, int max) {
        int number = gblRandRange(min, max);
        QVERIFY(number >= min);
        QVERIFY(number <= max);
    };

    for(unsigned i = -756; i < 9999; ++i) {
        for(unsigned j = i; j < 9999; ++j) {
            testRand(i, j);
        }
    }
}

inline void Algorithms::hashSip(void) {
    verifyHash_(gblHashSip);
}

inline void Algorithms::hashMurmur(void) {
    verifyHash_(gblHashMurmur);
}

inline void Algorithms::hashJenkins(void) {
    verifyHash_(gblHashJenkins);
}

inline void Algorithms::hashCrc(void) {
    verifyHash_(gblHashCrc);
}

inline void Algorithms::hashXX(void) {
    QSKIP("Unimplemented.");
 //   verifyHash_(gblHashXX);
}


inline void Algorithms::pow2Next(void) {
    for(uint64_t b = 2; b < 63; ++b) {
        const uint64_t value = (static_cast<uint64_t>(1u) << b);
        verifyPow2Next_(value, value << 1u);
    }
}

inline void Algorithms::gcd(void) {
    for(GblInt u = 1; u < 999; ++u) {
        for(GblInt v = 1; v < 999; ++v) {
            QCOMPARE(gblGcd(u, v), gcdRecursive_(u, v));
        }
    }
}

inline void Algorithms::ntohl_htonl(void) {
    for(uint32_t i = 0; i < 10000; ++i) {
        uint32_t v = (uint32_t)rand();
        QCOMPARE(gblntohl(gblhtonl(v)), v);
    }
}

inline void Algorithms::sortQuick(void) {
     verifySort_(gblSortQuick);
}
inline void Algorithms::sortSelection(void) {
    verifySort_(gblSortSelection);
}
inline void Algorithms::sortInsertion(void) {
    verifySort_(gblSortInsertion);
}
inline void Algorithms::sortShell(void) {
    verifySort_(gblSortShell);
}
inline void Algorithms::sortBubble(void) {
    verifySort_(gblSortBubble);
}

inline void Algorithms::searchBinary(void) {
    std::array<int, 900> entries;

    for(unsigned i = 0; i < entries.size(); ++i) {
        entries[i] = i;
    }


    QBENCHMARK {
        int i = rand() % entries.size();

        GblInt result = gblSearchBinary(entries.data(), sizeof(int), 0, entries.size(), &i,
                [](const void* pA, const void* pB) {
                                const int* pIA = static_cast<const int*>(pA);
                                const int* pIB = static_cast<const int*>(pB);
                                return *pIA - *pIB;
                            });
            QVERIFY(result != -1);
            QCOMPARE(entries[result], i);
    }

    int invalidInt = -400;
    GblInt result = gblSearchBinary(entries.data(), sizeof(int), 0, entries.size(), &invalidInt,
        [](const void* pA, const void* pB) {
                        const int* pIA = static_cast<const int*>(pA);
                        const int* pIB = static_cast<const int*>(pB);
                        return *pIA - *pIB;
                    });
    QVERIFY(result == -1);
}



}

#endif // TEST_GIMBAL_ALGORITHMS_H
