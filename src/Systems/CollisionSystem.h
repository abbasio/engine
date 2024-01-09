#pragma once

#include <unordered_map>
#include "../ECS/ECS.h"
#include "../Components/BoxColliderComponent.h"
#include "../Components/TransformComponent.h"
#include "../Logger/Logger.h"

// Struct holding the required data for calculating collisions
struct BoundingBox{
    int x;
    int y;
    int width;
    int height;
};

class CollisionSystem: public System{
    public:
        CollisionSystem(){
            RequireComponent<TransformComponent>();
            RequireComponent<BoxColliderComponent>();
        }

        void Update(){
            // Create a map: key = entityId, value = BoundingBox 
            unordered_map<int, BoundingBox> entityBoxes; 
            
            // Loop over system entities
            for (auto entity: GetSystemEntities()){
                const auto collider = entity.GetComponent<BoxColliderComponent>();
                const auto transform = entity.GetComponent<TransformComponent>();
                
                // Create a bounding box for the current entity
                BoundingBox box;
                box.x = transform.position.x;
                box.y = transform.position.y;
                box.width = transform.position.x + collider.width;
                box.height = transform.position.y + collider.height;
                
                // Loop over the map - check for collisions and log 
                for(const std::pair<const int, BoundingBox>& mapBox : entityBoxes){
                    if(
                        box.x < mapBox.second.width &&
                        box.width > mapBox.second.x &&
                        box.y < mapBox.second.height &&
                        box.height > mapBox.second.y
                    ) Logger::Log("Collision between entity " + to_string(entity.GetId()) + " and entity " + to_string(mapBox.first));
                }
                
                // Push the key/value pair of current entity and current bounding box into the map
                entityBoxes[entity.GetId()] = box; 
            }
        }
};
