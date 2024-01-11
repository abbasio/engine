#include <vector>
#include <algorithm>
#include "ECS.h"
#include "../Logger/Logger.h"
#include "../Components/SpriteComponent.h"

using namespace std;

//--------ENTITY
int Entity::GetId() const{
    return id;
}

void Entity::Kill() {
    registry -> KillEntity(*this);
} 

//--------COMPONENT
int IComponent::nextId = 0;

//--------SYSTEM
void System::AddEntityToSystem(Entity entity){
    entities.push_back(entity);

    sort(entities.begin(), entities.end(), [](Entity a, Entity b){
                if (!a.HasComponent<SpriteComponent>() || !b.HasComponent<SpriteComponent>()) return false;
                return (a.GetComponent<SpriteComponent>().zIndex < b.GetComponent<SpriteComponent>().zIndex);
            });
}

void System::RemoveEntityFromSystem(Entity entity){
    erase(entities, entity);
}

const vector<Entity>& System::GetSystemEntities() const{
    return entities;
}

const Signature& System::GetComponentSignature() const{
    return componentSignature;
}

//--------REGISTRY
Entity Registry::CreateEntity(){
    int entityId;

    if (freeIds.empty()){
        // Create a new entity ID if we do not have any to reuse
        entityId = numEntities++;
        // Resize entityComponentSignatures vector as needed
        if (entityId >= static_cast<int>(entityComponentSignatures.size())){
            entityComponentSignatures.resize(entityId + 1);
        }
    } else {
        // Reuse entity ID if one is available
        entityId = freeIds.front();
        freeIds.pop_front();
    }

    Entity entity(entityId);
    // Set the entry's parent registry to the current registry
    entity.registry = this;
    entitiesToBeAdded.insert(entity);
    
    Logger::Log("Entity created with id = " + to_string(entityId));
    
    return entity;
}

void Registry::KillEntity(Entity entity){
    entitiesToBeKilled.insert(entity);
}

void Registry::AddEntityToSystems(Entity entity){
    const auto entityId = entity.GetId();

    // Get entity component signature
    const auto& entityComponentSignature = entityComponentSignatures[entityId];

    // Loop over all systems to get system component signatures
    for (auto& system: systems){
        const auto& systemComponentSignature = system.second -> GetComponentSignature();
        
        // Use the bitwise& operator to check if the signatures match
        bool isInterested = (entityComponentSignature & systemComponentSignature) == systemComponentSignature;
        
        // Add entity to the system if the signatures match
        if (isInterested){
            system.second -> AddEntityToSystem(entity);
        }
    }
}

void Registry::RemoveEntityFromSystems(Entity entity){
    for (auto system: systems){
        system.second -> RemoveEntityFromSystem(entity);
    }
}


void Registry::Update(){
    // Process entities that are waiting to be created 
    for (auto entity: entitiesToBeAdded){
        AddEntityToSystems(entity);   
    }
    entitiesToBeAdded.clear();

    // Process entities that are waiting to be killed
    for (auto entity: entitiesToBeKilled){
        RemoveEntityFromSystems(entity);
        entityComponentSignatures[entity.GetId()].reset();

        // Make entity ID available for reuse
        freeIds.push_back(entity.GetId());

    }
    entitiesToBeKilled.clear();
}
