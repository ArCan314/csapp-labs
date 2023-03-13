int bis(int x, int m)
{
    int mask = 0x1;
    for (int i = 0; i < 32; i++)
    {
        if (m & mask)
            x |= mask;
        mask <<= 1;
    }

    return x;
}

int bic(int x, int m)
{
    int mask = 0x1;
    for (int i = 0; i < 32; i++)
    {
        if (m & mask)
            x &= ~mask;
        mask <<= 1;
    }

    return x;
}

int bool_or(int x, int y)
{
    int result = bis(x, y);
    return result;
}

int bool_xor(int x, int y)
{
    int result = bis(bic(x, y), bic(y, x));
    return result;
}

#include <cassert>
#include <iostream>
int main()
{
    #pragma omp parallel for num_threads(8) schedule(dynamic, 150)
    for (int i = 0; i < 0xffff; i++)
    {
        for (int j = i; j < 0xffff; j++)
        {
            assert(bool_or(i, j) == (i | j));
            assert(bool_xor(i, j) == (i ^ j));
        }
    }
    std::cout << "2.13 TEST COMPLETE" << std::endl;
    return 0;
}