#pragma once

#include <vector>
#include <bitset>
#include <set>
#include <deque>
#include <unordered_map>
#include <typeindex>
#include <memory>
#include "../Logger/Logger.h"

using namespace std;

//--------SIGNATURE
// We use a bitset (1s and 0s) to keep track of which components an entity has
// and also helps keep track of which entities a system is interested in.
const unsigned int MAX_COMPONENTS = 32;
typedef bitset<MAX_COMPONENTS> Signature;

//--------ENTITY
class Entity{
    private:
        int id;

    public:
        Entity(int id): id(id){};
        Entity(const Entity& entity) = default;
        void Kill();
        int GetId() const;
        // Custom operator to check if two entities are equal to one another
        bool operator ==(const Entity& entity) const { return GetId() == entity.GetId(); }
        bool operator <(const Entity& entity) const { return GetId() < entity.GetId(); }

        template <typename TComponent, typename ...TArgs> void AddComponent(TArgs&& ...args);
        template <typename TComponent> void RemoveComponent();
        template <typename TComponent> bool  HasComponent() const;
        template <typename TComponent> TComponent& GetComponent() const;

        //Hold a pointer to the entity's owner registr
        class Registry* registry;
};

//--------COMPONENT
struct IComponent{
    protected:
        static int nextId;
};

// Assign a unique ID to each component type
template<typename T>
class Component: public IComponent{
    public:
        // Returns the unique ID of Component<T>
        static int GetId(){
            static auto id = nextId++;
            return id;
    }
};

//--------SYSTEM 
// Processes entities that contain a specific signature
class System{
    private:
        Signature componentSignature;
        vector<Entity> entities;
    public:
        System() = default;
        virtual ~System() = default;
        
        void AddEntityToSystem(Entity entity);
        void RemoveEntityFromSystem(Entity entity);
        const vector<Entity>& GetSystemEntities() const;
        const Signature& GetComponentSignature() const;
        
        // Defines the component type that entities must have to be considered by the system
        template <typename TComponent> void RequireComponent(); 

};


//--------POOL
// A pool is just a vector of objects of type T
class IPool{
    public:
        virtual ~IPool(){}
};
template <typename T>
class Pool: public IPool{
    private:
        vector<T> data;
    public:
        Pool(int size = 100){
            data.resize(size);
        }
        virtual ~Pool() = default;
        
        bool isEmpty() const{
            return data.empty();
        }

        int getSize() const{
            return data.size();
        }

        void Resize(int n){
            data.resize(n);
        }

        void Clear(){
            data.clear();
        }

        void Add(T object){
            data.push_back(object);
        }

        void Set(int index, T object){
            data[index] = object;
        }

        T& Get(int index){
            return static_cast<T&>(data[index]);
        }

        T& operator [](unsigned int index){
            return data[index];
        }
};

//--------REGISTRY
// Manages the creation and destruction of entities, systems, and components.
class Registry{
    private:
        int numEntities = 0;
                
        // Vector of component pools
        // Each pool contains all the data for a certain component type
        // [index = component type id]
        // [Pool index = entity id]
        vector<shared_ptr<IPool>> componentPools;

        // Vector of component signatures
        // The signatures let us know which components are 'on' for an entity
        // [index = entity id]
        vector<Signature> entityComponentSignatures;

        // Map of active systems 
        // [index = system typeId]
        unordered_map<type_index, shared_ptr<System>> systems;
        
        // Set of entities that are flagged to be added or removed in the next registry update()
        set<Entity> entitiesToBeAdded;
        set<Entity> entitiesToBeKilled;

        // List of available entity IDs from previously removed entities
        deque<int> freeIds;

    public:
        Registry() = default;

        void Update();
        
        // Entity management        
        Entity CreateEntity(); 
        void KillEntity(Entity entity);
        
        // Component management
        template <typename TComponent, typename ...TArgs> void AddComponent(Entity entity, TArgs&& ...args);
        template <typename TComponent> void RemoveComponent(Entity entity);
        template <typename TComponent> bool HasComponent(Entity entity) const;
        template <typename TComponent> TComponent& GetComponent(Entity entity) const;
        
        // System management
        template <typename TSystem, typename ...TArgs> void AddSystem(TArgs&& ...args);
        template <typename TSystem> void RemoveSystem();
        template <typename TSystem> bool HasSystem() const;  
        template <typename TSystem> TSystem& GetSystem() const;  
        
