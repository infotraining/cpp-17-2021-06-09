#include <algorithm>
#include <iostream>
#include <numeric>
#include <optional>
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
    std::remove_const_t<T> backup = arg;
    backup++;

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

// ///////////////
// int x = 10;
// arg is lvalue deduce3(x);

// template <typename T = int&>
// void deduce3(int& && -> int& arg)
// {
//     auto target = arg;
//     puts(__PRETTY_FUNCTION__);
// }

///////////////
// arg is rvalue - deduce3(get_number());

// template <typename T = int>
// void deduce3(int&& arg)
// {
//     auto target = std::move(arg);
//     puts(__PRETTY_FUNCTION__);
// }

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

    std::cout << "\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\n";

    const std::string strx = "stringx";
    const std::string& refstrx = strx;
    std::string&& refrefstrx = "refrefstringx";
    std::string target = std::move(refrefstrx);

    deduce1(strx);
    deduce1(refstrx);
    deduce1(refrefstrx);

    deduce2(strx);
    deduce2(refstrx);
    deduce2(refrefstrx);

    deduce3(strx);
    deduce3(refstrx);
    deduce3(refrefstrx);

    deduce3(std::move(strx));
    deduce3(std::move(refstrx));
    deduce3(std::move(refrefstrx));

    std::cout << "\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\n";
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

template <typename T1, typename T2>
struct ValuePair
{
    T1 fst;
    T2 snd;

    template <typename TFirst, typename TSecond>
    ValuePair(TFirst&& f, TSecond&& s)
        : fst {std::forward<TFirst>(f)}
        , snd {std::forward<TSecond>(s)}
    {
    }
};

// deduction guides
template <typename T1, typename T2>
ValuePair(T1, T2) -> ValuePair<T1, T2>;

ValuePair(const char*, const char*)->ValuePair<std::string, std::string>;

TEST_CASE("CTAD")
{
    ValuePair<int, double> vp1 {1, 3.14};
    ValuePair<int, std::string> vp2 {1, "text"};

    ValuePair vp3 {1, 3.14};
    ValuePair vp4(4, "text"); // ValuePair<int, const char*>
    ValuePair vp5 {3.14, "text"s}; // ValuePair<double, string>
    ValuePair vp6("abc", "defgh"); // ValuePair<std::string, std::string>
}

///////////////////////////
// Aggregates + CTAD

template <typename T>
struct Aggregate1
{
    T value;
};

template <typename T>
Aggregate1(T) -> Aggregate1<T>;

TEST_CASE("CTAD - aggregates")
{
    Aggregate1 agg1 {1}; // Aggregate1<int>
    Aggregate1 agg2 {"text"}; // Aggregate1<const char*>
}

TEST_CASE("CTAD + std library")
{
    int tab[10];
    const int x = 10;

    std::pair p1 {x, tab}; // pair<int, int*>
    auto p2 = std::pair(1, 3.14);

    std::tuple t1 {1, 3.14, "text", "text"s}; // std::tuple<int, double, const char*, string>
    std::tuple t2 = t1; // std::tuple<int, double, const char*, string>
    std::tuple t3 {t1}; // std::tuple<int, double, const char*, string>
    std::tuple t4 = p2; // std::tuple<int, double>

    std::optional opt1 = 42; // std::optional<int>
    std::optional opt2 {3.14}; // std::optional<double>
    std::optional opt3 {opt2}; // std::optional<double>

    std::function f1 = foo; // function<void()>
    std::function f2 = [](int x)
    { std::cout << x << "\n"; }; // function<void(int)>

    int* ptr = new int {5};
    //std::unique_ptr uptr{ptr}; // ERROR - ill-formed (due to array type clash)
    //std::shared_ptr sptr{ptr}; // ERROR - ill-formed (due to array type clash)

    std::unique_ptr<int> uptr{ptr};
    std::shared_ptr sptr = std::move(uptr); // shared_ptr<int>
    std::weak_ptr wptr = sptr; // weak_ptr<int>

    std::vector vec = {1, 2, 3, 4}; // std::vector<int>
    std::vector vec2{vec}; // std::vector<int>
    std::vector vec3{vec, vec}; // std::vector<std::vector<int>>
}