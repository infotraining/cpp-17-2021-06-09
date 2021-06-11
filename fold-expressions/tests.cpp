#include <algorithm>
#include <numeric>
#include <iostream>
#include <string>
#include <vector>

#include "catch.hpp"

using namespace std;

namespace Cpp98
{
    void fold_98()
    {
        std::vector<int> vec = {1, 2, 3, 4, 5};

        auto sum = std::accumulate(std::begin(vec), std::end(vec), 0);
        std::cout << "sum: " << sum << "\n";

        auto result = std::accumulate(std::begin(vec), std::end(vec), "0"s,
            [](const std::string& reduced, int item) {
                return "("s + reduced + " + "s + std::to_string(item) + ")"s;
            });

        std::cout << result << "\n";
    }
}

TEST_CASE("fold expressions")
{
    Cpp98::fold_98();
}

namespace BeforeCpp17
{
    template <typename T>
    auto sum(const T& last)
    {
        return last;
    }

    template <typename THead, typename... TTail>
    auto sum(const THead& head, const TTail&... tail)
    {
        return head + sum(tail...);
    }
}

template <typename... TArgs>
auto sum(const TArgs&... args)  // (1, 2, 3, 4)
{
    return (... + args); // left fold - ((((1 + 2) + 3) + 4) +5 )
}

template <typename... TArgs>
auto sum_r(const TArgs&... args)  // (1, 2, 3, 4)
{
    return (args + ...); // right fold - (1 + (2 + (3 + (4 + 5 ))))
}

template <typename... TArgs>
void print(const TArgs&... args)
{
    bool is_first = true;

    auto with_space = [&is_first](const auto& arg) {
        if (!is_first)
            std::cout << " ";
            is_first = false;
        return arg;
    };

    (std::cout << ... << with_space(args)) << "\n";
}

template <typename... TArgs>
void print_lines(const TArgs&... args)
{
    (..., (std::cout << args << "\n"));    
}

TEST_CASE("fold + variadic templates")
{
    REQUIRE(sum(1, 2, 3, 4) == 10);

    print(1, 3.14, "text", "abc"s);

    print_lines(1, 3.14, "text", "abc"s);
}