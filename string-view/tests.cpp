#include <algorithm>
#include <numeric>
#include <iostream>
#include <string>
#include <vector>
#include <string_view>
#include <array>

#include "catch.hpp"

using namespace std;

TEST_CASE("string_view")
{
    SECTION("creating sv")
    {
        const char* cstr = "text";

        std::string_view sv1 = cstr;
        REQUIRE(sv1.size() == 4);

        std::string str = "abc";
        std::string_view sv2 = str;
        std::cout << sv2 << "\n";

        char text[] = {'a', 'b', 'c'};
        std::string_view sv3(text, 3);
        std::cout << sv3 << "\n";

        auto sv4 = "text"sv;

        std::string_view sv_empty;
        REQUIRE(sv_empty.data() == nullptr);

        std::string str_empty;
        REQUIRE(str_empty.data() != nullptr);
    }
}

template <typename Container>
void print_all(const Container& container, std::string_view prefix)
{
    cout << prefix << ": [ ";
    for (const auto& item : container)
        cout << item << " ";
    cout << "]\n";
}

TEST_CASE("using string_view")
{
    print_all(vector{1, 2, 3}, "vec");
}

string_view get_prefix(string_view text, size_t length)
{
    return {text.data(), length};
}


TEST_CASE("BEWARE of string_view")
{
    std::string_view evil = "abc"s;  // UB - dangling pointer
    std::cout << evil << std::endl;

    std::string text = "abcdef";

    std::cout << get_prefix(text, 3) << std::endl;
    std::cout << get_prefix("Jan Kowalski"s, 3) << std::endl; // UB - dangling pointer
}

TEST_CASE("string_view as constexpr")
{
    constexpr std::array ids = {"motorola"sv, "nokia"sv, "ericsson"sv};
}