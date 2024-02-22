#pragma once

#include "../ECS/ECS.h"
#include "../EventBus/EventBus.h"
#include "../Events/CollisionEvent.h"
#include "../Components/TransformComponent.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/SpriteComponent.h"

class MovementSystem: public System{
    public: 
        MovementSystem(){
            RequireComponent<TransformComponent>();
            RequireComponent<RigidBodyComponent>();
        }
        
        void SubscribeToEvents(std::unique_ptr<EventBus>& eventBus) {
            eventBus -> SubscribeToEvent<CollisionEvent>(this, &MovementSystem::onCollision);
        }

        void onCollision(CollisionEvent& event) {
            Entity a = event.a;
            Entity b = event.b;

            if(a.BelongsToGroup("enemies") && b.BelongsToGroup("obstacles")) {
                onEnemyHitsObstacle(a, b);
            }
            
            if(a.BelongsToGroup("obstacles") && b.BelongsToGroup("enemies")) {
                onEnemyHitsObstacle(b, a);
            }
        }

        void onEnemyHitsObstacle(Entity enemy, Entity obstacle){
            if (enemy.HasComponent<RigidBodyComponent>() && enemy.HasComponent<SpriteComponent>()){
                auto& rigidBody = enemy.GetComponent<RigidBodyComponent>();
                auto& sprite = enemy.GetComponent<SpriteComponent>();

                if (rigidBody.velocity.x != 0){
                    rigidBody.velocity.x *= -1;
                    sprite.flip = (sprite.flip == SDL_FLIP_NONE) ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
                }
                
                if (rigidBody.velocity.y != 0){
                    rigidBody.velocity.y *= -1;
                    sprite.flip = (sprite.flip == SDL_FLIP_NONE) ? SDL_FLIP_VERTICAL : SDL_FLIP_NONE;
                }
            }
        }

        void Update(double deltaTime){
            //Loop all entities that the system is interested in
            for (auto entity: GetSystemEntities()){
                auto& transform = entity.GetComponent<TransformComponent>();
                const auto rigidbody = entity.GetComponent<RigidBodyComponent>();   
                
                transform.position.x += rigidbody.velocity.x * deltaTime;  
                transform.position.y += rigidbody.velocity.y * deltaTime;

                bool isOutsideMap = (
                    transform.position.x < 0 ||
                    transform.position.x > Game::mapWidth ||
                    transform.position.y < 0 ||
                    transform.position.y > Game::mapHeight      
                );

                if (isOutsideMap && !entity.HasTag("player")) entity.Kill();
            }
        }  

};