        // Add and remove entities from systems based on component signatures
        void AddEntityToSystems(Entity entity);
        void RemoveEntityFromSystems(Entity entity);

};

//--------TEMPLATES

// Entity
template<typename TComponent, typename ...TArgs>
void Entity::AddComponent(TArgs&& ...args){
    registry -> AddComponent<TComponent>(*this, forward<TArgs>(args)...);
}

template<typename TComponent>
void Entity::RemoveComponent(){
    registry -> RemoveComponent<TComponent>(*this);
}

template<typename TComponent>
bool Entity::HasComponent() const{
    return registry -> HasComponent<TComponent>(*this);
}

template <typename TComponent> 
TComponent& Entity::GetComponent() const{
    return registry -> GetComponent<TComponent>(*this);
}

// System
template <typename TComponent>
void System::RequireComponent(){
    const auto componentId = Component<TComponent>::GetId();
    componentSignature.set(componentId);
}

// Registry
template<typename TComponent, typename ...TArgs>
void Registry::AddComponent(Entity entity, TArgs&& ...args){
    const auto componentId = Component<TComponent>::GetId();
    const auto entityId = entity.GetId();
    
    // Resize componentPools vector if needed
    if(componentId >= static_cast<int>(componentPools.size())){
        componentPools.resize(componentId + 1, nullptr);
    }

    // Add pool for given component type if it doesn't exist
    if(!componentPools[componentId]){
        shared_ptr<Pool<TComponent>> newComponentPool = make_shared<Pool<TComponent>>();
        componentPools[componentId] = newComponentPool;
    }

    // Get the pool of component values for the given component type
    shared_ptr<Pool<TComponent>> componentPool = static_pointer_cast<Pool<TComponent>>(componentPools[componentId]);

    // Resize componentPool vector if needed
    if(entityId >= componentPool -> getSize()){
        componentPool -> Resize(numEntities);
    }

    // Create a new component object of the type T, and forward the various parameters to the constructor
    TComponent newComponent(forward<TArgs>(args)...);

    // Add the new component to the component pool list, using the entityId as index
    componentPool -> Set(entityId, newComponent);

    // Finally, change the component signature of the entity and set the component id on the bitset to 1
    entityComponentSignatures[entityId].set(componentId);

    Logger::Log("Component ID = " + to_string(componentId) + " was added to entity ID " + to_string(entityId));
}

template<typename TComponent>
void Registry::RemoveComponent(Entity entity){
    const auto componentId = Component<TComponent>::GetId();
    const auto entityId = entity.GetId();

    entityComponentSignatures[entityId].set(componentId, false);
    
    Logger::Log("Component ID = " + to_string(componentId) + " was removed from entity ID " + to_string(entityId));
}

template<typename TComponent>
bool Registry::HasComponent(Entity entity) const{
    const auto componentId = Component<TComponent>::GetId();
    const auto entityId = entity.GetId();

    return entityComponentSignatures[entityId].test(componentId); 
}

template <typename TComponent> 
TComponent& Registry::GetComponent(Entity entity) const{
    const auto componentId = Component<TComponent>::GetId();
    const auto entityId = entity.GetId();
    auto componentPool = static_pointer_cast<Pool<TComponent>>(componentPools[componentId]);
    return componentPool -> Get(entityId);
}

template <typename TSystem, typename ...TArgs> 
void Registry::AddSystem(TArgs&& ...args){
    // Create a new system using the constructor 
    shared_ptr<TSystem> newSystem = make_shared<TSystem>(forward<TArgs>(args)...);
    // Add a new key/value pair to the unordered map of systems
    systems.insert(make_pair(type_index(typeid(TSystem)), newSystem));

}

template <typename TSystem> 
void Registry::RemoveSystem(){
    auto system = systems.find(type_index(typeid(TSystem)));   
    systems.erase(system);
}

template <typename TSystem> 
bool Registry::HasSystem() const{
    return systems.find(type_index(typeid(TSystem))) != systems.end(); 
}

template <typename TSystem> 
TSystem& Registry::GetSystem() const{
    auto system = systems.find(type_index(typeid(TSystem)));
    // Get the value from the key as static pointer cast of the type TSystem
    return *(static_pointer_cast<TSystem>(system -> second));    
}

