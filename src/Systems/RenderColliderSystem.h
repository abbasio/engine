#pragma once

#include <SDL2/SDL.h>

#include "../ECS/ECS.h"
#include "../Components/TransformComponent.h"
#include "../Components/BoxColliderComponent.h"


class RenderColliderSystem: public System{
    public:
        RenderColliderSystem(){
            RequireComponent<TransformComponent>();
            RequireComponent<BoxColliderComponent>();
        }

        void Update(SDL_Renderer* renderer, SDL_Rect& camera){
            for (auto entity: GetSystemEntities()){
                const auto transform = entity.GetComponent<TransformComponent>();
                const auto collider = entity.GetComponent<BoxColliderComponent>();

                SDL_Rect colliderRect = {
                    static_cast<int>(transform.position.x + (collider.offset.x * transform.scale.x)) - camera.x,
                    static_cast<int>(transform.position.y + (collider.offset.y * transform.scale.y)) - camera.y,
                    collider.width * (int)transform.scale.x,
                    collider.height * (int)transform.scale.y
                };
                if (collider.isColliding) SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
                else SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
                SDL_RenderDrawRect(renderer, &colliderRect);
            }
        }
};
