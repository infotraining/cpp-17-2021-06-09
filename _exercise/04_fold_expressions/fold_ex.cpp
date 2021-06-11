#include <iostream>
#include <memory>
#include <set>
#include <string>
#include <string_view>
#include <vector>
#include <algorithm>

#include "catch.hpp"

using namespace std;

/////////////////////////////////////////////////////////////////////////////////////////////////

// TODO
template <typename TContainer, typename... TArgs>
size_t matches(const TContainer& vec, const TArgs&... items)
{
   return (... + std::count(std::begin(vec), std::end(vec), items));
}

TEST_CASE("matches - returns how many items is stored in a container")
{
    // Tip: use std::count() algorithm

    vector<int> v{1, 2, 3, 4, 5};

    REQUIRE(matches(v, 2, 5) == 2);
    REQUIRE(matches(v, 100, 200) == 0);
    REQUIRE(matches("abccdef", 'x', 'y', 'z') == 0);
    REQUIRE(matches("abccdef", 'a', 'c', 'f') == 4);
}

/////////////////////////////////////////////////////////////////////////////////////////////////

class Gadget
{
public:
    virtual std::string id() const { return "a"; }
    virtual ~Gadget() = default;
};

class SuperGadget : public Gadget
{
public:
    std::string id() const override
    {
        return "b";
    }
};


template<typename... Ts> 
std::vector<std::common_type_t<Ts...>> make_vector(Ts&&... args)
{
    std::vector<std::common_type_t<Ts...>> ret_val;

    ret_val.reserve(sizeof...(args));

    (..., ret_val.push_back(std::forward<Ts>(args)));
    
    return ret_val;
}


TEST_CASE("make_vector - create vector from a list of arguments")
{
    // Tip: use std::common_type_t<Ts...> trait

    using namespace Catch::Matchers;

    SECTION("ints")
    {
        int x = 10;

        std::vector<int> v = make_vector(1, 2, 3, x);

        REQUIRE_THAT(v, Equals(vector{1, 2, 3, 10}));
    }

    SECTION("unique_ptrs")
    {
        auto ptrs = make_vector(make_unique<int>(5), make_unique<int>(6));

        REQUIRE(ptrs.size() == 2);
    }

    SECTION("unique_ptrs with polymorphic hierarchy")
    {
        auto gadgets = make_vector(make_unique<Gadget>(), make_unique<SuperGadget>(), make_unique<Gadget>());

        static_assert(is_same_v<decltype(gadgets)::value_type, unique_ptr<Gadget>>);

        vector<string> ids;
        transform(begin(gadgets), end(gadgets), back_inserter(ids), [](auto& ptr) { return ptr->id(); });

        REQUIRE_THAT(ids, Equals(vector<string>{"a", "b", "a"}));
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////

template <typename T>
struct Range
{
    T low, high;
};

template <typename T1, typename T2>
Range(T1,T2) -> Range<std::common_type_t<T1, T2>>;

template <typename T, typename... TArgs>
bool all_within(const Range<T>& range, const TArgs&... args)
{
    auto in_range = [&range](const auto& value) { return value >= range.low && value <= range.high; };

    return (... && in_range(args));
}

TEST_CASE("all_within - checks if all values fit in range [low, high]")
{
    REQUIRE(all_within(Range{10, 20.0}, 1, 15, 30) == false);
    REQUIRE(all_within(Range{10, 20}, 11, 12, 13) == true);
    REQUIRE(all_within(Range{5.0, 5.5}, 5.1, 5.2, 5.3) == true);
}