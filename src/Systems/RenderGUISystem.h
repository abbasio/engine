#pragma once

#include "../ECS/ECS.h"
#include <imgui/imgui.h>
#include <imgui/imgui_impl_sdl2.h>
#include <imgui/imgui_impl_sdl2.h>
#include <imgui/imgui_impl_sdlrenderer2.h>

class RenderGUISystem: public System {  
    public:  
        RenderGUISystem() = default;
        
        void Update() {
            ImGui_ImplSDLRenderer2_NewFrame();
            ImGui_ImplSDL2_NewFrame(); 
            ImGui::NewFrame();
            // Create an ImGui window
            if (ImGui::Begin("Spawn enemies")){
                // TODO: Create an ImGui window
            }
            ImGui::End();
            
            ImGui::Render();
            ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData());
        }

};
