#include <algorithm>
#include <array>
#include <iostream>
#include <numeric>
#include <string>
#include <vector>

#include "catch.hpp"

using namespace std;

struct Gadget
{
    string id;
    int usage_counter = 0;

    explicit Gadget(string id)
        : id {move(id)}
    {
        cout << "Gadget(" << this->id << " - " << this << ")\n";
    }

    Gadget(const Gadget& source)
        : id {source.id}
    {
        cout << "Gadget(cc: " << this->id << " - " << this << ")\n";
    }

    Gadget(Gadget&& source) noexcept
        : id {move(source.id)}
    {
        cout << "Gadget(mv: " << this->id << " - " << this << ")\n";
    }

    ~Gadget()
    {
        cout << "~Gadget(" << this->id << " - " << this << ")\n";
    }

    void use()
    {
        ++usage_counter;

        auto printer = [this]()
        { std::cout << "Using " << id << "\n"; };
        printer();
    }

    auto get_reporter()
    {
        return [*this]
        { std::cout << "Report from Gadget(" << id << ")\n"; };
    }
};

TEST_CASE("lambda expressions")
{
    std::function<void()> reporter;

    {
        Gadget g {"ipad"};
        g.use();
        reporter = g.get_reporter();
    }

    reporter();
}

TEST_CASE("lambda as constexpr")
{
    auto square = [](int x)
    { return x * x; };

    int tab1[square(8)] = {};

    static_assert(std::size(tab1) == 64);
}

template <size_t N>
constexpr auto create_array()
{
    auto square = [](int x)
    { return x * x; };

    std::array<int, square(N)> data = {square(1), square(2), square(5)};

    for (size_t i = 3; i < square(N); ++i)
        data[i] = square(i);

    return data;
}

template <typename TContainer>
void print(string_view prefix, const TContainer& container)
{
    std::cout << prefix << ": [ ";
    for (const auto& item : container)
        std::cout << item << " ";
    std::cout << "]\n";
}

namespace Cpp20
{
    template <typename Iterator, typename Predicate>
    constexpr Iterator find_if(Iterator first, Iterator last, Predicate pred)
    {
        while (first != last)
        {
            if (pred(*first))
            {
                return first;
            }
            first++;
        }
        return last;
    }
}

TEST_CASE("using constexpr lambda")
{
    constexpr auto data = create_array<10>();

    print("data", data);

    constexpr int lt_5000 = *Cpp20::find_if(begin(data), end(data), [](int item) { return item > 5000; });

    static_assert(lt_5000 == 5041);
}