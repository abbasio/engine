#pragma once

#include "../ECS/ECS.h"
#include <glm/glm.hpp>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_sdl2.h>
#include <imgui/imgui_impl_sdl2.h>
#include <imgui/imgui_impl_sdlrenderer2.h>

#include "../Components/TransformComponent.h"
#include "../Components/ProjectileEmitterComponent.h"
#include "../Components/SpriteComponent.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/BoxColliderComponent.h"
#include "../Components/HealthComponent.h"

class RenderGUISystem: public System {  
    public:  
        RenderGUISystem() = default;
        
        void Update(const std::unique_ptr<Registry>& registry) {
            ImGui_ImplSDLRenderer2_NewFrame();
            ImGui_ImplSDL2_NewFrame(); 
            ImGui::NewFrame();
            // Create an ImGui window
            if (ImGui::Begin("Spawn enemies")){
                static int sprite_index = 0;
                static const char* sprites[]{"tank-right", "truck-right"};

                static int enemyXPos = 0;
                static int enemyYPos = 0;
                static int enemyXScale = 1;
                static int enemyYScale = 1;
                static float enemyRotation = 0.0f;
                static int enemyXVelocity = 0;
                static int enemyYVelocity = 0;
                static int enemyHealth = 100;

                static bool useProjectileEmitter = false;
                static int projectileXVelocity = 0;
                static int projectileYVelocity = 0;
                static int projectileFrequency = 2000;
                static int projectileDuration = 5000;
                static int projectileDamage = 10;
                
                ImGui::Combo("enemy sprite", &sprite_index, sprites, IM_ARRAYSIZE(sprites));
                ImGui::InputInt("x position (pixels)", &enemyXPos);
                ImGui::InputInt("y position (pixels)", &enemyYPos);
                ImGui::InputInt("x scale multiplier", &enemyXScale);
                ImGui::InputInt("y scale multiplier", &enemyYScale);
                ImGui::SliderAngle("enemy rotation (degrees)", &enemyRotation);
                ImGui::Checkbox("use projectile emitter?", &useProjectileEmitter);
                if (useProjectileEmitter){
                    ImGui::InputInt("projectile x velocity (pixels per second)", &projectileXVelocity);
                    ImGui::InputInt("projectile y velocity (pixels per second)", &projectileYVelocity);
                    ImGui::InputInt("projectile frequency (ms)", &projectileFrequency);
                    ImGui::InputInt("projectile duration (ms)", &projectileDuration);
                    ImGui::InputInt("projectile damage", &projectileDamage);
                }
                ImGui::InputInt("health", &enemyHealth);

                // Button to create a new enemy
                if (ImGui::Button("Create Enemy")) {
                    // Create an entity
                    Entity enemy = registry -> CreateEntity();
                    enemy.Group("enemies");
                    enemy.AddComponent<TransformComponent>(glm::vec2(enemyXPos, enemyYPos), glm::vec2(enemyXScale, enemyYScale), enemyRotation * (180 / 3.14159265358979));
                    if (useProjectileEmitter) {
                        enemy.AddComponent<ProjectileEmitterComponent>(glm::vec2(projectileXVelocity, projectileYVelocity), projectileFrequency, projectileDuration, projectileDamage);
                    }
                    enemy.AddComponent<SpriteComponent>(sprites[sprite_index], 32, 32, 2);
                    enemy.AddComponent<RigidBodyComponent>(glm::vec2(enemyXVelocity, enemyYVelocity));
                    enemy.AddComponent<BoxColliderComponent>(32, 32, 2);
                    enemy.AddComponent<HealthComponent>(enemyHealth);

                }
            }
            ImGui::End();
            
            ImGui::Render();
            ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData());
        }

};
