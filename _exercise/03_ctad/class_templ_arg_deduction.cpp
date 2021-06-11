#include <algorithm>
#include <iostream>
#include <numeric>
#include <string>
#include <vector>
#include <array>

#include "catch.hpp"

using namespace std::literals;

template <typename T>
struct Range
{
    T low, high;
};

template <typename T1, typename T2>
Range(T1,T2) -> Range<std::common_type_t<T1, T2>>;

TEST_CASE("CTAD for Range")
{
    Range r1{4, 5};
    static_assert(std::is_same_v<decltype(r1), Range<int>>);

    Range r2{3.14, 6.28};
    static_assert(std::is_same_v<decltype(r2), Range<double>>);

    SECTION("extra")
    {
        Range r3{1, 3.14};
        static_assert(std::is_same_v<decltype(r3), Range<double>>);
    }
}

////////////////////////////////////////////

template <typename T> 
struct Wrapper
{
    T value;

    template <typename TArg>
    explicit Wrapper(TArg&& arg) : value(std::forward<TArg>(arg))
    {}
};

template <typename T>
Wrapper(T) -> Wrapper<T>;

Wrapper (const char*) -> Wrapper<std::string>;

TEST_CASE("CTAD for Wrapper")
{
    int s = 10;

    Wrapper w1{s};
    static_assert(std::is_same_v<decltype(w1), Wrapper<int>>);

    Wrapper w2{std::move(s)};
    static_assert(std::is_same_v<decltype(w2), Wrapper<int>>);

    Wrapper w3{"abc"};
    static_assert(std::is_same_v<decltype(w3), Wrapper<std::string>>);
    REQUIRE(w3.value == "abc"s);
}

///////////////////////////////////////

template <typename T, size_t N>
struct Array
{
    T items[N];
};

// template<typename... T>
// Array(T...) -> Array<std::common_type_t<T...>, sizeof...(T)>;

template <typename T, typename... TRest>
Array(T, TRest...) -> Array<std::enable_if_t<(... && std::is_same_v<T, TRest>), T>, sizeof...(TRest) + 1>;


TEST_CASE("CTAD for Array")
{
    Array arr1{1, 2, 3};
    static_assert(std::is_same_v<decltype(arr1), Array<int, 3>>);

    Array arr2{"abc", "def", "ghi", "klm"};
    static_assert(std::is_same_v<decltype(arr2), Array<const char*, 4>>);

    SECTION("extra")
    {
        Array arr3{1.0, 2.3, 3.1, 4.0, 5.0}; // it should be an error - all items on the list should have the same type
    }

    std::array arr4{1, 2, 3}; // std::array<int, 3>
    //std::array arr5{1, 4, 6, 7L};
}

TEST_CASE("vector + CTAD")
{
    std::vector vec = {1, 2, 3}; // vector<int>

    std::vector other_vec{vec}; // vector<int>

    std::vector another_vec{vec, vec}; // vector<vector<int>>
}

//////////////////////////////
// auto as template parameter

template <auto N>
struct Value
{
    constexpr static auto value = N;
};

TEST_CASE("auto as template parameter")
{
    static_assert(Value<1024>::value == 1024);
    static_assert(Value<1024L>::value == 1024L);
    
    std::cout << Value<'b'>::value << std::endl;
}