#include <iostream>
#include <list>
#include <vector>

#include "catch.hpp"

using namespace std;

template <typename Iterator>
void advance_it(Iterator& it, size_t n)
{
    using it_category = typename std::iterator_traits<Iterator>::iterator_category;

    if constexpr (std::is_base_of_v<std::random_access_iterator_tag, it_category>)
    {
        it += n;
    }
    else
    {
        while (n--)
        {
            ++it;
        }
    }
}

TEST_CASE("constexpr-if with iterator categories")
{
    SECTION("random_access_iterator")
    {
        vector<int> data = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

        auto it = data.begin();

        advance_it(it, 3);

        REQUIRE(*it == 4);
    }

    SECTION("input_iterator")
    {
        list<int> data = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

        auto it = data.begin();

        advance_it(it, 3);

        REQUIRE(*it == 4);
    }
}