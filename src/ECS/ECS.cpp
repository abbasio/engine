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

void Entity::Tag(const string& tag) {
    registry -> TagEntity(*this, tag);
}

bool Entity::HasTag(const string& tag) const {
    return registry -> EntityHasTag(*this, tag);
}

void Entity::Group(const string& group) {
    registry -> GroupEntity(*this, group);
}

bool Entity::BelongsToGroup(const string& group) const {
    return registry -> EntityBelongsToGroup(*this, group);
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

// Manage entities
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

// Manage entity groups and tags
void Registry::TagEntity(Entity entity, const string& tag) {
    entityPerTag.emplace(tag, entity);
    tagPerEntity.emplace(entity.GetId(), tag);
}

bool Registry::EntityHasTag(Entity entity, const string& tag) const {
    // Check to see if the entity exists in the map of EntityId -> tag
    if (tagPerEntity.find(entity.GetId()) == tagPerEntity.end()) return false;
    // If so, check if the tag associated with it matches the input
    return entityPerTag.find(tag) -> second == entity;
}

Entity Registry::GetEntityByTag(const string& tag) const {
    return entityPerTag.at(tag);
}

void Registry::RemoveEntityTag(Entity entity) {
    auto taggedEntity = tagPerEntity.find(entity.GetId());
    if (taggedEntity != tagPerEntity.end()) {
        auto tag = taggedEntity -> second;
        entityPerTag.erase(tag);
        tagPerEntity.erase(taggedEntity);
    }
}

void Registry::GroupEntity(Entity entity, const string& group) {
    entitiesPerGroup.emplace(group, set<Entity>());
    entitiesPerGroup[group].emplace(entity);
    groupPerEntity.emplace(entity.GetId(), group);
}

bool Registry::EntityBelongsToGroup(Entity entity, const string& group) const {
    // Check to see if the entity exists in the map of EntityId -> group
    if (groupPerEntity.find(entity.GetId()) == groupPerEntity.end()) return false;
    // If so, check if the group associted with it matches the input
    return groupPerEntity.find(entity.GetId()) -> second == group;
}

vector<Entity> Registry::GetEntitiesByGroup(const string& group) const {
    auto& setOfEntities =  entitiesPerGroup.at(group);
    return vector<Entity>(setOfEntities.begin(), setOfEntities.end());
}

void Registry::RemoveEntityGroup(Entity entity) {
    // Check to see if the entity is in any groups 
    auto groupedEntity = groupPerEntity.find(entity.GetId());
    if (groupedEntity != groupPerEntity.end()) {
        // If the entity is in a group, access the list of entities for that group
        auto group = entitiesPerGroup.find(groupedEntity -> second);
        if (group != entitiesPerGroup.end()) {
            // Find the entity from that list of entities and erase it
            auto entityInGroup = group -> second.find(entity);
            if (entityInGroup != group -> second.end()) {
                group -> second.erase(entityInGroup);
            }
        }
        // Erase the group from the entity
        groupPerEntity.erase(groupedEntity);
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
        // Remove entity from component pools
        for (auto pool: componentPools) {
            pool -> RemoveEntityFromPool(entity.GetId());
        }

        RemoveEntityFromSystems(entity);
        entityComponentSignatures[entity.GetId()].reset();

        // Make entity ID available for reuse
        freeIds.push_back(entity.GetId());
        
        // Remove entity from group/tag maps
        RemoveEntityTag(entity);
        RemoveEntityGroup(entity);
    }
    entitiesToBeKilled.clear();
}
