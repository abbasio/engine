#include "ECS.h"
#include <vector>
#include "../Logger/Logger.h"

using namespace std;

//--------ENTITY
int Entity::GetId() const{
    return id;
}

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


void Registry::Update(){
    
}
