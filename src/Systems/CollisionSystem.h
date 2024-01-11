#pragma once

#include <SDL2/SDL.h>

#include <vector>
#include <unordered_map>
#include "../ECS/ECS.h"
#include "../Components/BoxColliderComponent.h"
#include "../Components/TransformComponent.h"
#include "../Logger/Logger.h"

struct EntityBox {
    shared_ptr<Entity> entity;
    SDL_Rect box;
};

class CollisionSystem: public System{
    public:
        CollisionSystem(){
            RequireComponent<TransformComponent>();
            RequireComponent<BoxColliderComponent>();
        }

        void Update(){
            // Create a vector to cache bounding boxes
            vector<Entity> entities = GetSystemEntities();
            vector<EntityBox> entityBoxes(entities.size());

            // Loop over system entities
            for (auto aEntity: entities){
                auto& aCollider = aEntity.GetComponent<BoxColliderComponent>();
                const auto aTransform = aEntity.GetComponent<TransformComponent>();
               
                aCollider.isColliding = false;

                // Create a bounding box for the current entity
                SDL_Rect aBox {
                    (int)aTransform.position.x + ((int)aCollider.offset.x * (int)aTransform.scale.x),
                    (int)aTransform.position.y + ((int)aCollider.offset.y * (int)aTransform.scale.y),
                    aCollider.width * (int)aTransform.scale.x,
                    aCollider.height * (int)aTransform.scale.y
                };
                
                EntityBox aEntityBox;
                aEntityBox.entity = make_shared<Entity>(aEntity);
                aEntityBox.box = aBox;
                
                // Loop over the vector of boxes - check for collisions and log
                for (auto bEntityBox: entityBoxes){
                    shared_ptr<Entity> bEntity = bEntityBox.entity;
                    SDL_Rect bBox = bEntityBox.box;
                    if (SDL_HasIntersection(&aBox, &bBox)){
                        auto& bCollider = bEntity -> GetComponent<BoxColliderComponent>();
                        aCollider.isColliding = true;
                        bCollider.isColliding = true;
                        
                        // Emit an event
                        
                        Logger::Log("Collision between entity " + to_string(aEntity.GetId()) + " and entity " + to_string(bEntity -> GetId()));
                    }
                }
               
                // Push the bounding box into the cache vector
                entityBoxes.push_back(aEntityBox);
            }
        }
};
