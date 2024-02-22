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

            //for (auto entity: GetSystemEntities()){
            //    RenderableEntity renderableEntity;
            //    renderableEntity.transformComponent = entity.GetComponent<TransformComponent>();
            //    renderableEntity.spriteComponent = entity.GetComponent<SpriteComponent>();
            //
            //    renderableEntities.emplace_back(renderableEntity);
            //
            //    std::sort(renderableEntities.begin(), renderableEntities.end(), [](const RenderableEntity& a, const RenderableEntity& b){
            //        return a.spriteComponent.zIndex < b.spriteComponent.zIndex;
            //    });
            //}
           
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
