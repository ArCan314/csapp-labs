#include <cstdlib>
#include <cstring>
#include <climits>
#include <cassert>
#include <cmath>

#include <random>
#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <bitset>
#include <mutex>

#include "util.h"

// 2.61
bool func_2_61_A(int x)
{
    return !(x ^ -1);
}

bool func_2_61_B(int x)
{
    return !(x & -1);
}

bool func_2_61_C(int x)
{
    return !((x & 0xff) ^ 0xff);
}

bool func_2_61_D(int x)
{
    int width = sizeof(x) << 3;
    unsigned mask = 0xff << (width - 8);
    return !((x & mask) & (-1 & mask));
}

// 2.62
constexpr int int_shifts_are_arithmetic()
{
    constexpr int width = sizeof(int) << 3;
    constexpr int num = (1 << (width - 1)) >> (width - 1);
    return (num & 0x2) >> 1;
}

// 2.63
unsigned srl(unsigned x, int k)
{
    unsigned xsra = (int)x >> k;

    constexpr int width = sizeof(int) * 8;
    unsigned mask = -1 << (width - k - 1) << 1; // if k == 0: (-1 << 32) == (-1 << (32 % 32)) == (-1 << 0)
    return xsra & ~mask;
}

int sra(int x, int k)
{
    int xsrl = (unsigned)x >> k;
    int width = sizeof(int) * 8;
    int mask = -1 << (width - k - 1) << 1;
    int neg_sign = ~(-1u >> 1) & x;
    (neg_sign && (xsrl |= mask)); // simulate if
    return xsrl;
}

// 2.64
int any_odd_one(unsigned x)
{
    return (bool)(x & 0x55555555);
}

// 2.65
int odd_ones(unsigned x)
{
    x ^= x >> 16;
    x ^= x >> 8;
    x ^= x >> 4;
    x ^= x >> 2;
    x ^= x >> 1;
    return x & 1;
}

// 2.66
int leftmost_ones(unsigned x)
{
    x |= x >> 1;
    x |= x >> 2;
    x |= x >> 4;
    x |= x >> 8;
    x |= x >> 16;
    return (x >> 1) + (x && 1);
}

// 2.67
constexpr int int_size_is_32() // least 16
{
    constexpr unsigned set_msb = 1 << 15 << 15 << 1;
    constexpr unsigned beyond_msb = set_msb << 1;

    return set_msb && !beyond_msb;
}

// 2.68
constexpr int lower_one_mask(int n)
{
    unsigned mask = -1u << (n - 1) << 1;
    return ~mask;
}

// 2.69
unsigned rotate_left(const unsigned x, const int n)
{
    int width = sizeof(unsigned) << 3;
    unsigned hi = x << n, lo = ((~(-1u >> n)) & x) >> (width - n);
    return hi | lo;
}

// 2.70
int fits_bits(int x, int n)
{
    int width = sizeof(int) << 3;
    int offset = width - n;
    return (x << offset >> offset) == x;
}

int fits_bits_compare(int x, int n)
{
    int max = (1 << (n - 1)) - 1;
    int min = -max - 1;
    return x >= min && x <= max;
}

// 2.71
// TODO
typedef unsigned packet_t;
constexpr int xbyte(packet_t word, int bytenum)
{
    return (int)(char)(word >> (bytenum << 3));
    // return (int)word << ((3 - bytenum) << 3) >> 24;
}

// 2.72
void copy_int(int val, void *buf, int maxbytes)
{
    if (maxbytes >= (int)sizeof(val))
        memcpy(buf, (void *)&val, sizeof(val));
}

// 2.73
int saturating_add(int x, int y)
{
    int sum = x + y;
    int width = sizeof(int) << 3;
    int pos_overflow_flag = !(x & INT_MIN) && !(y & INT_MIN) && (sum & INT_MIN);
    int neg_overflow_flag = (x & INT_MIN) && (y & INT_MIN) && !(sum & INT_MIN);
    (pos_overflow_flag && (sum = INT_MAX)) || (neg_overflow_flag && (sum = INT_MIN));
    return sum;
}

// 2.74 refer to 2.74.cc

