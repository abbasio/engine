#pragma once

#include <set>
#include <deque>
#include <vector>
#include <bitset>
#include <memory>
#include <typeindex>
#include <unordered_map>

using namespace std;

//--------SIGNATURE
// We use a bitset (1s and 0s) to keep track of which components an entity has
// Also helps keep track of which entities a system is interested in.
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

        // Manage tags and groups
        void Tag(const string& tag);
        bool HasTag(const string& tag) const;
        void Group(const string& group);
        bool BelongsToGroup(const string& group) const;
        
        // Custom operators
        bool operator ==(const Entity& entity) const { return GetId() == entity.GetId(); }
        bool operator <(const Entity& entity) const { return GetId() < entity.GetId(); }

        template <typename TComponent, typename ...TArgs> void AddComponent(TArgs&& ...args);
        template <typename TComponent> void RemoveComponent();
        template <typename TComponent> bool  HasComponent() const;
        template <typename TComponent> TComponent& GetComponent() const;

        //Hold a pointer to the entity's owner registry
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
        vector<Entity>& GetSystemEntities();
        const Signature& GetComponentSignature() const;
        
        // Defines the component type that entities must have to be considered by the system
        template <typename TComponent> void RequireComponent(); 

};


//--------POOL
// A pool is just a vector of objects of type T
// Size: Actual number of elements in vector
// Returned by size() and affected by resize(new_size)
// Capacity: Number of elements that CAN be stored in vector (memory allocated)
// Returned by capacity() and affected by reserve(new_capacity)
class IPool{
    public:
        virtual ~IPool() = default;
        virtual void RemoveEntityFromPool(int entityId) = 0;
};
template <typename T>
class Pool: public IPool{
    private:
        vector<T> data;

        unordered_map<int, int> entityIdToIndex;
        unordered_map<int, int> indexToEntityId;
    public:
        Pool(int capacity = 100){
            data.reserve(capacity);
        }
        virtual ~Pool() = default;
        
        bool IsEmpty() const{
            return data.empty();
        }

        int GetSize() const{
            return data.size();
        }

        void Reserve(int n){
            data.reserve(n);
        }

        void Clear(){
            data.clear();
        }

        void Add(T object){
            data.push_back(object);
        }

        void Set(int entityId, T object){
            if (entityIdToIndex.find(entityId) != entityIdToIndex.end()) {
                // If the element already exists, just replace the component object
                int index = entityIdToIndex[entityId];
                data[index] = object;
            } else {
                // Using push_back() will add the value to the end of the vector
                // and automatically increase vector capacity if needed
                data.push_back(object);
                // When adding a new object, keep track of entity ID and vector index
                // The index at the end of the vector will be its size - 1 (for non-empty vectors)
                int index = data.size() - 1;
                entityIdToIndex.emplace(entityId, index);
                indexToEntityId.emplace(index, entityId);
            }
        }

        void Remove(int entityId) {
            // Copy the last element to the deleted position to keep the array packed
            // Then remove the last element
            int indexOfRemoved = entityIdToIndex[entityId];
            int indexOfLast = data.size() - 1;
            data[indexOfRemoved] = data[indexOfLast];
            data.pop_back();

            // Update the index-entity maps to point to the correct elements
            int entityIdOfLastElement = indexToEntityId[indexOfLast];
            entityIdToIndex[entityIdOfLastElement] = indexOfRemoved;
            indexToEntityId[indexOfRemoved] = entityIdOfLastElement;

            entityIdToIndex.erase(entityId);
            indexToEntityId.erase(indexOfLast);
        }

        void RemoveEntityFromPool(int entityId) override {
            if (entityIdToIndex.find(entityId) != entityIdToIndex.end()) Remove(entityId);
        }

        T& Get(int entityId){
            int index = entityIdToIndex[entityId];
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
        
        // Entity tags - one tag per entity
        // Map tag -> entity
        unordered_map<string, Entity> entityPerTag;
        // Map entityId -> tag
        unordered_map<int, string> tagPerEntity;

        // Entity groups - a set of entities per group name
        // Map group -> entities
        unordered_map<string, set<Entity>> entitiesPerGroup;
        // Map entityId -> group
        unordered_map<int, string> groupPerEntity;

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
        
        // A note on tags and groups:
        // Strings are not optimal to use for tagging and grouping entities
        // Keeping them for now, but would be better to use a data structure to map strings to unique numerical IDs
        // Values sorted inside said structure allows for binary search

        // Tag management
        void TagEntity(Entity entity, const string& tag);
        bool EntityHasTag(Entity entity, const string& tag) const;
        Entity GetEntityByTag(const string& tag) const;
        void RemoveEntityTag(Entity entity);

        // Group management
        void GroupEntity(Entity entity, const string& group);
        bool EntityBelongsToGroup(Entity entity, const string& group) const;
        vector<Entity> GetEntitiesByGroup(const string& group) const;
        void RemoveEntityGroup(Entity entity);       
        
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

    // Create a new component object of the type T, and forward the various parameters to the constructor
    TComponent newComponent(forward<TArgs>(args)...);

    // Add the new component to the component pool list, using the entityId as index
    componentPool -> Set(entityId, newComponent);

    // Finally, change the component signature of the entity and set the component id on the bitset to 1
    entityComponentSignatures[entityId].set(componentId);
}

template<typename TComponent>
void Registry::RemoveComponent(Entity entity){
    const auto componentId = Component<TComponent>::GetId();
    const auto entityId = entity.GetId();


    // Remove component from component list for that entity
    shared_ptr<Pool<TComponent>> componentPool = static_pointer_cast<Pool<TComponent>>(componentPools[componentId]);
    componentPool -> Remove(entityId);

    // Set the component signature for that entity to false
    entityComponentSignatures[entityId].set(componentId, false);
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

