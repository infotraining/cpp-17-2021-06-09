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

using namespace std::literals;

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

std::array<int, 3> get_coord()
{
    return std::array {1, 2, 3};
}

struct Description
{
    std::string header;
    std::string content;
};

struct ErrorCode
{
    int ec;
    const char* m;
    Description d;
};

ErrorCode open_file(const char* filename)
{
    return ErrorCode {13, "Error#13", {"Header", "Content"}};
}

TEST_CASE("Structured bindings - features")
{
    SECTION("native arrays")
    {
        int coords[] = {1, 2, 3};

        auto [x, y, z] = coords;

        REQUIRE(x == 1);
        REQUIRE(y == 2);
    }

    SECTION("std::pair")
    {
        std::map<int, std::string> dict = {{1, "one"}, {2, "two"}};
        auto [pos, was_inserted] = dict.insert(std::pair(3, "three"));

        REQUIRE(was_inserted == true);
    }

    SECTION("std::tuple")
    {
        auto [one, two, pi] = std::tuple(1, "two"s, 3.14);
    }

    SECTION("std::array")
    {
        auto [x, y, z] = get_coord();

        REQUIRE(x == 1);
        REQUIRE(z == 3);
    }

    SECTION("struct/class")
    {
        auto [error_code, error_message, description] = open_file("not-found");

        REQUIRE(error_code == 13);
        REQUIRE(error_message == "Error#13"s);
        REQUIRE(description.header == "Header");
    }
}

struct Timestamp
{
    int h, m, s;
};

TEST_CASE("Structure bindings - how it works")
{
    Timestamp t1 {10, 59, 0};

    auto [hours, minutes, seconds] = t1;

    hours = 11;

    REQUIRE(t1.h == 10);

    SECTION("is interpreted as")
    {
        auto entity = t1;
        auto&& hours = entity.h;
        auto&& minutes = entity.m;
        auto&& seconds = entity.s;
    }
}

TEST_CASE("Modifiers")
{
    Timestamp t1 {10, 59, 0};

    auto& [hours, minutes, seconds] = t1;

    hours++;

    REQUIRE(t1.h == 11);

    SECTION("is interpreted as")
    {
        auto& entity = t1;
        auto&& hours = entity.h;
        auto&& minutes = entity.m;
        auto&& seconds = entity.s;
    }
}

TEST_CASE("use cases")
{
    SECTION("iteration over map")
    {
        std::map<int, std::string> dict = { {1, "one"}, {2, "two"} };
        auto [pos, was_inserted] = dict.insert(std::pair(3, "three"));

        for(const auto& [key, value] : dict)
        {
            std::cout << key << " - " << value << "\n";
        }
    }

    SECTION("for + multiple declarations")
    {
        std::list lst{"lone", "ltwo", "lthree"};

        for(auto [index, it] = std::tuple(0, lst.begin()); it != lst.end(); ++it, ++index)
        {
            std::cout << index << " - " << *it << "\n";
        }
    }
}

////////////////////////////////////////
// tuple like protocol

enum Something 
{
    some, thing
};

const std::map<Something, std::string_view> something_desc = {
    { some, "some"sv}, {thing, "thing"sv}
};

// step 1 - std::tuple_size<Something>
template<>
struct std::tuple_size<Something> {
    static constexpr size_t value = 2;
};

// step 2 - std::tuple_element<Index, Something>
template <>
struct std::tuple_element<0, Something> {
    using type = int;
};

template <>
struct std::tuple_element<1, Something> {
    using type = std::string_view;
};

// step 3 - get<Index>
template <size_t Index>
decltype(auto) get(const Something& sth);

template <>
decltype(auto) get<0>(const Something& sth)
{
    return static_cast<int>(sth);
}

template <>
decltype(auto) get<1>(const Something& sth)
{
    return something_desc.at(sth);
}

TEST_CASE("Structured binding for Something")
{
    Something sth = some;

    const auto [value, description] = sth;

    REQUIRE(value == 0);
    REQUIRE(description == "some"sv);
}