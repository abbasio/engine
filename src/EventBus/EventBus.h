#pragma once

#include "../Logger/Logger.h"
#include "Event.h"
#include <map>
#include <list>
#include <memory>
#include <typeindex>

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
template <typename TOwner, typename TEvent>
class EventCallback: public IEventCallback {
    private:
        typedef void (TOwner::*CallbackFunction)(TEvent&);
        
        // Get function pointer + context owner
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

        template <typename TEvent, typename TOwner>
        void SubscribeToEvent(TOwner* ownerInstance, void (TOwner::*callbackFunction)(TEvent&)){
            auto handlers = subscribers[typeid(TEvent)];
            if(!handlers.get()) handlers = make_unique<HandlerList>();
            
            // Create the wrapper around the function pointer to be called on event trigger
            auto subscriber = make_unique<EventCallback<TOwner, TEvent>>(ownerInstance, callbackFunction);
            // Add the callback function to the handler list
            // Requires changing ownership since it's a unique pointer
            handlers -> push_back(move(subscriber));     
        }
        
        template <typename TEvent, typename ...TArgs>
        void EmitEvent(TArgs&& ...args){
            auto handlers = subscribers[typeid(TEvent)].get();
            // Loop over all function callbacks for the emitted event and execute them
            // Pass the parameters (TArgs) into each callback
            for (auto it = handlers -> begin(); it != handlers -> end(); it++){
                auto handler = it -> get();
                TEvent event(forward<TArgs>(args)...);
                handler -> Execute(event);
            }
        }
};
