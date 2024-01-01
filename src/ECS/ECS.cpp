#include "ECS.h"
#include <vector>
#include "../Logger/Logger.h"

using namespace std;

int Entity::GetId() const{
    return id;
}

void System::AddEntityToSystem(Entity entity){
    entities.push_back(entity);
}

void System::RemoveEntityFromSystem(Entity entity){
    erase(entities, entity);
}

const Signature& System::GetComponentSignature() const{
    return componentSignature;
}

Entity Registry::CreateEntity(){
    int entityId;

    entityId = numEntities++;

    Entity entity(entityId);
    entitiesToBeAdded.insert(entity);

    Logger::Log("Entity created with id = " + to_string(entityId));
    
    return entity;
}

template<typename T, typename ...TArgs>
void Registry::AddComponent(Entity entity, TArgs&& ...args){
    const auto componentId = Component<T>::GetId();
    const auto entityId = entity.GetId();
    
    // Resize componentPools vector if needed
    if(componentId >= componentPools.size()){
        componentPools.resize(componentId + 1, nullptr);
    }

    // Add pool for given component type if it doesn't exist
    if(!componentPools[componentId]){
        Pool<T>* newComponentPool = new Pool<T>;
        componentPools[componentId] = newComponentPool;
    }

    // Get the pool of component values for the given component type
    Pool<T>* componentPool = Pool<T>(componentPools[componentId]);

    // Resize componentPool vector if needed
    if(entityId >= componentPool -> GetSize()){
        componentPool -> Resize(numEntities);
    }

    // Create a new component object of the type T, and forward the various parameters to the constructor
    T newComponent(forward<TArgs>(args)...);

    // Add the new component to the component pool list, using the entityId as index
    componentPool -> Set(entityId, newComponent);

    // Finally, change the component signature of the entity and set the component id on the bitset to 1
    entityComponentSignatures[entityId].set(componentId);
}

void Registry::Update(){
    
}
