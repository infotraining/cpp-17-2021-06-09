#include <algorithm>
#include <numeric>
#include <iostream>
#include <string>
#include <vector>
#include <variant>

#include "catch.hpp"

using namespace std;

TEST_CASE("variant")
{
    std::variant<int, double, std::string, std::vector<int>> v1;

    REQUIRE(std::holds_alternative<int>(v1) == true);    
    REQUIRE(std::get<int>(v1) == 0);

    v1 = 3.14;
    v1 = "text"s;
    v1 = std::vector{1, 2, 3};   
}

struct S
{
    S(int v) : value{v}
    {}
 
    int value;
};

TEST_CASE("monostate")
{
    std::variant<std::monostate, S, int> v;
    REQUIRE(std::holds_alternative<std::monostate>(v));
}

TEST_CASE("emplace")
{
    std::variant<int, std::string, int> v;

    v.emplace<std::string>("text");
    REQUIRE(std::get<string>(v) == "text"s);

    v.emplace<0>(42);
    v.emplace<2>(665);

    REQUIRE(v.index() == 2);
    REQUIRE(std::get<2>(v) == 665);

    REQUIRE_THROWS_AS(std::get<string>(v), std::bad_variant_access);

    if (string* ptr_text = std::get_if<string>(&v); ptr_text)
    {
        std::cout << *ptr_text << "\n";
    }
}

// Visitor
struct Printer
{
    void operator()(int x) { std::cout << "int: " << x << "\n"; }
    void operator()(double x) { std::cout << "double: " << x << "\n"; }
    void operator()(const std::string& s) { std::cout << "string: " << s << "\n"; }
    void operator()(const std::vector<int>& v) { std::cout << "vector: " << v.size() << "\n"; }
};


// aggregate
template <typename... Closures>
struct overload : Closures...
{
    using Closures::operator()...; // Since C++17
};

// deduction guide
template <typename... Closures>
overload(Closures...) -> overload<Closures...>;


TEST_CASE("visiting")
{
    std::variant<int, double, std::string, std::vector<int>> v = 3.14;
    v = 42;

    std::visit(Printer{}, v);

    auto printer = overload {
        [](int x) { std::cout << "int: " << x << "\n"; },
        [](double x) { std::cout << "double: " << x << "\n"; },
        [](const std::string& s) { std::cout << "string: " << s << "\n"; },
        [](const std::vector<int>& v) { std::cout << "vector: " << v.size() << "\n"; }
    };

    std::visit(printer, v);
}


[[nodiscard]] std::variant<std::string, std::errc> load_content(const std::string& filename)
{
    if (filename == "")
        return std::errc::bad_file_descriptor;
    return "content"s;          
}

TEST_CASE("using variant")
{
    auto result = load_content("data");

    std::visit(overload{ 
        [](std::errc ec) { std::cout << "Error!!! " <<  static_cast<int>(ec) << std::endl; },
        [](const std::string& s) { std::cout << "Content: " << s << "\n"; }
    }, result);
}