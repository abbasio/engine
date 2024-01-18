#pragma once

#include <SDL2/SDL.h>

#include "../ECS/ECS.h"
#include "../EventBus/EventBus.h"
#include "../Components/ProjectileEmitterComponent.h"
#include "../Components/BoxColliderComponent.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/TransformComponent.h"
#include "../Components/LifecycleComponent.h"
#include "../Components/SpriteComponent.h"
#include "../Components/DamageComponent.h"

#include "../Events/KeyPressedEvent.h"

class ProjectileEmitSystem: public System {
    public:
        ProjectileEmitSystem() {
            RequireComponent<ProjectileEmitterComponent>();
            RequireComponent<TransformComponent>();
        }
        
         
        void SubscribeToEvents(std::unique_ptr<EventBus>& eventBus) {
            eventBus -> SubscribeToEvent<KeyPressedEvent>(this, &ProjectileEmitSystem::onKeyPress);
        }

        void onKeyPress(KeyPressedEvent& event) {
            if (event.key == "Space") {
                for (auto entity: GetSystemEntities()) {
                    auto& projectileEmitter = entity.GetComponent<ProjectileEmitterComponent>();
                    uint32_t now = SDL_GetTicks();
                    if (!projectileEmitter.isAuto && (int)(now - projectileEmitter.lastFiredTime) > projectileEmitter.projectileFrequency) {
                        const auto transform = entity.GetComponent<TransformComponent>();
                        glm::vec2 projectilePosition = transform.position;
                        glm::vec2 projectileVelocity = projectileEmitter.projectileVelocity;
                        if(entity.HasComponent<SpriteComponent>()){
                            const auto sprite = entity.GetComponent<SpriteComponent>();

                            projectilePosition.x += (transform.scale.x * sprite.width / 2);
                            projectilePosition.y += (transform.scale.y * sprite.height / 2);

                            if (sprite.direction == "Up"){
                                projectileVelocity.y *= -1;
                                projectileVelocity.x = 0;
                            }
                            if (sprite.direction == "Right") projectileVelocity.y = 0;
                            if (sprite.direction == "Down") projectileVelocity.x = 0;
                            if (sprite.direction == "Left"){
                                projectileVelocity.x *= -1;
                                projectileVelocity.y = 0;
                            }
                            
                        }
                        // Add a new projectile entity to the registry
                        Entity projectile = entity.registry -> CreateEntity();
                        projectile.Group("projectiles");
                        projectile.AddComponent<TransformComponent>(projectilePosition, glm::vec2(1.0, 1.0), 0.0);
                        projectile.AddComponent<RigidBodyComponent>(projectileVelocity);
                        projectile.AddComponent<SpriteComponent>("bullet-image", 4, 4, 4);
                        projectile.AddComponent<BoxColliderComponent>(4, 4, 1);
                        // Set something on the projectile emitter component to control lifecycle instead of hardcoding
                        projectile.AddComponent<LifecycleComponent>(5000);
                        projectile.AddComponent<DamageComponent>(projectileEmitter.projectileDamage);

                        projectileEmitter.lastFiredTime = now;
                    }
                }
            }
        } 
         
        void Update(std::unique_ptr<Registry>& registry) {
            for (auto entity: GetSystemEntities()){
                auto& projectileEmitter = entity.GetComponent<ProjectileEmitterComponent>();
                const auto transform = entity.GetComponent<TransformComponent>();
                uint32_t now = SDL_GetTicks();
                // Check if it's time to emit a new projectile
                if (projectileEmitter.isAuto && (int)(now - projectileEmitter.lastFiredTime) > projectileEmitter.projectileFrequency) {
                    glm::vec2 projectilePosition = transform.position;
                    if(entity.HasComponent<SpriteComponent>()){
                        const auto sprite = entity.GetComponent<SpriteComponent>();
                        projectilePosition.x += (transform.scale.x * sprite.width / 2);
                        projectilePosition.y += (transform.scale.y * sprite.height / 2);

                    }
                    // Add a new projectile entity to the registry
                    Entity projectile = registry -> CreateEntity();
                    projectile.Group("projectiles");
                    projectile.AddComponent<TransformComponent>(projectilePosition, glm::vec2(1.0, 1.0), 0.0);
                    projectile.AddComponent<RigidBodyComponent>(projectileEmitter.projectileVelocity);
                    projectile.AddComponent<SpriteComponent>("bullet-image", 4, 4, 4);
                    projectile.AddComponent<BoxColliderComponent>(4, 4, 2);
                    // Set something on the projectile emitter component to control lifecycle instead of hardcoding
                    projectile.AddComponent<LifecycleComponent>(5000);
                    projectile.AddComponent<DamageComponent>(projectileEmitter.projectileDamage);

                    projectileEmitter.lastFiredTime = now;
                    
                }
                
            }
        }
};
