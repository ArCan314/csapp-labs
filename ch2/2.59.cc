int func(int x, int y)
{
    return (x & 0x0000'00ff) | (y & 0xffff'ff00);
}

#include <cassert>
#include <random>
#include "util.h"

int main()
{
    std::mt19937 eng(0);
    std::uniform_int_distribution<unsigned> dist(0, 0xffffffff);

    for (int i = 0; i < 10; i++)
    {
        int x = dist(eng), y = dist(eng);
        int res = func(x, y);
        show_bytes(reinterpret_cast<unsigned char *>(&x), sizeof(decltype(x)));
        show_bytes(reinterpret_cast<unsigned char *>(&y), sizeof(decltype(y)));
        show_bytes(reinterpret_cast<unsigned char *>(&res), sizeof(decltype(res)));
        std::printf("\n");
    }

    return 0;
}