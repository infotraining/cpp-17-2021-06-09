#include <algorithm>
#include <iostream>
#include <numeric>
#include <string>
#include <vector>

#include "catch.hpp"

using namespace std;

struct Aggregate1
{
    int a;
    double b;
    int coord[3];
    std::string name;

    void print() const
    {
        std::cout << "Aggregate1{" << a << ", " << b;
        std::cout << ", [ ";
        for (const auto& item : coord)
            std::cout << item << " ";
        std::cout << "], '" << name << "'}\n";
    }
};

// since C++17
struct Aggregate2 : Aggregate1, std::string
{
    std::vector<int> data;
};

TEST_CASE("aggregate")
{
    Aggregate1 agg1 {1, 3.14, {1, 2, 3}, "Jan"};
    agg1.print();

    Aggregate1 agg2 {1, 3.14};
    agg2.print();

    Aggregate1 agg3 {};
    agg3.print();

    int numbers[10] = {1, 2, 3};

    static_assert(std::is_aggregate_v<Aggregate2>);

    Aggregate2 super_agg {{1, 3.14, {1, 2, 3}, "Agg1"}, {"text"}, {12, 13, 14}};
    super_agg.print();
    REQUIRE(super_agg.size() == 4);
    REQUIRE(super_agg.data[0] == 12);

    std::cout << super_agg.c_str() << "\n";
}