// 2.75
unsigned unsigned_high_prod(unsigned x, unsigned y)
{
    // TODO
}

// 2.76
void *calloc(size_t nmemb, size_t size)
{
    if (nmemb == 0 || size == 0)
        return NULL;

    size_t mul = nmemb * size;
    if (mul / nmemb == size)
    {
        void *res = malloc(nmemb * size);
        if (res != NULL)
        {
            memset(res, 0, nmemb * size);
            return res;
        }
    }
    return NULL;
}

// 2.77
int func_2_77_A(int x)
{
    return (x << 4) + x;
}

int func_2_77_B(int x)
{
    return x - (x << 3);
}

int func_2_77_C(int x)
{
    return (x << 6) - (x << 2);
}

int func_2_77_D(int x)
{
    return (x << 4) - (x << 7);
}

// 2.78
int divide_power2(int x, int k)
{
    int bias = (1 << k) - 1;
    int neg_flag = x & INT_MIN;
    (neg_flag && (x += bias));
    return x >> k;
}

// 2.79
int mul3div4(int x)
{
    int bias = 3;
    constexpr int width = sizeof(int) << 3;
    x = (x << 1) + x;
    int neg_flag = (1 << (width - 1)) & x;
    (neg_flag && (x += bias));
    return x >> 2;
}

// 2.80
int threeforths(int x)
{
    // FAULT
    // int temp = x;
    // ((INT_MIN & x) && (x += 3)); // bias = 3

    // int quarter = x >> 2;
    // return temp - quarter;

    // TRUE
    int neg_flag = x & INT_MIN;
    int high_30 = x & ~0x3;
    int low_2 = x & 0x3;

    high_30 >>= 2;
    high_30 = (high_30 << 1) + high_30;

    low_2 = (low_2 << 1) + low_2;
    (neg_flag && (low_2 += 3));
    low_2 >>= 2;
    return high_30 + low_2;
}

// 2.81
int func_2_81_A(int k)
{
    int is_not_zero = (k != 0);
    return (-1 << is_not_zero << (k - is_not_zero));
}

int func_2_81_B(int j, int k)
{
    int is_not_zero = (j != 0);
    return ~func_2_81_A(k) << (j - is_not_zero) << is_not_zero;
}

// 2.82
void func_2_82()
{
    // A. (x < y) == (-x > -y): FALSE
    // static_assert((INT_MIN < -1) != (-INT_MIN > -(-1)));

    // B. ((x+y) << 4) + y - x == 17*y + 15*x: TRUE

    // C. ~x + ~y + 1 == ~(x + y): TRUE

    // D. (ux - uy) == -(unsigned)(y - x): TRUE

    // E. ((x >> 2) << 2) <= x: TRUE
}

// 2.83
// Y/(2^k) / (1 - 1/(2^k))
// (a). (5/8) / (7/8) == 5/7
// (b). (3/8) / (15/16) == 2/5
// (c). (19/64) / (63/64) == 19/63

// 2.84
// return ((ux << 1) == 0 && (uy << 1) == 0) ||
//        ((sx && sy) && (ux >= uy)) ||
//        ((sx && !sy)) ||
//        ((!sx && !sy) && (ux <= uy));

// 2.89
void func_2_89()
{
    // A. (float)x == (float)dx: TRUE

    // B. dx - dy == (double)(x - y): FALSE

    {
        constexpr int x = 0x7fff'ffff, y = -1;
        constexpr double dx = double(x), dy = double(y);
        // static_assert((dx - dy) != (double)(x - y));
    }

    // C. (dx + dy) + dz == dx + (dy + dz): TRUE

    // D. (dx * dy) * dz == dx * (dy * dz): TRUE

    // E. dx/dx == dz/dz: FALSE
    // static_assert(0.0/0.0 == 0.0/0.0);
}

// 2.90
float u2f(unsigned u)
{
    return *(float *)&u;
}

float fpwr2(int x)
{
    unsigned exp, frac;
    unsigned u;
    int bias = 127;

    if (x < -149)
    {
        exp = 0;
        frac = 0;
    }
    else if (x < -126)
    {
        exp = 0;
        frac = 1 << (149 + x);
    }
    else if (x < 128)
    {
        exp = x + 127;
        frac = 0;
    }
    else
    {
        exp = 255;
        frac = 0;
    }

    u = exp << 23 | frac;
    return u2f(u);
}

