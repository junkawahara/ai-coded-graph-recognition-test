#ifndef GRAPH_RECOGNITION_BIGINT_H
#define GRAPH_RECOGNITION_BIGINT_H

/**
 * @file bigint.h
 * @brief Arbitrary-precision signed integers for exact (fraction-free) linear algebra
 *
 * A minimal big integer: sign plus little-endian base-2^32 magnitude. It
 * supports exactly what fraction-free Gaussian elimination (Bareiss) and
 * exact orientation tests need -- addition, subtraction, schoolbook
 * multiplication, truncating division with remainder (Knuth's Algorithm D),
 * comparison and a decimal rendering for diagnostics. There is no GCD,
 * no bit manipulation API and no fast multiplication; the intended operand
 * sizes are a few thousand bits.
 *
 * Used by the exact variants of Tutte's barycentric embedding
 * (decompositions/planar_embedding.h), where double precision underflows on
 * deeply nested inputs.
 */

#include <algorithm>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <vector>

namespace graph_recognition {

/**
 * @brief Arbitrary-precision signed integer
 *
 * Value = sign * sum(mag[i] * 2^(32 i)); mag has no leading zero limbs and
 * is empty exactly when the value is zero (sign 0).
 */
class BigInt {
public:
    BigInt() : sign_(0) {}

    /** @brief Conversion from a machine integer (LLONG_MIN included) */
    BigInt(long long v) : sign_(0) {  // NOLINT(google-explicit-constructor)
        if (v == 0) return;
        sign_ = v < 0 ? -1 : 1;
        uint64_t u = v < 0 ? (uint64_t)0 - (uint64_t)v : (uint64_t)v;
        while (u) {
            mag_.push_back((uint32_t)(u & 0xFFFFFFFFu));
            u >>= 32;
        }
    }

    /** @brief -1, 0 or 1 */
    int sign() const { return sign_; }
    bool is_zero() const { return sign_ == 0; }

    /** @brief Number of 32-bit limbs of the magnitude (0 for zero) */
    size_t limbs() const { return mag_.size(); }

    /** @brief Three-way comparison: negative, zero or positive as a <, ==, > b */
    static int compare(const BigInt& a, const BigInt& b) {
        if (a.sign_ != b.sign_) return a.sign_ < b.sign_ ? -1 : 1;
        int c = compare_mag(a.mag_, b.mag_);
        return a.sign_ >= 0 ? c : -c;
    }

    BigInt operator-() const {
        BigInt r(*this);
        r.sign_ = -r.sign_;
        return r;
    }

    friend BigInt operator+(const BigInt& a, const BigInt& b) {
        if (a.sign_ == 0) return b;
        if (b.sign_ == 0) return a;
        BigInt r;
        if (a.sign_ == b.sign_) {
            r.mag_ = add_mag(a.mag_, b.mag_);
            r.sign_ = a.sign_;
            return r;
        }
        int c = compare_mag(a.mag_, b.mag_);
        if (c == 0) return r;
        if (c > 0) {
            r.mag_ = sub_mag(a.mag_, b.mag_);
            r.sign_ = a.sign_;
        } else {
            r.mag_ = sub_mag(b.mag_, a.mag_);
            r.sign_ = b.sign_;
        }
        return r;
    }

    friend BigInt operator-(const BigInt& a, const BigInt& b) { return a + (-b); }

    friend BigInt operator*(const BigInt& a, const BigInt& b) {
        BigInt r;
        if (a.sign_ == 0 || b.sign_ == 0) return r;
        r.mag_ = mul_mag(a.mag_, b.mag_);
        r.sign_ = a.sign_ * b.sign_;
        return r;
    }

    BigInt& operator+=(const BigInt& b) { return *this = *this + b; }
    BigInt& operator-=(const BigInt& b) { return *this = *this - b; }
    BigInt& operator*=(const BigInt& b) { return *this = *this * b; }

    friend bool operator==(const BigInt& a, const BigInt& b) { return compare(a, b) == 0; }
    friend bool operator!=(const BigInt& a, const BigInt& b) { return compare(a, b) != 0; }
    friend bool operator<(const BigInt& a, const BigInt& b) { return compare(a, b) < 0; }
    friend bool operator>(const BigInt& a, const BigInt& b) { return compare(a, b) > 0; }
    friend bool operator<=(const BigInt& a, const BigInt& b) { return compare(a, b) <= 0; }
    friend bool operator>=(const BigInt& a, const BigInt& b) { return compare(a, b) >= 0; }

