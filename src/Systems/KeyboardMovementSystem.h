#pragma once

#include "../ECS/ECS.h"
#include "../Logger/Logger.h"
#include "../EventBus/EventBus.h"
#include "../Events/KeyPressedEvent.h"
#include "../Events/KeyReleasedEvent.h"
#include "../Components/SpriteComponent.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/KeyboardControlComponent.h"


class KeyboardMovementSystem: public System {
    public:
        KeyboardMovementSystem() {
            RequireComponent<SpriteComponent>();
            RequireComponent<RigidBodyComponent>();
            RequireComponent<KeyboardControlComponent>();
        }
        
        void SubscribeToEvents(std::unique_ptr<EventBus>& eventBus) {
            eventBus -> SubscribeToEvent<KeyPressedEvent>(this, &KeyboardMovementSystem::onKeyPress);
            eventBus -> SubscribeToEvent<KeyReleasedEvent>(this, &KeyboardMovementSystem::onKeyRelease);
        }

        void onKeyPress(KeyPressedEvent& event) {
            for (auto entity: GetSystemEntities()) {
                const auto keyboardControl = entity.GetComponent<KeyboardControlComponent>();
                auto& sprite = entity.GetComponent<SpriteComponent>();
                auto& rigidBody = entity.GetComponent<RigidBodyComponent>();
                
                if (event.key == "Up") {
                    rigidBody.velocity.y = -keyboardControl.speed;
                    sprite.srcRect.y = sprite.height * 0;
                }
                
                if (event.key == "Right") {
                    rigidBody.velocity.x = keyboardControl.speed;
                    sprite.srcRect.y = sprite.height * 1;
                }
                
                if (event.key == "Down") {
                    rigidBody.velocity.y = keyboardControl.speed;
                    sprite.srcRect.y = sprite.height * 2;
                }
                
                if (event.key == "Left") {
                    rigidBody.velocity.x = -keyboardControl.speed;
                    sprite.srcRect.y = sprite.height * 3;
                }
            }
        }

        void onKeyRelease(KeyReleasedEvent& event) {
            for (auto entity: GetSystemEntities()){
                auto& rigidBody = entity.GetComponent<RigidBodyComponent>();
                //auto& sprite = entity.GetComponent<SpriteComponent>();

                if (event.key == "Up" && rigidBody.velocity.y < 0) rigidBody.velocity.y = 0;
                if (event.key == "Right" && rigidBody.velocity.x > 0) rigidBody.velocity.x = 0;
                if (event.key == "Down" && rigidBody.velocity.y > 0) rigidBody.velocity.y = 0;
                if (event.key == "Left" && rigidBody.velocity.x < 0) rigidBody.velocity.x = 0;
                
                //if (event.key == "Up" && rigidBody.velocity.y < 0){
                //    rigidBody.velocity.y = 0;
                //    if (rigidBody.velocity.x > 0) sprite.srcRect.y = sprite.height * 1;
                //    if (rigidBody.velocity.x < 0) sprite.srcRect.y = sprite.height * 3;
                //} 
                //if (event.key == "Right" && rigidBody.velocity.x > 0){
                //    rigidBody.velocity.x = 0;
                //    if (rigidBody.velocity.y > 0) sprite.srcRect.y = sprite.height * 2;
                //    if (rigidBody.velocity.y < 0) sprite.srcRect.y = sprite.height * 0;
                //} 
                //if (event.key == "Down" && rigidBody.velocity.y > 0){
                //    rigidBody.velocity.y = 0;
                //    if (rigidBody.velocity.x > 0) sprite.srcRect.y = sprite.height * 1;
                //    if (rigidBody.velocity.x < 0) sprite.srcRect.y = sprite.height * 3;
                //} 
                //if (event.key == "Left" && rigidBody.velocity.x < 0){
                //    rigidBody.velocity.x = 0;
                //    if (rigidBody.velocity.y > 0) sprite.srcRect.y = sprite.height * 2;
                //    if (rigidBody.velocity.y < 0) sprite.srcRect.y = sprite.height * 0;
                //} 
            }
        }

        void Update() {
        }
};
