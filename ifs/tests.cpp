#include <algorithm>
#include <iostream>
#include <map>
#include <mutex>
#include <numeric>
#include <queue>
#include <string>
#include <thread>
#include <vector>

#include "catch.hpp"

using namespace std;

TEST_CASE("if with initializers")
{
    vector vec = {1, 2, 6346, 23, 645, 42, 3564, 43};

    if (auto pos = std::find(begin(vec), end(vec), 42); pos != end(vec))
    {
        std::cout << "Found " << *pos << "\n";
    }
    else
    {
        std::cout << "Item not found...\n";
        assert(pos == end(vec));
    }
}

TEST_CASE("if with initializer & structure bindings")
{
    std::map<int, std::string> dict = {{1, "one"}, {2, "two"}};

    if (auto [pos, was_inserted] = dict.emplace(2, "two"); was_inserted)
    {
        const auto& [key, value] = *pos;
        std::cout << "Item: " << key << " - " << value << " was inserted...\n";
    }
    else
    {
        const auto& [key, value] = *pos;
        std::cout << "Item: " << key << " - " << value << " was already in map...\n";
    }
}

TEST_CASE("if with mutex")
{
    std::queue<std::string> q_msg;
    std::mutex mtx_q_msg;

    SECTION("thread#1")
    {
        std::lock_guard lk {mtx_q_msg}; // CTAD
        q_msg.push("START");
    }

    SECTION("thread#2")
    {
        std::string msg;

        if (std::lock_guard lk {mtx_q_msg}; !std::empty(q_msg))
        {
            msg = q_msg.front();
            q_msg.pop();
        }

        std::cout << msg << "\n";
    }
}

namespace BeforeCpp17
{
    template <typename T>
    auto is_power_of_2(T value) -> std::enable_if_t<std::is_integral<T>::value, bool>
    {
        return value > 0 && (value & (value - 1)) == 0;
    }

    template <typename T>
    auto is_power_of_2(T value) -> std::enable_if_t<std::is_floating_point<T>::value, bool>
    {
        int exponent;
        const T mantissa = std::frexp(value, &exponent);
        return mantissa == static_cast<T>(0.5);
    }
}

namespace Cpp17
{
    template <typename T>
    auto is_power_of_2(T value)
    {
        if constexpr (std::is_integral_v<T>)
        {
            return value > 0 && (value & (value - 1)) == 0;
        }
        else
        {
            int exponent;
            const T mantissa = std::frexp(value, &exponent);
            return mantissa == static_cast<T>(0.5);
        }
    }
}

TEST_CASE("if constexpr")
{
    using namespace Cpp17;

    REQUIRE(is_power_of_2(2));
    REQUIRE(is_power_of_2(4));
    REQUIRE(is_power_of_2(8));
    REQUIRE(is_power_of_2(5) == false);

    REQUIRE(is_power_of_2(8.0));
    REQUIRE(is_power_of_2(64.0f));
}

namespace BeforeCpp17
{
    void print()
    {
        std::cout << "\n";
    }

    template <typename Head, typename... Tail>
    void print(const Head& head, const Tail&... tail)
    {
        std::cout << head << " ";
        print(tail...);
    }
}

namespace Cpp17
{
    template <typename Head, typename... Tail>
    void print(const Head& head, const Tail&... tail)
    {
        std::cout << head << " ";
        
        if constexpr(sizeof...(tail) > 0)
        {
            print(tail...);
        }
        else
        {
            std::cout << "\n";
        }
    }
}

TEST_CASE("variadic templates + constexpr if")
{
    using namespace Cpp17;

    print(1, 3.14, "test"s); 
        // print(1, tail...(3.14, "test"s)); // 1 ; print(3.14, "test"s)
            // print(3.14, tail...("test"s)); // 3.14; print("test"s);
              // print("test"s); // "test";
}

template <typename... Ts>
struct ListType
{
    static constexpr size_t size = sizeof...(Ts);
};

TEST_CASE("sizeof...")
{
    static_assert(ListType<int, double>::size == 2);
    static_assert(ListType<int, double, char>::size == 3);
    static_assert(ListType<>::size == 0);
}