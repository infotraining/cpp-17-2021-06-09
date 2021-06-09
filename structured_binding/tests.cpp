#include <algorithm>
#include <array>
#include <iostream>
#include <list>
#include <map>
#include <numeric>
#include <string>
#include <tuple>
#include <vector>

#include "catch.hpp"

std::tuple<int, int, double> calc_stats(const std::vector<int>& data)
{
    std::vector<int>::const_iterator min_pos, max_pos;
    std::tie(min_pos, max_pos) = std::minmax_element(data.begin(), data.end());

    double avg = accumulate(data.begin(), data.end(), 0.0) / data.size();

    return std::make_tuple(*min_pos, *max_pos, avg);
}

namespace Cpp17
{
    template <typename Container>
    std::tuple<int, int, double> calc_stats(const Container& data)
    {
        auto [min_pos, max_pos] = std::minmax_element(std::begin(data), std::end(data));

        double avg = std::accumulate(std::begin(data), std::end(data), 0.0) / std::size(data); // std::size() - new free function

        return std::tuple(*min_pos, *max_pos, avg); // CTAD - since C++17
    }
}

TEST_CASE("Before C++17")
{
    std::vector<int> data = {4, 42, 665, 1, 123, 13};

    int min, max;
    double avg;

    std::tie(min, max, avg) = calc_stats(data);

    REQUIRE(min == 1);
    REQUIRE(max == 665);
    REQUIRE(avg == Approx(141.333));
}

TEST_CASE("Since C++17")
{
    std::vector<int> data = {4, 42, 665, 1, 123, 13};

    auto [min, max, avg] = Cpp17::calc_stats(data);

    REQUIRE(min == 1);
    REQUIRE(max == 665);
    REQUIRE(avg == Approx(141.333));
}

TEST_CASE("Generalization of calcstats")
{
    SECTION("std::array")
    {
        std::array<int, 10> data = {1, 2, 3, 4, 5};

        auto [min, max, avg] = Cpp17::calc_stats(data);
    }

    SECTION("native array")
    {
        int data[] = {1, 2, 3, 4, 5};

        auto [min, max, avg] = Cpp17::calc_stats(data);
    }
}