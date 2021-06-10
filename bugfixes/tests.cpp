#include <algorithm>
#include <numeric>
#include <iostream>
#include <string>
#include <vector>

#include "catch.hpp"

using namespace std;

enum class Coffee : uint8_t 
{
    espresso, chemex, drip
};

TEST_CASE("enums init")
{
    Coffee c1 = Coffee::espresso;
    Coffee c2{2};

    REQUIRE(c2 == Coffee::drip);
}

TEST_CASE("bytes in C++")
{
    uint8_t byte = 65;

    byte += 42;

    std::cout << "byte: " << byte << std::endl;
}

namespace Explain
{
    enum class byte : uint8_t
    {
    };
}

TEST_CASE("byte in C++17")
{
    std::byte b1{65};
    std::byte b2{0b01010111};

    std::byte result = ~((b1 & b2) << 2);

    std::cout << "result: " << std::to_integer<uint32_t>(result) << "\n";
}

TEST_CASE("auto + {}")
{
    int x1 = 10;
    int x2(10);
    int x3{10};

    auto a1 = 10; // int
    auto a2(10);  // int
    auto a3{10};  // int
    //auto a3_ill_formed{1, 2, 3};
    auto a4 = {10}; // std::initializer_list<int>
}

void foo1()
{}

void foo2() noexcept
{}

template <typename F1, typename F2>
void call_wrapper(F1 f1, F2 f2)
{
    f1();
    f2();
}

TEST_CASE("noexcept")
{
    static_assert(!std::is_same_v<decltype(foo1), decltype(foo2)>);

    call_wrapper(foo1, foo2);

    void (*ptr_fun)() noexcept;
    // ptr_fun = foo1; // ERROR
    ptr_fun = foo2;
}

template <typename T>
void swap(T& a, T& b) noexcept(noexcept(a.swap(b)))
{
    a.swap(b);
}

struct CopyMoveDisabled
{
    vector<int> data;

    CopyMoveDisabled(std::initializer_list<int> lst)
        : data {lst}
    {
    }

    CopyMoveDisabled(const CopyMoveDisabled&) = delete;
    CopyMoveDisabled& operator=(const CopyMoveDisabled&) = delete;
    CopyMoveDisabled(CopyMoveDisabled&&) = delete;
    CopyMoveDisabled& operator=(CopyMoveDisabled&&) = delete;
    ~CopyMoveDisabled() = default;
};

CopyMoveDisabled create_impossible()
{
    return CopyMoveDisabled{1, 2, 3, 4};  // rvo
}

// CopyMoveDisabled still_impossible()
// {
//     CopyMoveDisabled cmd{1, 2, 3};
//     return cmd; // nrvo
// }

void use_impossible(CopyMoveDisabled arg)
{
    std::cout << "Using object: ";
    for(const auto& item : arg.data)
        std::cout << item << " ";
    std::cout << "\n";
}

TEST_CASE("RVO & Copy elision")
{
    CopyMoveDisabled cmd = create_impossible();

    CopyMoveDisabled&& ref_cmd = create_impossible();

    use_impossible(create_impossible()); // rvo
}