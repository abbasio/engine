#pragma once

#include <SDL2/SDL.h>
#include <glm/glm.hpp>
struct ProjectileEmitterComponent {
    glm::vec2 projectileVelocity;
    int projectileFrequency;
    int projectileDuration;
    int hitPercentDamage;
    bool isFriendly;
    int lastFiredTime;

    ProjectileEmitterComponent(glm::vec2 projectileVelocity = glm::vec2(0), int projectileFrequency = 0, int projectileDuration = 10000, int hitPercentDamage = 10, bool isFriendly = false) {
        this -> projectileVelocity = projectileVelocity;
        this -> projectileFrequency = projectileFrequency;
        this -> projectileDuration = projectileDuration;
        this -> hitPercentDamage = hitPercentDamage;
        this -> isFriendly = isFriendly;
        this -> lastFiredTime = SDL_GetTicks();
    }
};
