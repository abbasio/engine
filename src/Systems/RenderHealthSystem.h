#pragma once

#include <SDL2/SDL.h>
#include <glm/glm.hpp>

#include "../ECS/ECS.h"
#include "../AssetStore/AssetStore.h"
#include "../Components/HealthComponent.h"
#include "../Components/TransformComponent.h"


class RenderHealthSystem: public System {
    public: 
        RenderHealthSystem() {
            RequireComponent<HealthComponent>();
            RequireComponent<TransformComponent>(); 
        }

        void Update(SDL_Renderer* renderer, std::unique_ptr<AssetStore>& assetStore, SDL_Rect& camera){
            for (auto entity: GetSystemEntities()){
                const auto transform = entity.GetComponent<TransformComponent>();
                const auto health = entity.GetComponent<HealthComponent>();
               
                // Draw a filled rectangle for the health bar
                SDL_Rect healthBar = {
                    (int)transform.position.x - camera.x,
                    (int)transform.position.y - 10 - camera.y,
                    32 * health.healthPercentage / 100,
                    5
                };
                                
                SDL_Color healthBarColor;
                switch (health.healthPercentage) {
                    case 71 ... 100:
                        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
                        healthBarColor = {0, 255, 0};
                        break;
                    case 31 ... 70:
                        SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
                        healthBarColor = {255, 255, 0};
                        break;
                    case 0 ... 30:
                        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
                        healthBarColor = {255, 0, 0};
                        break;
                }
                
                SDL_RenderFillRect(renderer, &healthBar);
                
                // Draw the health percentage as text 
                std::string healthText = std::to_string(health.healthPercentage) + "%";

                SDL_Surface* surface = TTF_RenderText_Blended(assetStore -> GetFont("kitchensink_font"), healthText.c_str(), healthBarColor); 
                SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
                SDL_FreeSurface(surface);

                int healthTextWidth = 0;
                int healthTextHeight = 0;

                SDL_QueryTexture(texture, NULL, NULL, &healthTextWidth, &healthTextHeight);

                SDL_Rect destRect = {
                    (int)transform.position.x - camera.x + (32 * health.healthPercentage / 100) + 10,
                    (int)transform.position.y - 10 - camera.y,
                    healthTextWidth,
                    healthTextHeight
                };
                
                SDL_RenderCopy(renderer, texture, NULL, &destRect);
                SDL_DestroyTexture(texture);

            }
        }
};
