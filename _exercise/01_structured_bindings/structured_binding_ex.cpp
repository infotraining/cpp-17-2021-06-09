#include <iostream>
#include <string>
#include <tuple>

#include "catch.hpp"

using namespace std;

enum class Bitfields : uint32_t
{
};

template<>
struct std::tuple_size<Bitfields>
{
    static unsigned int const value = 4;
};

template<size_t Index>
struct std::tuple_element<Index, Bitfields>
{
    using type = uint8_t;
};

template <size_t Index>
decltype(auto) get(const Bitfields& arg)
{
    return static_cast<uint8_t>(static_cast<uint32_t>(arg) >> ( (3 - Index) * 8 ) );
}

TEST_CASE("split Bitfields to bytes")
{
    Bitfields value{0b00000001'11100010'00000100'01001000};

    const auto [b1, b2, b3, b4] = value;

    CHECK(b1 == 0b00000001);
    CHECK(b2 == 0b11100010);
    CHECK(b3 == 0b00000100);
    CHECK(b4 == 0b01001000);
}