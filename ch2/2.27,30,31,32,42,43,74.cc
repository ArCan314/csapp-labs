#include <cassert>
#include <random>
#include <iostream>
#include <algorithm>
#include <utility>
#include <limits>

// 2.27
bool uadd_ok(unsigned x, unsigned y)
{
    return !(x > x + y || y > x + y);
}

bool uadd_validate(unsigned x, unsigned y)
{
    unsigned long ulx = x, uly = y;
    return ulx + uly < 0x1'0000'0000ul;
}

// 2.30
bool tadd_ok(int x, int y)
{
    return !((x + y < 0 && x > 0 && y > 0) || (x + y > 0 && x < 0 && y < 0));
}

bool tadd_validate(int x, int y)
{
    if (x <= 0x7fff'ffffu && y <= 0x7fff'ffffu)
        return static_cast<bool>(!((x + y) & 0x8000'0000u));
    else if (x > 0x7fff'ffffu && y > 0x7fff'ffffu)
        return static_cast<bool>((x + y) & 0x8000'0000u);
    return true;
}

// 2.31
bool tadd_ok_buggy(int x, int y) // always returns true
{
    int sum = x + y;
    return (sum - x == y) && (sum - y == x);
}

// 2.32
bool tsub_ok_buggy(int x, int y) // buggy when x < 0 and y = 0x8000'0000
{
    return tadd_ok(x, -y);
}

bool tsub_ok(int x, int y)
{
    return tadd_ok(x, -y) || (y == 0x8000'0000 && x < 0);
}

// 2.36
bool tmul_ok(int32_t x, int32_t y)
{
    int64_t lx = x, ly = y;
    if (lx * ly > 0)
        return !((lx * ly) & 0xffff'ffff'8000'0000);
    else if (lx * ly < 0)
        return ((lx * ly) & 0xffff'ffff'8000'0000) == 0xffff'ffff'8000'0000;
    return true;
}

bool tmul_validate(int32_t x, int32_t y)
{
    int p = x * y;
    return !x || p / x == y;
}

// 2.42
int div16(int x)
{
    int bias = (x >> 31) & 0xf;
    return (x + bias) >> 4;
}

int div16_validate(int x)
{
    return x / 16;
}

// 2.43
constexpr int arith(int x, int y)
{
    int result = 0;
    result = x * 31 + y / 8;
    return result;
}

constexpr int optarith(int x, int y)
{
    int t = x;
    x <<= 5;
    x -= t;
    if (y < 0)
        y += 7;
    y >>= 3;
    return x + y;
}

// test
int main()
{
    std::mt19937 eng(0);
    std::uniform_int_distribution<unsigned> dist(0, 0xffffffff);
    std::pair<unsigned, unsigned> a_stat{0xffffffff, 0}, b_stat{0xffffffff, 0};

#pragma omp parallel for num_threads(8) schedule(dynamic, 10000000)
    for (int i = 0; i < 0xffffffff; i++)
    {
        unsigned a = dist(eng), b = dist(eng);

        a_stat.first = std::min(a_stat.first, a);
        a_stat.second = std::max(a, a_stat.second);
        b_stat.first = std::min(b_stat.first, b);
        b_stat.second = std::max(b_stat.second, b);
        // if (tmul_ok(a, b) != tmul_validate(a, b))
        // {
        //     std::cout << int(a) << ", " << int(b) << ", " << long(a * b) << std::endl;
        //     std::cout << tmul_ok(a, b) << ", " << tmul_validate(a, b) << std::endl;
        // }

        // 2.27 test
        assert(uadd_ok(a, b) == uadd_validate(a, b));

        // 2.30 test
        assert(tadd_ok(a, b) == tadd_validate(a, b));

        // 2.36 test
        assert(tmul_ok(a, b) == tmul_validate(a, b));

        // 2.42 test
        assert(div16(a) == div16_validate(a));
    }
    // std::cout << "stat_a_range = { " << a_stat.first << ", " << a_stat.second << "}" << std::endl;
    // std::cout << "stat_b_range = { " << b_stat.first << ", " << b_stat.second << "}" << std::endl;

    // 2.31 test
    assert(tsub_ok_buggy(-1000, std::numeric_limits<int>::min()) == false);
    assert(tsub_ok(-1000, std::numeric_limits<int>::min()) == true);

    // 2.43 test
    static_assert(arith(100, 100) == optarith(100, 100));
    static_assert(arith(100, -100) == optarith(100, -100));
    static_assert(arith(100, -873264) == optarith(100, -873264));
    static_assert(arith(100, -87324354) == optarith(100, -87324354));
    static_assert(arith(100, -9) == optarith(100, -9));

    std::cout << "2.27,30,31,32,42,43,74 TEST COMPLETE" << std::endl;
    return 0;
}