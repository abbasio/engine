#pragma once

#include "../ECS/ECS.h"
#include "../Logger/Logger.h"
#include "../EventBus/EventBus.h"
#include "../Components/SpriteComponent.h"
#include "../Components/RigidBodyComponent.h"
#include "../Events/KeyPressedEvent.h"


class KeyboardMovementSystem: public System {
    public:
        KeyboardMovementSystem() {
            RequireComponent<SpriteComponent>();
            RequireComponent<RigidBodyComponent>();
        }
        
        void SubscribeToEvents(std::unique_ptr<EventBus>& eventBus) {
            eventBus -> SubscribeToEvent<KeyPressedEvent>(this, &KeyboardMovementSystem::onKeyPress);
        }

        void onKeyPress(KeyPressedEvent& event) {
            Logger::Log("Key pressed: " + event.key);
        }

        void Update() {
        }
};
