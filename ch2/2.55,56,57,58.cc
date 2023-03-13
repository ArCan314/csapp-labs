#include <cstdio>
#include <cstdint>
#include <iostream>

#include "util.h"

// 2.57 see util.h

// 2.58
bool is_little_endian()
{
    int a = 1;
    if (reinterpret_cast<unsigned char *>(&a)[0] == 0x01)
        return true;
    return false;
}

int main()
{
    // 2.55, 2.56
    show_bytes(0x12345678);
    show_bytes(2.0f);
    show_bytes(0x12345678l);
    show_bytes(123456.123456);
    show_bytes(short(0x1234));

    // 2.58
    std::cout << "is little endian: " << (is_little_endian() ? "true" : "false") << std::endl;

    return 0;
}