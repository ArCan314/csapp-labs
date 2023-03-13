#include <random>
#include <cassert>

constexpr const int kIntMax = 0x7fffffff;
constexpr const int kIntMin = -kIntMax - 1;

int main()
{
    std::mt19937 eng(0);
    std::uniform_int_distribution<int> dist(0x8000'0000, 0x7fff'ffff);

    int x = dist(eng);
    int y = dist(eng);

    unsigned ux = x;
    unsigned uy = y;

    // A: Fail
    // static_assert((kIntMin > 0) || (kIntMin - 1 < 0));

    // B: True
    assert((x & 7) != 7 || (x << 29 < 0));

    // C: False
    // static_assert(46341 * 46341 >= 0); // oxb505

    // D: True
    assert(x < 0 || -x <= 0);

    // E: False
    // static_assert((kIntMin > 0) || (-kIntMin >= 0));

    // F: True
    assert(x + y == uy + ux);

    // G: True
    assert(x * ~y - ux * uy == -x);

    return 0;
}

// x * (-y - 1) + uy * ux == -x
// -xy - x + uy * ux == -x