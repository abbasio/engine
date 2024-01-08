#pragma once

#include <SDL2/SDL.h>

struct AnimationComponent{
    int numFrames;
    int currentFrame;
    int frameRateSpeed;
    int startTime;
    bool isLoop;

    AnimationComponent(int numFrames = 1, int frameRateSpeed = 1, bool isLoop = true){
        this -> numFrames = numFrames;
        this -> currentFrame = 1;
        this -> frameRateSpeed = frameRateSpeed;
        this -> isLoop = isLoop;
        this -> startTime = SDL_GetTicks();
    }
};


