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
        
        void Update(SDL_Renderer* renderer, std::unique_ptr<AssetStore>& assetStore, SDL_Rect& camera){
            // Sort all entities of our system by their z index
            
            // Loop all entities that the system is interested in
            for (auto entity: GetSystemEntities()){
                const auto transform = entity.GetComponent<TransformComponent>();
                const auto sprite = entity.GetComponent<SpriteComponent>();
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
