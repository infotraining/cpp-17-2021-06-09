#include <algorithm>
#include <iostream>
#include <set>
#include <string>
#include <string_view>
#include <vector>

#include "catch.hpp"

using namespace std;

// http://quick-bench.com/Zy4c_WSYI9V1ozF-KUU9JKz2Dhs

namespace BeforeCpp17
{
    vector<string> split(const string& text, const string& delims = " ,")
    {
        vector<string> tokens;

        auto it1 = cbegin(text);

        while (it1 != cend(text))
        {
            auto it2 = find_first_of(it1, cend(text), cbegin(delims), cend(delims));

            tokens.emplace_back(it1, it2);

            if (it2 == cend(text))
                break;

            it1 = std::next(it2);
        }

        return tokens;
    }
}

namespace SinceCpp17
{
    vector<string_view> split_sv(string_view text, string_view delims = " ,")
    {
        vector<string_view> tokens;

        auto it1 = cbegin(text);

        while (it1 != cend(text))
        {
            auto it2 = find_first_of(it1, cend(text), cbegin(delims), cend(delims));

            tokens.emplace_back(it1, it2 - it1);

            if (it2 == cend(text))
                break;

            it1 = std::next(it2);
        }

        return tokens;
    }
}

using namespace SinceCpp17;

TEST_CASE("split with spaces")
{
    string text = "one two three four";

    auto words = split_text(text);

    auto expected = {"one", "two", "three", "four"};

    REQUIRE(equal(begin(expected), end(expected), begin(words)));
}

TEST_CASE("split with commas")
{
    string text = "one,two,three,four";

    auto words = split_text(text);

    auto expected = {"one", "two", "three", "four"};

    REQUIRE(equal(begin(expected), end(expected), begin(words)));
}
