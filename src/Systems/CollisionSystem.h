#pragma once

#include <SDL2/SDL.h>

#include <vector>
#include <unordered_map>
#include "../ECS/ECS.h"
#include "../Components/BoxColliderComponent.h"
#include "../Components/TransformComponent.h"
#include "../Logger/Logger.h"

struct BoundingBox {
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
            vector<BoundingBox> boxes(entities.size());

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
                
                BoundingBox aBoundingBox;
                aBoundingBox.entity = make_shared<Entity>(aEntity);
                aBoundingBox.box = aBox;
                
                // Loop over the vector of boxes - check for collisions and log
                for (int i = 0; i < (int)boxes.size(); i++){
                    shared_ptr<Entity> bEntity = boxes[i].entity;
                    SDL_Rect bBox = boxes[i].box;
                    if (SDL_HasIntersection(&aBox, &bBox)){
                        auto& bCollider = bEntity -> GetComponent<BoxColliderComponent>();
                        aCollider.isColliding = true;
                        bCollider.isColliding = true;
                        Logger::Log("Collision between entity " + to_string(aEntity.GetId()) + " and entity " + to_string(bEntity -> GetId()));
                    }
                }
               
                // Push the bounding box into the cache vector
                boxes.push_back(aBoundingBox);
            }
        }
};
