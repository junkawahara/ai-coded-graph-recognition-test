#include "util/bigint.h"

#include <gtest/gtest.h>

#include <cstdlib>
#include <string>
#include <vector>

using graph_recognition::BigInt;

namespace {

std::string i128_to_string(__int128 v) {
    if (v == 0) return "0";
    bool neg = v < 0;
    unsigned __int128 u = neg ? (unsigned __int128)0 - (unsigned __int128)v : (unsigned __int128)v;
    std::string s;
    while (u) {
        s.insert(s.begin(), (char)('0' + (int)(u % 10)));
        u /= 10;
    }
    return neg ? "-" + s : s;
}

long long random_ll(int bits) {
    unsigned long long u = 0;
    for (int i = 0; i < 4; ++i) u = (u << 16) ^ (unsigned long long)(std::rand() & 0xFFFF);
    if (bits < 64) u &= ((unsigned long long)1 << bits) - 1;
    long long v = (long long)u;
    return (std::rand() & 1) ? -v : v;
}

TEST(BigIntTest, ConstructionAndConversion) {
    EXPECT_EQ(BigInt().to_string(), "0");
    EXPECT_EQ(BigInt(0).sign(), 0);
    EXPECT_EQ(BigInt(-1).to_string(), "-1");
    EXPECT_EQ(BigInt(1LL << 40).to_string(), "1099511627776");
    EXPECT_EQ(BigInt(-9223372036854775807LL - 1).to_string(), "-9223372036854775808");
    EXPECT_EQ(BigInt(-9223372036854775807LL - 1).to_long_long(), -9223372036854775807LL - 1);
    EXPECT_EQ(BigInt(9223372036854775807LL).to_long_long(), 9223372036854775807LL);
    EXPECT_THROW((BigInt(9223372036854775807LL) + BigInt(1)).to_long_long(), std::overflow_error);
}

// Every operation against __int128 on random 60-bit operands (products stay
// within 120 bits).
TEST(BigIntTest, MatchesInt128OnRandomOperands) {
    std::srand(20260923);
    for (int trial = 0; trial < 20000; ++trial) {
        long long a = random_ll(1 + std::rand() % 60), b = random_ll(1 + std::rand() % 60);
        BigInt A(a), B(b);
        EXPECT_EQ((A + B).to_string(), i128_to_string((__int128)a + b));
        EXPECT_EQ((A - B).to_string(), i128_to_string((__int128)a - b));
        EXPECT_EQ((A * B).to_string(), i128_to_string((__int128)a * b));
        int cmp = a < b ? -1 : (a > b ? 1 : 0);
        EXPECT_EQ(BigInt::compare(A, B), cmp);
        if (b != 0) {
            EXPECT_EQ((A / B).to_string(), i128_to_string((__int128)a / b));
            EXPECT_EQ((A % B).to_string(), i128_to_string((__int128)a % b));
        }
    }
}

// Multi-limb division: (a * b + r) / b == a with remainder r for random
// multi-limb a, b and 0 <= r < |b|, in every sign combination.
TEST(BigIntTest, MultiLimbDivisionInvertsMultiplication) {
    std::srand(4321);
    for (int trial = 0; trial < 3000; ++trial) {
        BigInt a(1), b(1);
        int la = 1 + std::rand() % 6, lb = 1 + std::rand() % 4;
        for (int i = 0; i < la; ++i) a = a * BigInt(random_ll(62)) + BigInt(random_ll(20));
        for (int i = 0; i < lb; ++i) b = b * BigInt(random_ll(62)) + BigInt(random_ll(20));
        if (b.is_zero()) continue;
        BigInt absb = b.sign() < 0 ? -b : b;
        BigInt r = BigInt(random_ll(62));
        r = r % absb;
        if (r.sign() < 0) r = -r;
        // remainder takes the sign of the dividend
        BigInt prod = a * b;
        BigInt x = prod.sign() < 0 ? prod - r : prod + r;
        if (x.is_zero()) continue;
        BigInt q, rem;
        BigInt::divmod(x, b, q, rem);
        EXPECT_EQ(q, a) << "trial=" << trial;
        EXPECT_EQ(rem.sign() < 0 ? -rem : rem, r) << "trial=" << trial;
        BigInt back = q * b + rem;
        EXPECT_EQ(back, x) << "trial=" << trial;
        BigInt exact;
        EXPECT_EQ(BigInt::div_exact(prod, b, exact), true);
        EXPECT_EQ(exact, a);
        if (!r.is_zero()) { EXPECT_FALSE(BigInt::div_exact(x, b, exact)); }
    }
}

// Knuth D edge case: the trial quotient overshoots and the add-back step runs.
// Dividends of the form (B^k - 1) by divisors with a high top limb exercise it.
TEST(BigIntTest, DivisionAddBackCases) {
    BigInt base32(1LL << 32);
    BigInt big(1);
    for (int i = 0; i < 6; ++i) big = big * base32;
    big = big - BigInt(1);  // 2^192 - 1
    std::vector<BigInt> divisors;
    divisors.push_back(base32 * base32 - BigInt(1));                          // 2^64 - 1
    divisors.push_back(base32 * base32 * BigInt(0x80000000LL) - BigInt(1));    // 2^95 - 1
    divisors.push_back(base32 * BigInt(0x80000000LL) + BigInt(0x7FFFFFFFLL)); // 2^63 + 2^31 - 1
    divisors.push_back(base32 * base32 * base32 - base32);                    // 2^96 - 2^32
    for (size_t i = 0; i < divisors.size(); ++i) {
        BigInt q, r;
        BigInt::divmod(big, divisors[i], q, r);
        EXPECT_EQ(q * divisors[i] + r, big) << "divisor " << i;
        EXPECT_TRUE(r.sign() >= 0 && r < divisors[i]) << "divisor " << i;
    }
}

TEST(BigIntTest, FactorialFifty) {
    BigInt f(1);
    for (int i = 2; i <= 50; ++i) f *= BigInt(i);
    EXPECT_EQ(f.to_string(), "30414093201713378043612608166064768844377641568960512000000000000");
    BigInt g(f);
    for (int i = 50; i >= 2; --i) {
        BigInt q;
        ASSERT_TRUE(BigInt::div_exact(g, BigInt(i), q));
        g = q;
    }
    EXPECT_EQ(g, BigInt(1));
    EXPECT_THROW(f / BigInt(0), std::domain_error);
}

}  // namespace
