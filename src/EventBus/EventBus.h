#pragma once

#include "../Logger/Logger.h"
#include "Event.h"

#include <functional>
#include <typeindex>
#include <memory>
#include <list>
#include <map>

using namespace std;

// Parent class
class IEventCallback{
    private:
        virtual void Call(Event& e) = 0; 
    public:
        virtual ~IEventCallback() = default;

        void Execute(Event& e){
            Call(e);
        }
};

// Wrapper around a function pointer
// We need the owner so we can execute the function in the correct context
template <typename TOwner, typename TEvent>
class EventCallback: public IEventCallback {
    private:
        typedef void (TOwner::*CallbackFunction)(TEvent&);
        
        // Get context owner + pointer to function
        TOwner* ownerInstance;
        CallbackFunction callbackFunction;

        virtual void Call(Event& e) override{
            // Invoke a member function
            // callbackFunction = function to execute
            // ownerInstance = parent class, context to execute in
            // e = argument for callbackFunction
            invoke(callbackFunction, ownerInstance, static_cast<TEvent&>(e));
        }

        public:
            EventCallback(TOwner* ownerInstance, CallbackFunction callbackFunction){
                this -> ownerInstance = ownerInstance;
                this -> callbackFunction = callbackFunction;
            }

            virtual ~EventCallback() override = default;
              
};

using HandlerList = list<unique_ptr<IEventCallback>>;

class EventBus {
    private:
        // key = event type, value = list of functions to be executed per event
        map<type_index, unique_ptr<HandlerList>> subscribers;

    public:
        EventBus(){
            Logger::Log("EventBus constructor called!");
        }
        
        ~EventBus(){
            Logger::Log("EventBus destructor called!");
        }

        // Clear subscriber list
        void Reset() {
            subscribers.clear();
        } 

        
        // Subscribe to an event type <T>
        // Example: eventBus -> SubscribeToEvent<CollisionEvent>(this, &Game::onCollision);
        // This will put the onCollision function into the list of handlers for the event type CollisionEvent
        template <typename TEvent, typename TOwner>
        void SubscribeToEvent(TOwner* ownerInstance, void (TOwner::*callbackFunction)(TEvent&)){
            if (!subscribers[typeid(TEvent)].get()) {
                subscribers[typeid(TEvent)] = make_unique<HandlerList>();
            }
            
            // Create the wrapper around the function pointer to be called on event trigger
            auto subscriber = make_unique<EventCallback<TOwner, TEvent>>(ownerInstance, callbackFunction);
            // Add the callback function to the handler list
            // Requires changing ownership since it's a unique pointer
            subscribers[typeid(TEvent)] -> push_back(move(subscriber));     
        }
        
        // Emit an event of type <T>
        // As soon as an event is emitted, we execute all listener callback functions
        // Example: eventBus -> EmitEvent<CollisionEvent>(player, enemy);
        // This will execute all functions in the list of handlers for event type CollisionEvent
        // Handler functions will be executed with arguments (player, enemy)
        template <typename TEvent, typename ...TArgs>
        void EmitEvent(TArgs&& ...args){
            auto handlers = subscribers[typeid(TEvent)].get();
            // Define the event: The parameters that the handler functions will be called with
            TEvent event(forward<TArgs>(args)...);
            if (handlers) {
                // Loop over all handlers and execute them with the event parameters
                for (auto it = handlers -> begin(); it != handlers -> end(); it++){
                    auto handler = it -> get();
                    handler -> Execute(event);
                }
            }
        }
};
