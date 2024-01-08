#pragma once

#include <SDL2/SDL.h>
#include "../ECS/ECS.h"
#include "../Components/TransformComponent.h"
#include "../Components/SpriteComponent.h"
#include "../AssetStore/AssetStore.h"
class RenderSystem: public System{
    public: 
        RenderSystem(){
            RequireComponent<SpriteComponent>();
            RequireComponent<TransformComponent>();
        }
        
        void Update(SDL_Renderer* renderer, std::unique_ptr<AssetStore>& assetStore){
            // Sort all entities of our system by their z index
            
            // Loop all entities that the system is interested in
            for (auto entity: GetSystemEntities()){
                const auto transform = entity.GetComponent<TransformComponent>();
                const auto sprite = entity.GetComponent<SpriteComponent>();
                SDL_Texture* texture = assetStore -> GetTexture(sprite.assetId);   
                
                // Set source rectangle of our original sprite texture
                SDL_Rect srcRect = sprite.srcRect;
                if (srcRect.w == 0 && srcRect.h == 0){
                    SDL_QueryTexture(texture, NULL, NULL, &srcRect.w, &srcRect.h);
                }
                
                auto spriteWidth = sprite.width;
                auto spriteHeight = sprite.height;
                if (spriteWidth == 0 && spriteHeight == 0){
                    spriteWidth = srcRect.w;
                    spriteHeight = srcRect.h;
                } 
    
                // Set destination rectangle with the x, y position to be rendered
                SDL_Rect destRect = {
                    static_cast<int>(transform.position.x),
                    static_cast<int>(transform.position.y),
                    static_cast<int>(spriteWidth * transform.scale.x),
                    static_cast<int>(spriteHeight * transform.scale.y)
                };

                SDL_RenderCopyEx(
                            renderer,
                            texture, 
                            &srcRect,
                            &destRect,
                            transform.rotation,
                            NULL,
                            SDL_FLIP_NONE
                        );    


            }
        }  

};
