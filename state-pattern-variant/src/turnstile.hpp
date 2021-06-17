#ifndef CLASS_TURNSTILE_HPP
#define CLASS_TURNSTILE_HPP

#include <iostream>
#include <string>
#include <variant>

class TurnstileAPI
{
public:
    virtual void lock()
    {
        std::cout << "Locking turnstile..." << std::endl;
    }

    virtual void unlock()
    {
        std::cout << "Unlocking turnstile..." << std::endl;
    }

    virtual void alarm()
    {
        std::cout << "Alarm..." << std::endl;
    }

    virtual void display(const std::string& msg)
    {
        std::cout << msg << std::endl;
    }

    virtual ~TurnstileAPI() = default;
};

enum class TurnstileState
{
    locked,
    unlocked
};

namespace Before
{

    class Turnstile
    {
        TurnstileState state_;
        TurnstileAPI& api_;

    public:
        Turnstile(TurnstileAPI& api)
            : state_{TurnstileState::locked},
              api_{api}
        {
        }

        TurnstileState state() const
        {
            return state_;
        }

        void coin()
        {
            if (state_ == TurnstileState::unlocked)
            {
                api_.display("Thank you...");
            }
            else
            {
                api_.unlock();
                state_ = TurnstileState::unlocked;
            }
        }

        void pass()
        {
            if (state_ == TurnstileState::unlocked)
            {
                api_.lock();
                state_ = TurnstileState::locked;
            }
            else
            {
                api_.alarm();
            }
        }
    };
}

namespace After
{
    class ITurnstileState
    {
    public:
        virtual const ITurnstileState* coin(TurnstileAPI& api) const = 0;
        virtual const ITurnstileState* pass(TurnstileAPI& api) const = 0;
        virtual TurnstileState state() const = 0;
        virtual ~ITurnstileState()
        {
        }

        static ITurnstileState* locked_state;
        static ITurnstileState* unlocked_state;
    };

    class LockedState : public ITurnstileState
    {
        // TurnstileState interface
    public:
        const ITurnstileState* coin(TurnstileAPI& api) const override
        {
            api.unlock();
            return ITurnstileState::unlocked_state;
        }

        const ITurnstileState* pass(TurnstileAPI& api) const override
        {
            api.alarm();

            return this;
        }

        TurnstileState state() const override
        {
            return TurnstileState::locked;
        }
    };

    class UnlockedState : public ITurnstileState
    {
        // TurnstileState interface
    public:
        const ITurnstileState* coin(TurnstileAPI& api) const override
        {
            api.display("Thank you...");
            return this;
        }

        const ITurnstileState* pass(TurnstileAPI& api) const override
        {
            api.lock();
            return ITurnstileState::locked_state;
        }

        TurnstileState state() const override
        {
            return TurnstileState::unlocked;
        }
    };

    class Turnstile
    {
        const ITurnstileState* state_;
        TurnstileAPI& api_;

    public:
        Turnstile(TurnstileAPI& api)
            : state_{ITurnstileState::locked_state},
              api_{api}
        {
        }

        TurnstileState state() const
        {
            return state_->state();
        }

        void coin()
        {
            state_ = state_->coin(api_);
        }

        void pass()
        {
            state_ = state_->pass(api_);
        }
    };
}

namespace cpp17
{
    template <typename... Ts>
    struct overloaded : Ts...
    {
        using Ts::operator()...;
    };

    template <typename... Ts>
    overloaded(Ts...) -> overloaded<Ts...>; 

    class Turnstile
    {
        TurnstileAPI& api_;

        struct Locked {};
        struct Unlocked {};

        using TurnstileState = std::variant<Locked, Unlocked>;
        TurnstileState state_ = Locked{};

        struct PassEvent
        {
            TurnstileAPI& api_;

            TurnstileState operator()(const Locked& locked_state) const
            {
                api_.alarm();
                return Locked{};
            }

            TurnstileState operator()(const Unlocked& unlocked_state) const
            {
                api_.lock();
                return Locked{};
            }
        };

        struct CoinEvent
        {
            TurnstileAPI& api_;

            TurnstileState operator()(const Locked& locked_state) const
            {
                api_.unlock();
                return Unlocked{};
            }

            TurnstileState operator()(const Unlocked& unlocked_state) const
            {
                api_.display("Thank you...");
                return Unlocked{};
            }
        };

    public:
        explicit Turnstile(TurnstileAPI& api)
            : api_{api}
        {}

        void coin()
        {
            state_ = std::visit(CoinEvent{api_}, state_);
        }

        void pass()
        {
             state_ = std::visit(PassEvent{api_}, state_);
        }

        ::TurnstileState state() const
        {
            if(std::holds_alternative<Locked>(state_))
                return ::TurnstileState::locked;
            
            return ::TurnstileState::unlocked;
        }
    };
}

#endif //CLASS_TEMPLATES_VECTOR_HPP
