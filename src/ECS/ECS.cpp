#include "ECS.h"
#include <vector>
#include "../Logger/Logger.h"

using namespace std;

//--------ENTITY
int Entity::GetId() const{
    return id;
}

//--------COMPONENT
int IComponent::nextId = 0;

//--------SYSTEM
void System::AddEntityToSystem(Entity entity){
    entities.push_back(entity);
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

    entityId = numEntities++;

    Entity entity(entityId);
    entitiesToBeAdded.insert(entity);

    Logger::Log("Entity created with id = " + to_string(entityId));
    
    return entity;
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

void Registry::Update(){
    
}