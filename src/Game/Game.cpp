#include <SDL2/SDL_image.h>
#include <SDL2/SDL.h>
#include <glm/glm.hpp>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_sdl2.h>
#include <imgui/imgui_impl_sdlrenderer2.h>

#include "../Systems/KeyboardMovementSystem.h"
#include "../Systems/ProjectileEmitSystem.h"
#include "../Systems/RenderColliderSystem.h"
#include "../Systems/CameraMovementSystem.h"
#include "../Systems/RenderHealthSystem.h"
#include "../Systems/RenderTextSystem.h"
#include "../Systems/RenderGUISystem.h"
#include "../Systems/AnimationSystem.h"
#include "../Systems/LifecycleSystem.h"
#include "../Systems/CollisionSystem.h"
#include "../Systems/MovementSystem.h"
#include "../Systems/DamageSystem.h"
#include "../Systems/RenderSystem.h"

#include "../Events/KeyReleasedEvent.h"
#include "../Events/KeyPressedEvent.h"
#include "../Logger/Logger.h"
#include "./LevelLoader.h"
#include "../ECS/ECS.h"
#include "./Game.h"

int Game::windowWidth;
int Game::windowHeight;
int Game::mapWidth;
int Game::mapHeight;

Game::Game(){
    isRunning = false;
    isDebug = false;
    
    // Instantiate unique pointers
    assetStore = std::make_unique<AssetStore>(renderer);
    registry = std::make_unique<Registry>(); 
    eventBus = std::make_unique<EventBus>(); 
    
    Logger::Log("Game constructor called!");
}

Game::~Game(){
    Logger::Log("Game destructor called!"); 
}

void Game::Initialize() {
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        Logger::Err("Error initializing SDL."); 
        return;
    }
    
    if (TTF_Init() != 0){
        Logger::Err("Error initializing SDL TTF");
        return;
    }

    SDL_DisplayMode displayMode;
    SDL_GetCurrentDisplayMode(0, &displayMode);
    
    windowWidth = displayMode.w; 
    windowHeight = displayMode.h;
    
    SDL_CreateWindowAndRenderer(windowWidth, windowHeight, SDL_WINDOW_BORDERLESS, &window, &renderer);
    if (!window || !renderer){
        Logger::Err("Error creating SDL Window or Renderer!");
    }
    
    SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
    assetStore -> SetRenderer(renderer);
    isRunning = true;
    
    // Initialize ImGui
    ImGui::CreateContext();
    ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer2_Init(renderer);
    // Initialize camera view with the entire screen area
    camera.x = 0;
    camera.y = 0;
    camera.w = windowWidth;
    camera.h = windowHeight;
}


void Game::ProcessInput(){
    SDL_Event sdlEvent;
    while(SDL_PollEvent(&sdlEvent)){
        // ImGui SDL input
        ImGui_ImplSDL2_ProcessEvent(&sdlEvent);
        ImGuiIO& io = ImGui::GetIO();

        int mouseX, mouseY;
        const int buttons = SDL_GetMouseState(&mouseX, &mouseY);

        io.MousePos = ImVec2(mouseX, mouseY);
        io.MouseDown[0] = buttons & SDL_BUTTON(SDL_BUTTON_LEFT);
        io.MouseDown[1] = buttons & SDL_BUTTON(SDL_BUTTON_RIGHT);

        // Handle core SDL events
        switch(sdlEvent.type){
            case SDL_QUIT:
                isRunning = false;
                break;  
            case SDL_KEYDOWN:
                if (sdlEvent.key.keysym.sym == SDLK_ESCAPE){
                    isRunning = false;
                }
                if (sdlEvent.key.keysym.sym == SDLK_F1){
                    isDebug = !isDebug;
                }
                eventBus -> EmitEvent<KeyPressedEvent>(SDL_GetKeyName(sdlEvent.key.keysym.sym)); 
                break; 
            case SDL_KEYUP:
               eventBus -> EmitEvent<KeyReleasedEvent>(SDL_GetKeyName(sdlEvent.key.keysym.sym));
               break;
        }
    }
}

void Game::Setup(){
    // Add the systems that need to be processed in our game
    registry -> AddSystem<KeyboardMovementSystem>();
    registry -> AddSystem<ProjectileEmitSystem>();
    registry -> AddSystem<RenderColliderSystem>();
    registry -> AddSystem<CameraMovementSystem>();
    registry -> AddSystem<RenderHealthSystem>();
    registry -> AddSystem<RenderTextSystem>();
    registry -> AddSystem<RenderGUISystem>();
    registry -> AddSystem<AnimationSystem>();
    registry -> AddSystem<CollisionSystem>();
    registry -> AddSystem<LifecycleSystem>();
    registry -> AddSystem<MovementSystem>();
    registry -> AddSystem<RenderSystem>();
    registry -> AddSystem<DamageSystem>();
    
    // Load the first level
    LevelLoader loader;
    loader.LoadLevel(registry, assetStore, renderer, 1);
}

void Game::Update(){
    // Comment the following two lines out if you want to uncap the framerate
    int timeToWait = MS_PER_FRAME - (SDL_GetTicks() - millisecsPreviousFrame);
    if (timeToWait > 0 && timeToWait <= MS_PER_FRAME) SDL_Delay(timeToWait);
    
    // Difference in ticks since last frame, converted to seconds
    double deltaTime = (SDL_GetTicks() - millisecsPreviousFrame) / 1000.0;
    
    millisecsPreviousFrame = SDL_GetTicks();
    
    // Reset all event handlers for current frame
    eventBus -> Reset();

    // Perform subscription events for all systems
    registry -> GetSystem<KeyboardMovementSystem>().SubscribeToEvents(eventBus); 
    registry -> GetSystem<ProjectileEmitSystem>().SubscribeToEvents(eventBus);
    registry -> GetSystem<MovementSystem>().SubscribeToEvents(eventBus);
    registry -> GetSystem<DamageSystem>().SubscribeToEvents(eventBus);
    
    // Invoke all systems that need to update
    registry -> GetSystem<CameraMovementSystem>().Update(camera);
    registry -> GetSystem<ProjectileEmitSystem>().Update(registry);
    registry -> GetSystem<CollisionSystem>().Update(eventBus);
    registry -> GetSystem<MovementSystem>().Update(deltaTime);
    registry -> GetSystem<LifecycleSystem>().Update();
    registry -> GetSystem<AnimationSystem>().Update();
    
    // Update the registry to process entities that are waiting to be created/deleted
    registry -> Update(); 
}

void Game::Render(){
    SDL_SetRenderDrawColor(renderer, 21, 21, 21, 255);
    SDL_RenderClear(renderer);
        
    // Invoke all systems that need to render
    registry -> GetSystem<RenderSystem>().Update(renderer, assetStore, camera);
    registry -> GetSystem<RenderTextSystem>().Update(renderer, assetStore, camera);
    registry -> GetSystem<RenderHealthSystem>().Update(renderer, assetStore, camera);
    if (isDebug){
        registry -> GetSystem<RenderColliderSystem>().Update(renderer, camera);
        
        registry -> GetSystem<RenderGUISystem>().Update(registry);
        // Start the ImGui frame
    } 
    SDL_RenderPresent(renderer);
}

void Game::Run(){
    Setup();
    while(isRunning){
        ProcessInput();
        Update();
        Render();
    }
}

void Game::Destroy(){
   ImGui_ImplSDLRenderer2_Shutdown();
   ImGui_ImplSDL2_Shutdown();
   ImGui::DestroyContext();

   SDL_DestroyRenderer(renderer);
   SDL_DestroyWindow(window);
   SDL_Quit(); 
}

