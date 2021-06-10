#include <algorithm>
#include <iostream>
#include <numeric>
#include <string>
#include <vector>

#include "catch.hpp"

using namespace std;

void foo()
{
}

template <typename T>
void deduce1(T arg)
{
    puts(__PRETTY_FUNCTION__);
}

TEST_CASE("Template Argument Deduction - case 1")
{
    int x = 10;
    const int cx = 10;
    int& ref_x = x;
    const int& cref_x = cx;
    int tab[10];

    auto ax1 = x; // int
    deduce1(x);

    auto ax2 = cx; // int
    deduce1(cx);

    auto ax3 = ref_x; // int
    deduce1(ref_x);

    auto ax4 = cref_x; // int
    deduce1(cref_x);

    auto ax5 = tab; // int*  - array decays to pointer
    deduce1(tab);

    auto ax6 = foo; // void(*)()
    deduce1(foo);
}

template <typename T>
void deduce2(T& arg)
{
    puts(__PRETTY_FUNCTION__);
}

TEST_CASE("Template Argument Deduction - case 2")
{
    int x = 10;
    const int cx = 10;
    int& ref_x = x;
    const int& cref_x = cx;
    int tab[10];

    auto& ax1 = x; // int&
    deduce2(x);

    auto& ax2 = cx; // const int&
    deduce2(cx);

    auto& ax3 = ref_x; // int&
    deduce2(ref_x);

    auto& ax4 = cref_x; // const int&
    deduce2(cref_x);

    auto& ax5 = tab; // int(&)[5]
    deduce2(tab);

    auto& ax6 = foo; // void(&)()
    deduce2(foo);
}

namespace Explain
{
    template <typename T, size_t N>
    constexpr size_t size(T (&arr)[N])
    {
        return N;
    }
}


TEST_CASE("deduce size of an array")
{
    int tab1[34];

    static_assert(Explain::size(tab1) == 34);
}

template <typename T>
void deduce3(T&& arg)
{
    auto target = std::forward<T>(arg);
    puts(__PRETTY_FUNCTION__);
}

std::string get_name()
{
    return "Jan"s;
}

TEST_CASE("Template Argument Deduction - case 3")
{
    std::string x = "text";
    
    auto&& ax1 = x; // lvalue => auto&& -> string&
    static_assert(std::is_lvalue_reference_v<decltype(ax1)>);
    
    deduce3(x); // 

    auto&& ax2 = get_name(); // rvalue => auto&& -> string&& 
    static_assert(std::is_rvalue_reference_v<decltype(ax2)>);

    ax2.append("!!!");

    deduce3("text"s);  

    deduce3(ax2);
    deduce3(std::move(ax2));

    int tab[10];

    auto&& ax3 = tab; // int(&)[10]
}

namespace Explain
{
    template <typename T>
    class vector
    {
        std::list<T> items_;

    public:
        void push_back(const T& item) // lvalue ref to const
        {
            puts(__PRETTY_FUNCTION__);
            items_.push_back(item); // copy of item
        }

        void push_back(T&& item) // rvalue reference
        {
            puts(__PRETTY_FUNCTION__);
            items_.push_back(std::move(item));
        }

        template <typename... TArgs>
        void emplace_back(TArgs&&... args) // universal reference
        {
            items_.emplace_back(std::forward<TArgs>(args)...);
        }
    };
}

TEST_CASE("vector explained")
{
    Explain::vector<string> vec;

    string text = "text";
    vec.push_back(text); // insert by copy
    vec.push_back("text"s); // insert by move
    vec.emplace_back(10, 'c'); // inserted in-place using args
}