    /**
     * @brief Truncating division: a = q * b + r with |r| < |b| and sign(r) = sign(a) (or 0)
     * @throws std::domain_error on division by zero
     */
    static void divmod(const BigInt& a, const BigInt& b, BigInt& q, BigInt& r) {
        if (b.sign_ == 0) throw std::domain_error("BigInt: division by zero");
        q = BigInt();
        r = BigInt();
        if (a.sign_ == 0) return;
        if (compare_mag(a.mag_, b.mag_) < 0) {
            r = a;
            return;
        }
        divmod_mag(a.mag_, b.mag_, q.mag_, r.mag_);
        q.sign_ = q.mag_.empty() ? 0 : a.sign_ * b.sign_;
        r.sign_ = r.mag_.empty() ? 0 : a.sign_;
    }

    /** @brief Truncating quotient */
    friend BigInt operator/(const BigInt& a, const BigInt& b) {
        BigInt q, r;
        divmod(a, b, q, r);
        return q;
    }

    /** @brief Remainder of the truncating division (sign of the dividend) */
    friend BigInt operator%(const BigInt& a, const BigInt& b) {
        BigInt q, r;
        divmod(a, b, q, r);
        return r;
    }

    /**
     * @brief Exact quotient a / b
     * @return false (and q unspecified) when b does not divide a
     */
    static bool div_exact(const BigInt& a, const BigInt& b, BigInt& q) {
        BigInt r;
        divmod(a, b, q, r);
        return r.sign_ == 0;
    }

    /** @brief Decimal representation, with a leading '-' for negatives */
    std::string to_string() const {
        if (sign_ == 0) return "0";
        std::vector<uint32_t> cur(mag_);
        std::vector<uint32_t> chunks;  // base 10^9, least significant first
        while (!cur.empty()) {
            uint64_t rem = 0;
            for (size_t i = cur.size(); i-- > 0;) {
                uint64_t x = (rem << 32) | cur[i];
                cur[i] = (uint32_t)(x / 1000000000u);
                rem = x % 1000000000u;
            }
            chunks.push_back((uint32_t)rem);
            trim(cur);
        }
        std::string s = sign_ < 0 ? "-" : "";
        s += std::to_string((unsigned long long)chunks.back());
        for (size_t i = chunks.size() - 1; i-- > 0;) {
            std::string part = std::to_string((unsigned long long)chunks[i]);
            s += std::string(9 - part.size(), '0') + part;
        }
        return s;
    }

    /**
     * @brief Value as a long long
     * @throws std::overflow_error if it does not fit
     */
    long long to_long_long() const {
        if (sign_ == 0) return 0;
        if (mag_.size() > 2) throw std::overflow_error("BigInt: does not fit in long long");
        uint64_t u = mag_[0];
        if (mag_.size() == 2) u |= (uint64_t)mag_[1] << 32;
        if (sign_ > 0) {
            if (u > (uint64_t)9223372036854775807LL) throw std::overflow_error("BigInt: does not fit in long long");
            return (long long)u;
        }
        if (u > (uint64_t)9223372036854775807LL + 1) throw std::overflow_error("BigInt: does not fit in long long");
        if (u == (uint64_t)9223372036854775807LL + 1) return -9223372036854775807LL - 1;
        return -(long long)u;
    }

private:
    typedef std::vector<uint32_t> Mag;

    static void trim(Mag& m) {
        while (!m.empty() && m.back() == 0) m.pop_back();
    }

    static int compare_mag(const Mag& a, const Mag& b) {
        if (a.size() != b.size()) return a.size() < b.size() ? -1 : 1;
        for (size_t i = a.size(); i-- > 0;) {
            if (a[i] != b[i]) return a[i] < b[i] ? -1 : 1;
        }
        return 0;
    }

    static Mag add_mag(const Mag& a, const Mag& b) {
        const Mag& lo = a.size() < b.size() ? a : b;
        const Mag& hi = a.size() < b.size() ? b : a;
        Mag r(hi.size() + 1, 0);
        uint64_t carry = 0;
        for (size_t i = 0; i < hi.size(); ++i) {
            uint64_t s = (uint64_t)hi[i] + (i < lo.size() ? lo[i] : 0) + carry;
            r[i] = (uint32_t)s;
            carry = s >> 32;
        }
        r[hi.size()] = (uint32_t)carry;
        trim(r);
        return r;
    }

