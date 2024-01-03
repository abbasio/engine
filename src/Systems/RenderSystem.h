#pragma once

#include <SDL2/SDL.h>
#include "../ECS/ECS.h"
#include "../Components/TransformComponent.h"
#include "../Components/SpriteComponent.h"
class RenderSystem: public System{
    public: 
        RenderSystem(){
            RequireComponent<SpriteComponent>();
            RequireComponent<TransformComponent>();
        }
        
        void Update(SDL_Renderer* renderer){
            //Loop all entities that the system is interested in
            for (auto entity: GetSystemEntities()){
                const auto transform = entity.GetComponent<TransformComponent>();
                const auto sprite = entity.GetComponent<SpriteComponent>();   
                
                SDL_Rect objRect = {
                    static_cast<int>(transform.position.x),
                    static_cast<int>(transform.position.y),
                    sprite.width,
                    sprite.height   
                };
                SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); 
                SDL_RenderFillRect(renderer, &objRect);    
            }
        }  

};
