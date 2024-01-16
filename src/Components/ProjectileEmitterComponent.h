#pragma once

#include <SDL2/SDL.h>
#include <glm/glm.hpp>
struct ProjectileEmitterComponent {
    glm::vec2 projectileVelocity;
    int projectileFrequency;
    int projectileDuration;
    int projectileDamage;
    int projectileDamageLayer;
    int lastFiredTime;

    ProjectileEmitterComponent(glm::vec2 projectileVelocity = glm::vec2(0), int projectileFrequency = 0, int projectileDuration = 10000, int projectileDamage = 10, int projectileDamageLayer = 0) {
        this -> projectileVelocity = projectileVelocity;
        this -> projectileFrequency = projectileFrequency;
        this -> projectileDuration = projectileDuration;
        this -> projectileDamage = projectileDamage;
        this -> projectileDamageLayer = projectileDamageLayer;
        this -> lastFiredTime = SDL_GetTicks();
    }
};
