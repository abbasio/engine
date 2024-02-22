#pragma once

#include "../Components/DamageComponent.h"
#include "../Components/HealthComponent.h"
#include "../Components/BoxColliderComponent.h"

#include "../ECS/ECS.h"
#include "../EventBus/EventBus.h"
#include "../Events/CollisionEvent.h"
class DamageSystem: public System {
    public:
        DamageSystem() {
            RequireComponent<BoxColliderComponent>();
        }
        
        void SubscribeToEvents(std::unique_ptr<EventBus>& eventBus) {
            eventBus -> SubscribeToEvent<CollisionEvent>(this, &DamageSystem::onCollision);
        }

        void onCollision(CollisionEvent& event) {
            Entity a = event.a;
            Entity b = event.b;
            
            if(a.BelongsToGroup("projectiles") && (b.HasTag("player") || b.BelongsToGroup("enemies"))) {
                onProjectileHit(a, b);
            }
            
            if(b.BelongsToGroup("projectiles") && (a.HasTag("player") || a.BelongsToGroup("enemies"))) {
                onProjectileHit(b, a);
            }
        }

        void onProjectileHit(Entity projectile, Entity unit) {
            auto projectileCollider = projectile.GetComponent<BoxColliderComponent>();
            auto projectileDamage = projectile.GetComponent<DamageComponent>();
            auto unitCollider = unit.GetComponent<BoxColliderComponent>();
            auto& unitHealth = unit.GetComponent<HealthComponent>();

            if (projectileCollider.damageLayer != unitCollider.damageLayer) {
                unitHealth.healthPercentage -= projectileDamage.damage; 
                if (unitHealth.healthPercentage <= 0) unit.Kill();
                projectile.Kill(); 
            }
        }

        void Update() {
        }
};
