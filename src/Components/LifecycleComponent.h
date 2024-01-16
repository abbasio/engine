#pragma once

#include <SDL2/SDL.h>

struct LifecycleComponent {
    int timeToLive;
    uint32_t startTime
        ;
    LifecycleComponent(int timeToLive = 1000, uint32_t startTime = SDL_GetTicks()) {
        this -> timeToLive = timeToLive;
        this -> startTime = startTime;
    }
};
