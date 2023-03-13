#include <cassert>

int main()
{
    static_assert((-2147483647 - 1 == 2147483648u) == true);
    static_assert((-2147483647 - 1 < 2147483647) == true);
    static_assert((-2147483647 - 1u < 2147483647) == false);
    static_assert((-2147483647 - 1 < -2147483647) == true);
    static_assert((-2147483647 - 1u < -2147483647) == true);

    return 0;
}