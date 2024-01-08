#pragma once

#include <SDL2/SDL.h>

#include "../ECS/ECS.h"
#include "../Components/SpriteComponent.h"
#include "../Components/AnimationComponent.h"
class AnimationSystem: public System{
    public:
        AnimationSystem(){
            RequireComponent<SpriteComponent>();
            RequireComponent<AnimationComponent>();
        }

        void Update(){
            for (auto entity: GetSystemEntities()){
                auto& animation = entity.GetComponent<AnimationComponent>();
                auto& sprite = entity.GetComponent<SpriteComponent>();
                
                // Calculate current frame based on how long the animation has been running, framerate, and the number of frames in the animation
                animation.currentFrame = ((SDL_GetTicks() - animation.startTime) * animation.frameRateSpeed / 1000) % animation.numFrames;
                // Change the source rectangle of the sprite component based on current frame and sprite width
                sprite.srcRect.x = animation.currentFrame * sprite.width;
            }
        }
};
