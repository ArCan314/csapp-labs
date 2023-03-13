int fun1(unsigned word)
{
    return (int)((word << 24) >> 24);
}

int fun2(unsigned word)
{
    return ((int)word << 24) >> 24;
}

#include <cassert>
#include <cstdio>
#include <cstdint>
#include <cinttypes>

int main()
{
    assert(fun1(0x00000076) == 0x00000076);
    assert(fun2(0x00000076) == 0x00000076);
    assert(fun1(0x87654321) == 0x00000021);
    assert(fun2(0x87654321) == 0x00000021);
    assert(fun1(0x000000c9) == 0x000000c9);
    assert(fun2(0x000000c9) == 0xffffffc9);
    assert(fun1(0xedcba987) == 0x00000087);
    assert(fun2(0xedcba987) == 0xffffff87);
    int a = 54321;
    short b = a;
    std::printf("%" PRId32 "\n", a);
    std::printf("%" PRId16 "\n", b);
    std::printf("int: %X, short: %X\n", a, b);
    return 0;
}