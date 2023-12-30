#include "ECS.h"
#include <vector>

int Entity::GetId() const{
    return id;
}
void System::AddEntityToSystem(Entity entity){
    entities.push_back(entity);
}
void System::RemoveEntityFromSystem(Entity entity){
    std::erase(entities, entity);
}
const Signature& System::GetComponentSignature() const{
    return componentSignature;
}
