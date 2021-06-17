#include "turnstile.hpp"

using namespace After;

namespace States
{
    LockedState locked_state;
    UnlockedState unlocked_state;
}

ITurnstileState* ITurnstileState::locked_state = &States::locked_state;
ITurnstileState* ITurnstileState::unlocked_state = &States::unlocked_state;
