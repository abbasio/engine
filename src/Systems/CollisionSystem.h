#pragma once

#include <SDL2/SDL.h>

#include <vector>
#include <unordered_map>
#include "../ECS/ECS.h"
#include "../Components/BoxColliderComponent.h"
#include "../Components/TransformComponent.h"
#include "../Logger/Logger.h"

struct BoundingBox {
    int entityId;
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
            for (auto entity: entities){
                int entityId = entity.GetId();
                const auto collider = entity.GetComponent<BoxColliderComponent>();
                const auto transform = entity.GetComponent<TransformComponent>();
                
                // Create a bounding box for the current entity
                SDL_Rect boxA {
                    (int)transform.position.x,
                    (int)transform.position.y,
                    collider.width,
                    collider.height
                };
                
                BoundingBox boundingBox;
                boundingBox.entityId = entityId;
                boundingBox.box = boxA;
                
                // Loop over the vector of boxes - check for collisions and log
                for (int i = 0; i < (int)boxes.size(); i++){
                    SDL_Rect boxB = boxes[i].box;
                    if (SDL_HasIntersection(&boxA, &boxB)){
                        Logger::Log("Collision between entity " + to_string(entityId) + " and entity " + to_string(boxes[i].entityId));
                    }
                }
               
                // Push the bounding box into the cache vector
                boxes.push_back(boundingBox);
            }
        }
};
