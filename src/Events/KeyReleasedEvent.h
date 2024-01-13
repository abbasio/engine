#pragma once

#include <SDL2/SDL.h>
#include "../ECS/ECS.h"
#include "../EventBus/Event.h"

class KeyReleasedEvent: public Event {
    public:
        std::string key;
        KeyReleasedEvent(std::string key): key(key) {}
};
