#pragma once
#include <cstdio>
#include <cinttypes>

void show_bytes(unsigned char *start, size_t len)
{
    int i;

    for (i = len - 1; i >= 0; i--)
        std::printf(" %.2X", start[i]);
    std::printf("\n");
}

// 2.57
template <typename T>
void show_bytes(T *start)
{
    show_bytes(reinterpret_cast<unsigned char *>(start), sizeof(T));
}

template <typename T>
void show_bytes(T start)
{
    show_bytes((&start));
}

template <typename FromType, typename ToType>
ToType byte_cast(FromType x)
{
    static_assert(sizeof(FromType) == sizeof(ToType));
    return *reinterpret_cast<ToType*>(&x);
}

unsigned f2u(float x)
{
    return byte_cast<float, unsigned>(x);
}