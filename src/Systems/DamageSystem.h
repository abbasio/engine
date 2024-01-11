#pragma once

#include "../ECS/ECS.h"
#include "../Logger/Logger.h"
#include "../Components/BoxColliderComponent.h"
#include "../EventBus/EventBus.h"
#include "../Events/CollisionEvent.h"
class DamageSystem: public System {
    public:
        DamageSystem() {
            RequireComponent<BoxColliderComponent>();
        }
        
        void SubscribeToEvents(std::unique_ptr<EventBus>& eventBus) {
            eventBus -> SubscribeToEvent<CollisionEvent>(this, &DamageSystem::onCollision);
        }

        void onCollision(CollisionEvent& event) {
            Logger::Log("Collision between entity " + to_string(event.a.GetId()) + " and entity " + to_string(event.b.GetId()));
            event.b.Kill();
        }

        void Update() {
            //TODO
        }
};