    /** a >= b required */
    static Mag sub_mag(const Mag& a, const Mag& b) {
        Mag r(a.size(), 0);
        int64_t borrow = 0;
        for (size_t i = 0; i < a.size(); ++i) {
            int64_t d = (int64_t)a[i] - (i < b.size() ? (int64_t)b[i] : 0) - borrow;
            if (d < 0) {
                d += (int64_t)1 << 32;
                borrow = 1;
            } else {
                borrow = 0;
            }
            r[i] = (uint32_t)d;
        }
        trim(r);
        return r;
    }

    static Mag mul_mag(const Mag& a, const Mag& b) {
        Mag r(a.size() + b.size(), 0);
        for (size_t i = 0; i < a.size(); ++i) {
            uint64_t carry = 0;
            for (size_t j = 0; j < b.size(); ++j) {
                uint64_t t = (uint64_t)a[i] * b[j] + r[i + j] + carry;
                r[i + j] = (uint32_t)t;
                carry = t >> 32;
            }
            size_t k = i + b.size();
            while (carry) {
                uint64_t t = (uint64_t)r[k] + carry;
                r[k] = (uint32_t)t;
                carry = t >> 32;
                ++k;
            }
        }
        trim(r);
        return r;
    }

    static int leading_zeros(uint32_t x) {
        int n = 0;
        while ((x & 0x80000000u) == 0) {
            x <<= 1;
            ++n;
        }
        return n;
    }

    /**
     * Knuth, TAOCP vol. 2, Algorithm D (after Hacker's Delight divmnu):
     * a >= b > 0 required; q and r receive the quotient and remainder.
     */
    static void divmod_mag(const Mag& a, const Mag& b, Mag& q, Mag& r) {
        const size_t m = a.size(), n = b.size();
        q.assign(m - n + 1, 0);
        r.clear();

        if (n == 1) {
            uint64_t rem = 0;
            for (size_t i = m; i-- > 0;) {
                uint64_t x = (rem << 32) | a[i];
                q[i] = (uint32_t)(x / b[0]);
                rem = x % b[0];
            }
            trim(q);
            if (rem) r.push_back((uint32_t)rem);
            return;
        }

        /* Normalize so that the top limb of the divisor has its high bit set. */
        const int s = leading_zeros(b[n - 1]);
        Mag vn(n), un(m + 1);
        for (size_t i = n - 1; i > 0; --i)
            vn[i] = (uint32_t)(((uint64_t)b[i] << s) | (s ? (uint64_t)b[i - 1] >> (32 - s) : 0));
        vn[0] = b[0] << s;
        un[m] = s ? a[m - 1] >> (32 - s) : 0;
        for (size_t i = m - 1; i > 0; --i)
            un[i] = (uint32_t)(((uint64_t)a[i] << s) | (s ? (uint64_t)a[i - 1] >> (32 - s) : 0));
        un[0] = a[0] << s;

        const uint64_t base = (uint64_t)1 << 32;
        for (size_t j = m - n + 1; j-- > 0;) {
            uint64_t num = ((uint64_t)un[j + n] << 32) | un[j + n - 1];
            uint64_t qhat = num / vn[n - 1];
            uint64_t rhat = num % vn[n - 1];
            while (qhat >= base || qhat * vn[n - 2] > ((rhat << 32) | un[j + n - 2])) {
                --qhat;
                rhat += vn[n - 1];
                if (rhat >= base) break;
            }

            /* Multiply and subtract. */
            int64_t borrow = 0;
            for (size_t i = 0; i < n; ++i) {
                uint64_t p = qhat * vn[i];
                int64_t t = (int64_t)un[i + j] - borrow - (int64_t)(p & 0xFFFFFFFFu);
                un[i + j] = (uint32_t)t;
                borrow = (int64_t)(p >> 32) - (t >> 32);
            }
            int64_t t = (int64_t)un[j + n] - borrow;
            un[j + n] = (uint32_t)t;

            q[j] = (uint32_t)qhat;
            if (t < 0) {
                /* qhat was one too large: add the divisor back. */
                --q[j];
                uint64_t carry = 0;
                for (size_t i = 0; i < n; ++i) {
                    uint64_t x = (uint64_t)un[i + j] + vn[i] + carry;
                    un[i + j] = (uint32_t)x;
                    carry = x >> 32;
                }
                un[j + n] = (uint32_t)((uint64_t)un[j + n] + carry);
            }
        }
        trim(q);

        /* Remainder: un[0..n-1] shifted back. */
        r.assign(n, 0);
        for (size_t i = 0; i < n; ++i)
            r[i] = (uint32_t)((un[i] >> s) | (s ? (uint64_t)un[i + 1] << (32 - s) : 0));
        trim(r);
    }

    int sign_;
    Mag mag_;
};

} // namespace graph_recognition

#endif
