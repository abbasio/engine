#pragma once

#include <SDL2/SDL.h>

#include "../EventBus/Event.h"

class KeyPressedEvent: public Event {
    public:
        std::string key;
        KeyPressedEvent(std::string key): key(key) {}
};
