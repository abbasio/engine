#pragma once

#include <SDL2/SDL.h>

#include "../ECS/ECS.h"
#include "../Components/ProjectileEmitterComponent.h"
#include "../Components/BoxColliderComponent.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/TransformComponent.h"
#include "../Components/SpriteComponent.h"

class ProjectileEmitSystem: public System {
    public:
        ProjectileEmitSystem() {
            RequireComponent<ProjectileEmitterComponent>();
            RequireComponent<TransformComponent>();
        }

        void Update(std::unique_ptr<Registry>& registry) {
            for (auto entity: GetSystemEntities()){
                auto& projectileEmitter = entity.GetComponent<ProjectileEmitterComponent>();
                const auto transform = entity.GetComponent<TransformComponent>();
                
                //TODO: Check if it's time to emit a new projectile
                if (SDL_GetTicks() - projectileEmitter.lastFiredTime > projectileEmitter.projectileFrequency){
                    glm::vec2 projectilePosition = transform.position;
                    if(entity.HasComponent<SpriteComponent>()){
                        const auto sprite = entity.GetComponent<SpriteComponent>();
                        projectilePosition.x += (transform.scale.x * sprite.width / 2);
                        projectilePosition.y += (transform.scale.y * sprite.height / 2);

                    }
                    // Add a new projectile entity to the registry
                    Entity projectile = registry -> CreateEntity();
                    projectile.AddComponent<TransformComponent>(projectilePosition, glm::vec2(1.0, 1.0), 0.0);
                    projectile.AddComponent<RigidBodyComponent>(projectileEmitter.projectileVelocity);
                    projectile.AddComponent<SpriteComponent>("bullet-image", 4, 4, 4);
                    projectile.AddComponent<BoxColliderComponent>(4, 4);

                    projectileEmitter.lastFiredTime = SDL_GetTicks();
                }
            }
        }
};