typedef unsigned float_bits; // must be unsigned type
float_bits extract_sign(float_bits f)
{
    return f >> 31;
}

float_bits extract_exp(float_bits f)
{
    return (f >> 23) & 0xff;
}

float_bits extract_frac(float_bits f)
{
    return f & 0x7f'ffff;
}

// 2.92
float_bits float_negate(float_bits f)
{
    float_bits exp = extract_exp(f);
    float_bits frac = extract_frac(f);
    float_bits sign = extract_sign(f);

    if (exp == 255 && frac) // Nan
        return f;
    return f ^ 0x8000'0000;
}

float_bits combine_float_bits(float_bits sign, float_bits exp, float_bits frac)
{
    return (sign << 31) | (exp << 23) | frac;
}

// 2.93
float_bits float_absval(float_bits f)
{
    float_bits exp = extract_exp(f);
    float_bits frac = extract_frac(f);
    float_bits sign = extract_sign(f);

    if (exp == 255 && frac)
        return f;
    return f & 0x7fff'ffff;
}

// 2.94
float_bits float_twice(float_bits f)
{
    float_bits exp = extract_exp(f);
    float_bits frac = extract_frac(f);
    float_bits sign = extract_sign(f);

    if (exp == 255) // f == Nan || f == inf || f == -inf
        ;
    else if (exp > 0) // normalized
    {
        exp++;
        if (exp == 255)
            frac = 0;
    }
    else // denormalized
    {
        frac <<= 1;
        if (frac & 0xff80'0000)
        {
            frac &= 0x7f'ffff;
            exp++;
        }
    }

    return combine_float_bits(sign, exp, frac);
}

// 2.95
float_bits float_half(float_bits f)
{
    float_bits exp = extract_exp(f);
    float_bits frac = extract_frac(f);
    float_bits sign = extract_sign(f);
    float_bits lowest_bit = frac & 1;

    if (exp == 255) // f == Nan || f == inf || f == -inf
        ;
    else if (exp > 1) // normalized
        exp--;
    else if (exp == 1)
    {
        exp--;
        frac = (frac >> 1) + 0x40'0000;
        if (lowest_bit && (frac & 1))
            frac++;
        if (frac & 0x80'0000)
        {
            exp++;
            frac = 0;
        }
    }
    else // denormalized
    {
        frac >>= 1;
        if (lowest_bit && (frac & 1))
            frac++;
    }

    return combine_float_bits(sign, exp, frac);
}

// 2.96
int float_f2i(float_bits f)
{
    float_bits exp = extract_exp(f);
    float_bits frac = extract_frac(f);
    float_bits sign = extract_sign(f);
    int result;

    if (exp == 255)
        result = 0x8000'0000;
    else if (exp < 127) // |f| < 1
        result = 0;
    else if (!sign && exp >= 158) // positive overflow
        result = 0x8000'0000;
    else if (sign && (exp > 158 || (exp == 158 && frac > 0))) // negative overflow
        result = 0x8000'0000;
    else
    {
        int e = exp - 127;
        result = 0x80'0000 | frac;
        if (e > 23)
            result <<= e - 23;
        else
            result >>= 23 - e;

        if (sign)
            result = -result;
    }

    return result;
}

// 2.97
float_bits float_i2f(int i)
{
    int width = sizeof(int) << 3;
    float_bits sign = i < 0;
    float_bits frac, e;

    if (i == INT_MIN)
    {
        frac = 0;
        e = width - 1;
    }
    else if (i == 0)
    {
        frac = 0;
        e = -127;
    }
    else
    {
        if (sign)
            i = -i;
        int highest_one = 1;

        // find index of the highest one
        {
            unsigned temp = i;
            while (temp >>= 1)
                highest_one++;
        }
        // std::cout << highest_one << std::endl;

        frac = i;
        if (highest_one > 24)
        {
            // show_bytes(frac);

            unsigned shift_bits = highest_one - 24;
            float_bits removed_bits = (~(-1 << shift_bits)) & frac;
            unsigned removed_bits_max_half = 1 << (shift_bits - 1);

            // std::cout << shift_bits << std::endl;
            // show_bytes(removed_bits);
            // show_bytes(removed_bits_max_half);

            frac >>= shift_bits;

            // show_bytes(frac);

            if (removed_bits > removed_bits_max_half)
                frac++;
            else if ((removed_bits == removed_bits_max_half) && (frac & 1))
                frac++;

            // show_bytes(frac);
            e = highest_one - 1;
            // std::cout << e << std::endl;
            if (frac & 0x100'0000)
            {
                unsigned add = (frac & 0x3) == 0x3;
                e++;
                frac >>= 1;
                frac += add;
            }
            frac &= 0x7f'ffff;
            // std::cout << e << std::endl;
            // show_bytes(frac);
        }
        else if (highest_one == 24)
        {
            e = 23;
            frac &= 0x7f'ffff;
        }
        else
        {
            frac <<= (24 - highest_one);
            e = highest_one - 1;
            frac &= 0x7f'ffff;
        }
    }
    return combine_float_bits(sign, e + 127, frac);
}

// int max = 0 1 11 1111 1111 1111 1111 1111 1111 1111
// float  =    1.11 1111 1111 1111 1111 1111 1000 0000 * 2^30

int main()
{
    // 2.63 test
    if constexpr (true)
    {
        std::uniform_int_distribution<int> dist_k(0, sizeof(int) * 3 - 1);
        std::mt19937 eng(0);
        std::uniform_int_distribution<unsigned> dist(0, 0xffff'ffff);
        for (int i = 0; i < 1000'0000; i++)
        {
            unsigned x = dist(eng), k = dist_k(eng);
            if (srl(x, k) != x >> k)
            {
                std::cout << "2.63: srl(x,k) FAIL: x = " << x << ", k = " << k << std::endl;
                show_bytes(x);
                show_bytes(srl(x, k));
                show_bytes(x >> k);
                break;
            }

            if (sra((int)x, k) != ((int)x) >> k)
            {
                std::cout << "2.63: sra(x,k) FAIL: x = " << (int)x << ", k = " << k << std::endl;
                show_bytes(x);
                show_bytes(sra(x, k));
                show_bytes(x >> k);
                break;
            }
        }
        std::cout << "2.63 TEST END" << std::endl;
    }

    // 2.64 test
    if constexpr (true)
    {
        int mask = 1, count = 1;
        while (mask)
        {
            if (any_odd_one(mask) != count % 2)
            {
                std::cout << "2.64: any_odd_one(x) FAIL: x = " << mask << ", any_odd_one(x) = " << any_odd_one(mask) << std::endl;
                show_bytes(mask);
                break;
            }
            mask <<= 1;
            count++;
        }
        std::cout << "2.64 TEST END" << std::endl;
    }

    // 2.67 test
    {
        static_assert(int_size_is_32());
    }

    // 2.68 test
    if constexpr (true)
    {
        std::bitset<32> temp;
        for (int i = 1; i <= 32; i++)
        {
            temp[i - 1] = true;
            if (lower_one_mask(i) != (int)temp.to_ulong())
            {
                std::cout << "2.68: lower_one_mask(i) FAIL: i = " << i << ", lower_one_mask(i) = " << lower_one_mask(i) << std::endl;
                show_bytes(lower_one_mask(i));
                show_bytes((int)temp.to_ulong());
                break;
            }
        }
        std::cout << "2.68 TEST END" << std::endl;
    }

    // 2.69 test
    if constexpr (true)
    {
        assert(rotate_left(0x12345678, 4) == 0x23456781);
        assert(rotate_left(0x12345678, 20) == 0x67812345);
        std::bitset<32> raw(0x12345678);
        for (int i = 0; i < 32; i++)
        {
            std::bitset<64> temp = raw.to_ulong();
            temp <<= i;
            for (int j = 0; j < i; j++)
                temp[j] = temp[j + 32];

            if (rotate_left(0x12345678, i) != (int)temp.to_ulong())
            {
                std::cout << "2.69: rotate_left(0x12345678, i) FAIL: i = " << i << ", rotate_left(0x12345678, i) = " << rotate_left(0x12345678, i) << std::endl;
                show_bytes(rotate_left(0x12345678, i));
                show_bytes((int)temp.to_ulong());
                break;
            }
        }
        std::cout << "2.69 TEST END" << std::endl;
    }

    // 2.70 test
    if constexpr (true)
    {
        for (int i = 0; i < 0x00ff'ffff; i++)
        {
            if (fits_bits(i, 8) != fits_bits_compare(i, 8))
            {
                std::cout << "2.70: fits_bits(i, 8) FAIL: i = " << i << ", fits_bits(i, 8) = " << fits_bits(i, 8) << std::endl;
                show_bytes(i);
                show_bytes(fits_bits(i, 8));
                show_bytes(fits_bits_compare(i, 8));
                break;
            }
        }
        std::cout << "2.70 TEST END" << std::endl;
    }

    // 2.71
    {
        constexpr int test_num = 0xfeba'8743;
        static_assert(xbyte(test_num, 0) == 0x43);
        static_assert(xbyte(test_num, 1) == 0xffff'ff87);
        static_assert(xbyte(test_num, 2) == 0xffff'ffba);
        static_assert(xbyte(test_num, 3) == 0xffff'fffe);
    }

    // 2.73
    if constexpr (true)
    {
        assert(saturating_add(INT_MAX, 1) == INT_MAX);
        assert(saturating_add(INT_MAX, 0x4321) == INT_MAX);
        assert(saturating_add(INT_MIN, -1) == INT_MIN);
        assert(saturating_add(INT_MIN, -0x4321) == INT_MIN);
        assert(saturating_add(1234, 4321) == 5555);
        std::cout << "2.73 TEST END" << std::endl;
    }

    // std::cout << std::atan(1574.3 / 500.0) * (2.0 / std::acos(-1)) * 100.0 << std::endl;

    // 2.78
    if constexpr (true)
    {
        std::mt19937 eng(0);
        std::uniform_int_distribution<unsigned> dist(0, UINT_MAX), dist_k(0, (sizeof(int) << 3) - 1);
        for (int i = 0; i < 100'0000; i++)
        {
            int x = dist(eng), k = dist_k(eng);
            if (divide_power2(x, k) != x / (1 << k))
            {
                std::cout << "2.78: divide_power2(x, k) FAIL: x = " << x << ", k = " << k << ", divide_power2(x, k) = " << divide_power2(x, k) << ", x / (1<<k) = " << x / (1 << k) << std::endl;
                show_bytes(divide_power2(x, k));
                show_bytes(x / (1 << k));
                break;
            }
        }
        std::cout << "2.78 TEST END" << std::endl;
    }

    // 2.79
    if constexpr (true)
    {
        std::mt19937 eng(0);
        std::uniform_int_distribution<unsigned> dist(0, UINT_MAX);
        for (int i = 0; i < 100'0000; i++)
        {
            int x = dist(eng);
            if (mul3div4(x) != 3 * x / 4)
            {
                std::cout << "2.79: mul3div4(x) FAIL: x = " << x << ", mul3div4(x) = " << mul3div4(x) << ", 3*x / 4 = " << 3 * x / 4 << std::endl;
                show_bytes(mul3div4(x));
                show_bytes(3 * x / 4);
                break;
            }
        }
        std::cout << "2.79 TEST END" << std::endl;
    }

    // 2.80
    if constexpr (true)
    {
        std::mt19937 eng(0);
        std::uniform_int_distribution<unsigned> dist(0, UINT_MAX);
        for (int i = 0; i < 100'0000; i++)
        {
            int x = dist(eng);
            // int x = -i;
            if (threeforths(x) != (long)x * 3 / 4)
            {
                std::cout << "2.80: threeforths(x) FAIL: x = " << x << ", threeforths(x) = " << threeforths(x) << ", (long)x * 3 / 4 = " << (long)x * 3 / 4 << std::endl;
                show_bytes(threeforths(x));
                show_bytes((long)x * 3 / 4);
                break;
            }
        }
        std::cout << "2.80 TEST END" << std::endl;
    }

    // 2.81
    if constexpr (true)
    {
        for (int k = 0; k <= 32; k++)
            for (int j = 0; j + k <= 32; j++)
            {
                std::bitset<32> compare;
                compare.flip();
                for (int i = 0; i < k; i++)
                    compare[i] = 0;

                if (func_2_81_A(k) != (int)compare.to_ulong())
                {
                    std::cout << "2.81.A: func_2_81_A(k) FAIL: k = " << k << ", func_2_81_A(k) = " << func_2_81_A(k) << ", (int)compare.to_ulong() = " << (int)compare.to_ulong() << std::endl;
                    show_bytes(func_2_81_A(k));
                    show_bytes((int)compare.to_ulong());
                    break;
                }

                compare.reset();
                for (int i = j; i < k + j; i++)
                    compare[i] = 1;
                if (func_2_81_B(j, k) != (int)compare.to_ulong())
                {
                    std::cout << "2.81.B: func_2_81_B(j, k) FAIL: j = " << j << ", k = " << k << ", func_2_81_B(j, k) = " << func_2_81_B(j, k) << ", (int)compare.to_ulong() = " << (int)compare.to_ulong() << std::endl;
                    show_bytes(func_2_81_B(j, k));
                    show_bytes((int)compare.to_ulong());
                    break;
                }
            }
        std::cout << "2.81 TEST END" << std::endl;
    }

    // 2.90
    if constexpr (true)
    {
        for (int i = -255; i <= 255; i++)
        {
            if (fpwr2(i) != std::pow(2.0f, (float)i))
            {
                std::cout << "2.90: fpwr2(x) FAIL: x = " << i << ", fpwr2(x) = " << fpwr2(i) << ", std::pow(2.0f, (float)i) = " << std::pow(2.0f, (float)i) << std::endl;
                show_bytes(fpwr2(i));
                show_bytes(std::pow(2.0f, (float)i));
                break;
            }
        }
        std::cout << "2.90 TEST END" << std::endl;
    }

    // 2.92
    if constexpr (true)
    {
        std::mutex mtx;
#pragma omp parallel for num_threads(8) schedule(dynamic, 1000'0000)
        for (unsigned long i = 0; i <= UINT32_MAX; i++)
        {
            if (u2f(float_negate(i)) != -u2f((unsigned)i))
            {
                if (std::isnan(u2f(float_negate(i))) && std::isnan(-u2f((unsigned)i)))
                    continue;

                std::lock_guard mtx_guard(mtx);

                std::cout << "2.92: u2f(float_negate(i)) FAIL: f = " << i << ", u2f(float_negate(i)) = " << u2f(float_negate(i)) << ", -u2f((unsigned)i) = " << -u2f((unsigned)i) << std::endl;
                show_bytes(u2f(float_negate(i)));
                show_bytes(-u2f((unsigned)i));
                // break;
            }
        }
        std::cout << "2.92 TEST END" << std::endl;
    }

    // 2.93
    if constexpr (true)
    {
        std::mutex mtx;
#pragma omp parallel for num_threads(8) schedule(dynamic, 1000'0000)
        for (unsigned long i = 0; i <= UINT32_MAX; i++)
        {
            if (u2f(float_absval(i)) != std::abs(u2f((unsigned)i)))
            {
                if (std::isnan(u2f(float_absval(i))) && std::isnan(std::abs(u2f((unsigned)i))))
                    continue;

                std::lock_guard mtx_guard(mtx);

                std::cout << "2.93: float_absval(i) FAIL: f = " << i << ", u2f(float_absval(i)) = " << u2f(float_absval(i)) << ", std::abs(u2f((unsigned)i)) = " << std::abs(u2f((unsigned)i)) << std::endl;
                show_bytes(u2f(float_absval(i)));
                show_bytes(std::abs(u2f((unsigned)i)));
                // break;
            }
        }
        std::cout << "2.93 TEST END" << std::endl;
    }

    // 2.94
    if constexpr (true)
    {
        std::mutex mtx;
#pragma omp parallel for num_threads(8) schedule(dynamic, 1000'0000)
        for (unsigned long i = 0; i <= UINT32_MAX; i++)
        {
            if (u2f(float_twice(i)) != 2.f * u2f((unsigned)i))
            {
                if (std::isnan(u2f(float_twice(i))) && std::isnan(2.f * u2f((unsigned)i)))
                    continue;

                std::lock_guard mtx_guard(mtx);

                std::cout << "2.94: float_twice(i) FAIL: f = " << i << ", u2f(float_twice(i)) = " << u2f(float_twice(i)) << ", 2.f * u2f((unsigned)i) = " << 2.f * u2f((unsigned)i) << std::endl;
                show_bytes(u2f(float_twice(i)));
                show_bytes(2.f * u2f((unsigned)i));
                // break;
            }
        }
        std::cout << "2.94 TEST END" << std::endl;
    }

    // 2.95
    if constexpr (true)
    {
        std::mutex mtx;
// bool is_end = false;
#pragma omp parallel for num_threads(8) schedule(dynamic, 1000'0000)
        for (unsigned long i = 0; i <= UINT32_MAX; i++)
        {
            if (u2f(float_half(i)) != u2f((unsigned)i) / 2.0f)
            {
                if (std::isnan(u2f(float_half(i))) && std::isnan(u2f((unsigned)i) / 2.0f))
                    continue;

                std::lock_guard mtx_guard(mtx);

                std::cout << "2.95: float_half(i) FAIL: f = " << i << ", u2f(float_half(i)) = " << u2f(float_half(i)) << ", u2f((unsigned)i) / 2.0f = " << u2f((unsigned)i) / 2.0f << std::endl;
                show_bytes(extract_sign(unsigned(i)));
                show_bytes(extract_exp(unsigned(i)));
                show_bytes(extract_frac(unsigned(i)));
                show_bytes(u2f((unsigned)i));
                show_bytes(u2f(float_half(i)));
                show_bytes(u2f((unsigned)i) / 2.0f);
                // break;
            }
        }
        std::cout << "2.95 TEST END" << std::endl;
    }

    // 2.96
    if constexpr (true)
    {
        std::mutex mtx;
// bool is_end = false;
#pragma omp parallel for num_threads(8) schedule(dynamic, 1000'0000)
        for (unsigned long i = 0; i <= UINT32_MAX; i++)
        {
            if (float_f2i(i) != (int)u2f((unsigned)i))
            {
                std::lock_guard mtx_guard(mtx);

                std::cout << "2.96: float_f2i(i) FAIL: f = " << i << ", float_f2i(i) = " << float_f2i(i) << ", (int)u2f((unsigned)i) = " << (int)u2f((unsigned)i) << std::endl;
                show_bytes(extract_sign(unsigned(i)));
                show_bytes(extract_exp(unsigned(i)));
                show_bytes(extract_frac(unsigned(i)));
                show_bytes(u2f((unsigned)i));
                show_bytes(float_f2i(i));
                show_bytes((int)u2f((unsigned)i));
                // break;
            }
        }
        std::cout << "2.96 TEST END" << std::endl;
    }

    // 2.97
    if constexpr (true)
    {
        std::mutex mtx;
#pragma omp parallel for num_threads(8) schedule(dynamic, 1000'0000)
        for (int64_t i = INT32_MIN; i <= INT32_MAX; i++)
        {
            float temp = u2f(float_i2f((int)i));
            if (temp != (float)(int)i)
            {
                std::lock_guard mtx_guard(mtx);

                std::cout << "2.97: float_i2f(i) FAIL: f = " << i << ", u2f(float_i2f((int)i)) = " << temp << ", (float)(int)i = " << (float)(int)i << std::endl;
                show_bytes((int)i);
                show_bytes(extract_sign(f2u(temp)));
                show_bytes(extract_exp(f2u(temp)));
                show_bytes(extract_frac(f2u(temp)));
                show_bytes(temp);
                show_bytes(extract_sign(f2u((float)(int)i)));
                show_bytes(extract_exp(f2u((float)(int)i)));
                show_bytes(extract_frac(f2u((float)(int)i)));
                show_bytes((float)(int)i);
                // break;
            }
        }
        std::cout << "2.97 TEST END" << std::endl;
    }
    return 0;
}
