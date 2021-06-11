#include <algorithm>
#include <numeric>
#include <iostream>
#include <string>
#include <vector>
#include <optional>
#include <atomic>
#include <charconv>
#include <array>

#include "catch.hpp"

using namespace std;

TEST_CASE("optional")
{
    std::optional<int> o1;
    REQUIRE(o1.has_value() == false);

    std::optional o2 = 42; // optional<int>
    REQUIRE(o2.has_value());

    o2 = std::nullopt;
    REQUIRE(o2.has_value() == false);

    std::optional<std::atomic<int>> counter(std::in_place, 42);

    if (counter)
    {
        std::cout << "Counter: " << *counter << "\n";
    }

    counter.reset();
    REQUIRE_THROWS_AS(counter.value(), std::bad_optional_access);

    counter.emplace(68);
    REQUIRE(counter.value() == 68);

    counter = std::nullopt;

    optional<string> config_file;
    REQUIRE(config_file.value_or("default_file.cfg") == "default_file.cfg"s);
}

TEST_CASE("weird stuff")
{
    optional<string> text = "text"s;

    auto target = std::move(*text);

    REQUIRE(text.has_value());

    if (text)
    {
        //std::cout << *text << std::endl; // UB
    }

    std::optional<bool> flag{false};

    if (!flag) // yield false
    {
    }

    if (flag == false) // yield true
    {
    }
}


std::optional<int> to_int(string_view str)
{
    int value{};

    auto start = str.data();
    auto end = str.data() + str.size();

    if (const auto [pos_end, error_code] = std::from_chars(start, end, value);
        error_code != std::errc{} || pos_end != end)
    {
        return nullopt;
    }

    return value;
}

TEST_CASE("to_int")
{
    optional<int> number = to_int("42");

    REQUIRE(number == 42);

    number = to_int("44as");

    REQUIRE(number.has_value() == false);
}

template <typename TContainer>
constexpr std::optional<std::string_view> find_id(const TContainer& container, std::string_view id)
{
    for(const auto& item : container)
        if (item == id)
            return item;

    return nullopt;
}

TEST_CASE("string_view + optional + constexpr")
{
    constexpr std::array ids = { "one"sv, "two"sv, "three"sv };

    static_assert(ids[1] == "two"sv);

    constexpr optional opt_id = find_id(ids, "two"sv);

    static_assert(opt_id.has_value());
}