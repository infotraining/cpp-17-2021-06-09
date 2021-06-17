#define CATCH_CONFIG_MAIN

#include "../src/turnstile.hpp"
#include "catch.hpp"
#include <algorithm>

using namespace std;
using namespace cpp17;

class MockTurnstileAPI : public TurnstileAPI
{
public:
    vector<string> operations;

    void lock() override
    {
        operations.push_back("L");
    }

    void unlock() override
    {
        operations.push_back("U");
    }

    void alarm() override
    {
        operations.push_back("A");
    }

    void display(const string& msg) override
    {
        operations.push_back("D:" + msg);
    }
};

TEST_CASE("Turnstile")
{
    SECTION("default state is locked")
    {
        MockTurnstileAPI mq_api;
        Turnstile t{mq_api};

        REQUIRE(t.state() == TurnstileState::locked);
    }
}

SCENARIO("Turnstile in locked state")
{
    GIVEN("Turnstile is in locked state")
    {
        MockTurnstileAPI mq_api;
        Turnstile t{mq_api};

        WHEN("coin is inserted")
        {
            t.coin();

            THEN("state is changed to unlocked")
            {
                REQUIRE(t.state() == TurnstileState::unlocked);
            }

            THEN("lock in turnstile is unlocked")
            {
                REQUIRE(mq_api.operations.back() == "U");
            }
        }

        WHEN("pass")
        {
            t.pass();

            THEN("state is not changed")
            {
                REQUIRE(t.state() == TurnstileState::locked);
            }

            THEN("alarm is raised")
            {
                REQUIRE(mq_api.operations.back() == "A");
            }
        }
    }
}

SCENARIO("Turnstile in unlocked state")
{
    GIVEN("Turtnstile is in unlocked state")
    {
        MockTurnstileAPI mq_api;
        Turnstile t{mq_api};
        t.coin();
        REQUIRE(t.state() == TurnstileState::unlocked);

        WHEN("pass")
        {
            t.pass();

            THEN("state is changed to locked")
            {
                REQUIRE(t.state() == TurnstileState::locked);
            }

            THEN("lock in turnstile is locked")
            {
                REQUIRE(mq_api.operations.back() == "L");
            }
        }

        WHEN("coin is inserted")
        {
            t.coin();

            THEN("state is not changed")
            {
                REQUIRE(t.state() == TurnstileState::unlocked);
            }

            THEN("thank you is displayed")
            {
                REQUIRE(mq_api.operations.back() == "D:Thank you...");
            }
        }
    }
}
