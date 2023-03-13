unsigned replace_byte(unsigned x, int i, unsigned char b)
{
    unsigned mask = 0xff00'0000;
    mask >>= 8 * i;
    return x & (~mask) | (static_cast<unsigned>(b) << (8 * (sizeof(x) / 8 - i - 1)));
}

#include "util.h"
#include <random>

int main()
{
    std::mt19937 eng(0);
    std::uniform_int_distribution<unsigned> dist(0, 0xffff'ffff), replace_index(0, 3), replace_content(0, 0xff);

    for (int i = 0; i < 5; i++)
    {
        unsigned x = dist(eng), ind = replace_index(eng), b = replace_content(eng);
        show_bytes(x);
        show_bytes(ind);
        show_bytes(b);
        show_bytes(replace_byte(x, ind, b));
        std::printf("\n");
    }

    return 0;
}