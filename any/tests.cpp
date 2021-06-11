#include <algorithm>
#include <any>
#include <iostream>
#include <numeric>
#include <string>
#include <vector>

#include "catch.hpp"

using namespace std;

TEST_CASE("any")
{
    std::any a;

    REQUIRE(a.has_value() == false);

    a = 4;
    a = 3.14;
    a = "text"s;
    a = vector {1, 2, 3};

    REQUIRE(a.has_value());

    SECTION("any_cast by copy")
    {
        auto vec = std::any_cast<std::vector<int>>(a);
        REQUIRE(vec == vector {1, 2, 3});

        REQUIRE_THROWS_AS(std::any_cast<std::string>(a), std::bad_any_cast);
    }

    SECTION("any_cast by pointer")
    {
        if (auto* vec_ptr = std::any_cast<std::vector<int>>(&a); vec_ptr)
        {
            vec_ptr->push_back(4);
            REQUIRE(a.type() == typeid(vector<int>));
        }
        else
        {
            REQUIRE(vec_ptr == nullptr);
        }
    }
}

////////////////////////////////////
// wide interfaces

class Observer
{
public:
    virtual void update(const std::any& sender, const string& msg) = 0;
    virtual ~Observer() = default;
};

class TempMonitor
{
    std::vector<Observer*> observes_;
public:
    void notify()
    {
        for(const auto& o : observes_)
            o->update(this, std::to_string(get_temp()));
    }
    double get_temp() const
    {
        return 23.88;
    }
};

class Logger : public Observer
{
public:
    void update(const std::any& sender, const string& msg) override
    {
        TempMonitor* const* monitor = std::any_cast<TempMonitor*>(&sender);
        if (monitor)
            (*monitor)->get_temp();
    }
};