#pragma once

#include <algorithm>
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
        
        void Update(SDL_Renderer* renderer, std::unique_ptr<AssetStore>& assetStore, SDL_Rect& camera){
           // Create a vector of sprite and transform components for all entities
            struct RenderableEntity {
                TransformComponent transformComponent; 
                SpriteComponent spriteComponent;
            };
            
            std::vector<RenderableEntity> renderableEntities;
            std::sort(GetSystemEntities().begin(), GetSystemEntities().end(), [](Entity a, Entity b){
                return a.GetComponent<SpriteComponent>().zIndex < b.GetComponent<SpriteComponent>().zIndex;        
            });
           
            // Loop all entities that the system is interested in
            for (auto entity: GetSystemEntities()){
                const auto transform = entity.GetComponent<TransformComponent>();
                const auto sprite = entity.GetComponent<SpriteComponent>();
                bool isEntityOutsideCameraView = {
                    transform.position.x + (transform.scale.x * sprite.width) < camera.x ||
                    transform.position.x > camera.x + camera.w ||
                    transform.position.y + (transform.scale.y * sprite.height)< camera.y ||
                    transform.position.y > camera.y + camera.h    
                };

                if (isEntityOutsideCameraView && !sprite.isFixed) continue;

                SDL_Texture* texture = assetStore -> GetTexture(sprite.assetId);   
               
                // Set source rectangle of our original sprite texture
                SDL_Rect srcRect = sprite.srcRect;
                // Look into SDL_QueryTexture to handle automatically

                // Set destination rectangle with the x, y position to be rendered
                // Base on the camera position
                SDL_Rect destRect = {
                    static_cast<int>(transform.position.x - (sprite.isFixed ? 0 : camera.x)),
                    static_cast<int>(transform.position.y - (sprite.isFixed ? 0 : camera.y)),
                    static_cast<int>(sprite.width * transform.scale.x),
                    static_cast<int>(sprite.height * transform.scale.y)
                };

                SDL_RenderCopyEx(
                            renderer,
                            texture, 
                            &srcRect,
                            &destRect,
                            transform.rotation,
                            NULL,
                            sprite.flip
                        );    


            }
        }  

};
