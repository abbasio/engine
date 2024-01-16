#pragma once

#include <SDL2/SDL.h>

#include "../ECS/ECS.h"
#include "../Components/LifecycleComponent.h"

class LifecycleSystem: public System {
    public:
        LifecycleSystem() {
            RequireComponent<LifecycleComponent>();
        }

        void Update() {
            for (auto entity: GetSystemEntities()){
                auto lifecycle = entity.GetComponent<LifecycleComponent>();
                if((SDL_GetTicks() - lifecycle.startTime) >= lifecycle.timeToLive) {
                   entity.Kill(); 
                }     
            }
        }
};

