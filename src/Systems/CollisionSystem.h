#pragma once

#include "../ECS/ECS.h"
#include "../Components/BoxColliderComponent.h"
#include "../Components/TransformComponent.h"
class CollisionSystem: public System{
    public:
        CollisionSystem(){
            RequireComponent<TransformComponent>();
            RequireComponent<BoxColliderComponent>();
        }

        void Update(){
            //TODO: 
            // Check all entities with box collider components
            // see if they are colliding with each other 
        }
};
