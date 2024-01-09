#pragma once

#include <SDL2/SDL.h>

#include <unordered_map>
#include "../ECS/ECS.h"
#include "../Components/BoxColliderComponent.h"
#include "../Components/TransformComponent.h"
#include "../Logger/Logger.h"

class CollisionSystem: public System{
    public:
        CollisionSystem(){
            RequireComponent<TransformComponent>();
            RequireComponent<BoxColliderComponent>();
        }

        void Update(){
            // Create a map to cache bounding boxes: key = entityId (int), value = bounding box (SDL_Rect)  
            unordered_map<int, SDL_Rect> entityBoxes; 
            
            // Loop over system entities
            for (auto entity: GetSystemEntities()){
                const auto collider = entity.GetComponent<BoxColliderComponent>();
                const auto transform = entity.GetComponent<TransformComponent>();
                
                // Create a bounding box for the current entity
                SDL_Rect boxA {
                    (int)transform.position.x,
                    (int)transform.position.y,
                    collider.width,
                    collider.height
                };
                
                // Loop over the map - check for collisions and log 
                for(const std::pair<const int, SDL_Rect>& boxB : entityBoxes){
                    if(SDL_HasIntersection(&boxA, &boxB.second)){
                        Logger::Log("Collision between entity " + to_string(entity.GetId()) + " and entity " + to_string(boxB.first));
                    } 
                }
                
                // Push the key/value pair of current entity and current bounding box into the map
                entityBoxes.insert({entity.GetId(), boxA}); 
            }
        }
};
