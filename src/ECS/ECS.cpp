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

void Registry::Update(){
    
}
