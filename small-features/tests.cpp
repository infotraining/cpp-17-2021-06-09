#include <algorithm>
#include <numeric>
#include <iostream>
#include <string>
#include <vector>
#include <future>
#include <thread>

#include "catch.hpp"

using namespace std;

struct ErrorCode
{
    int err_code;
    const char* msg;
};

[[nodiscard]] ErrorCode open_file(const std::string& filename)
{
    if (filename == ""s)
        return {13, "Empty string as filename"};
    return {0, ""};
}

std::string load_file(const std::string& filename)
{
    std::cout << "Start loading " << filename << "..." << std::endl;
    std::this_thread::sleep_for(1s);
    return "Content: "s + filename;
}

void save_file(const std::string& filename)
{
    std::cout << "Start saving " << filename << "..." << std::endl;
    std::this_thread::sleep_for(1s);
    std::cout << "Saved: "s + filename << std::endl;
}

TEST_CASE("attributes")
{
    auto [error_code, message] = open_file("");
    REQUIRE(error_code == 13);
}


[[nodiscard]] std::future<void> save_task(const std::string& filename) 
{
    std::packaged_task<void()> pt ([filename] { save_file(filename); });
    auto f = pt.get_future();

    std::thread thd{std::move(pt)};
    thd.detach();

    return f;

    // return std::async(std::launch::async, save_file, filename);
}

TEST_CASE("async bugs")
{
    std::future<std::string> f1 = std::async(std::launch::async, load_file, "data1.txt");
    std::future<std::string> f2 = std::async(std::launch::async, load_file, "data2.txt");

    while (f2.wait_for(0ms) != std::future_status::ready)
    {};

    std::cout << f1.get() << std::endl;
    std::cout << f2.get() << std::endl;

    ///////////////// BUG
    auto f3 = std::async(std::launch::async, save_file, "f1.txt");
    auto f4 = std::async(std::launch::async, save_file, "f2.txt");
    auto f5 = std::async(std::launch::async, save_file, "f3.txt");

    ///////////////// ALT TAKE
    save_task("f1");
    save_task("f2");
    save_task("f3");    

    [[maybe_unused]] int x;
}

enum Coffee
{
    espresso,
    americano [[deprecated]] = espresso
};

void step1() { }
void step2() { }
void step3() { }

namespace [[deprecated]] LegacyCode
{
    namespace Lib::ver_1_0
    {
        void f(int n)
        {
            switch (n)
            {
            case 1:
            case 2:
                step1();
                [[fallthrough]];
            case 3: // no warning on fallthrough
                step2();
                break;
            case 4:
                step3();
            }
        }
    }
}

TEST_CASE("deprecated")
{
    Coffee c = espresso;

    LegacyCode::Lib::ver_1_0::f(1);

    static_assert(sizeof(int) == 4);
}

