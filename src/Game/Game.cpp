#include <SDL2/SDL_image.h>
#include <SDL2/SDL.h>
#include <glm/glm.hpp>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_sdl2.h>
#include <imgui/imgui_impl_sdlrenderer2.h>
#include <fstream>
#include <sstream>

#include "../Components/ProjectileEmitterComponent.h"
#include "../Components/KeyboardControlComponent.h"
#include "../Components/CameraFollowComponent.h"
#include "../Components/TextLabelComponent.h"
#include "../Components/BoxColliderComponent.h"
#include "../Components/AnimationComponent.h"
#include "../Components/TransformComponent.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/HealthComponent.h"
#include "../Components/SpriteComponent.h"

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
#include "../ECS/ECS.h"
#include "Game.h"

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

void Game::LoadLevel(int level){
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

    // Add assets to the asset store
    assetStore -> AddTexture("chopper-image", "./assets/images/chopper-spritesheet.png"); 
    assetStore -> AddTexture("tank-right", "./assets/images/tank-panther-right.png");
    assetStore -> AddTexture("truck-right", "./assets/images/truck-ford-right.png");
    assetStore -> AddTexture("bullet-image", "./assets/images/bullet.png");
    assetStore -> AddTexture("tree-image", "./assets/images/tree.png");
    assetStore -> AddTexture("radar-image", "./assets/images/radar.png"); 
    assetStore -> AddTexture("tileset", "./assets/tilemaps/jungle.png");
   
    assetStore -> AddFont("kitchensink_font", "./assets/fonts/kitchensink.ttf", 12);

    // Create a 2D tilemap vector
    ifstream infile("./assets/tilemaps/jungle.map");
    vector<vector<int>> tileMap;
    string tileMapString;
    
    // Get each line of the map file
    while(getline(infile, tileMapString)){
        // Write each line into a stringstream
        stringstream ss(tileMapString);
        string tile;
        vector<int> row;
        // Get each value from the line, then push it into a 'row' vector
        while(getline(ss, tile, ',')){
            row.push_back(stoi(tile));
        }
        // Push each 'row' vector into the 2D tilemap vector
        tileMap.push_back(row);
    }

    int tileSetWidth = 10;
    int tileSize = 32;
    int tileScale = 2; 
    // Loop over the 2D tileMap vector and create a tile entity for each entry
    for(int i = 0; i < static_cast<int>(tileMap.size()); i++){
        for(int j = 0; j < static_cast<int>(tileMap[i].size()); j++){
            // Define the tile locations based on the indices 
            int tileXPos = j * tileSize * tileScale;
            int tileYPos = i * tileSize * tileScale;
            // Define sprite srcRect based on value
            int tileSetRow = floor(tileMap[i][j] / tileSetWidth);
            int tileSetColumn = (tileMap[i][j] - (tileSetRow * tileSetWidth));
            int srcRectX = tileSize * tileSetColumn;
            int srcRectY = tileSize * tileSetRow;

            Entity tile = registry -> CreateEntity();
            tile.Group("tiles");
            tile.AddComponent<TransformComponent>(glm::vec2(tileXPos, tileYPos), glm::vec2(tileScale, tileScale));
            tile.AddComponent<SpriteComponent>("tileset", tileSize, tileSize, 0, false, srcRectX, srcRectY); 
        }
    }
    
    // Set the map width (in pixels) based on the number of rows and columns in the tile map
    mapWidth = tileMap[0].size() * tileSize * tileScale;
    mapHeight = tileMap.size() * tileSize * tileScale;

    // Create entities and add components
    Entity chopper = registry -> CreateEntity();
    chopper.Tag("player");
    chopper.AddComponent<TransformComponent>(glm::vec2(300.0, 100.0), glm::vec2(1.0, 1.0), 0.0);
    chopper.AddComponent<ProjectileEmitterComponent>(glm::vec2(300.0, 300.0), 200, 10000, 10, false);
    chopper.AddComponent<SpriteComponent>("chopper-image", 32, 32, 2);
    chopper.AddComponent<RigidBodyComponent>(glm::vec2(0.0, 0.0));
    chopper.AddComponent<AnimationComponent>(2, 15, true);
    chopper.AddComponent<KeyboardControlComponent>(90);
    chopper.AddComponent<BoxColliderComponent>(32, 32, 1);
    chopper.AddComponent<CameraFollowComponent>();
    chopper.AddComponent<HealthComponent>(100);

    Entity radar = registry -> CreateEntity();
    radar.AddComponent<TransformComponent>(glm::vec2(windowWidth - 74, 10.0), glm::vec2(1.0, 1.0), 0.0);
    radar.AddComponent<SpriteComponent>("radar-image", 64, 64, 2, true);
    radar.AddComponent<RigidBodyComponent>(glm::vec2(0.0, 0.0));
    radar.AddComponent<AnimationComponent>(8, 5, true);
    
    Entity tank = registry -> CreateEntity();
    tank.Group("enemies");
    tank.AddComponent<TransformComponent>(glm::vec2(500.0, 500.0), glm::vec2(1.0, 1.0), 0.0);
    //tank.AddComponent<ProjectileEmitterComponent>(glm::vec2(0.0, 100.0), 2000, 10000, 33);
    tank.AddComponent<SpriteComponent>("tank-right", 32, 32, 1);
    tank.AddComponent<RigidBodyComponent>(glm::vec2(20.0, 0.0));
    tank.AddComponent<BoxColliderComponent>(32, 32, 2);
    tank.AddComponent<HealthComponent>(100);

    Entity truck = registry -> CreateEntity();
    truck.Group("enemies");
    truck.AddComponent<TransformComponent>(glm::vec2(10.0, 10.0), glm::vec2(1.0, 1.0), 0.0);
    truck.AddComponent<SpriteComponent>("truck-right", 32, 32, 1);
    truck.AddComponent<RigidBodyComponent>(glm::vec2(0.0, 0.0));
    truck.AddComponent<BoxColliderComponent>(32, 32, 2);
    truck.AddComponent<HealthComponent>(100);

    Entity treeA = registry -> CreateEntity();
    treeA.Group("obstacles");
    treeA.AddComponent<TransformComponent>(glm::vec2(600.0, 495.0), glm::vec2(1.0, 1.0), 0.0);
    treeA.AddComponent<SpriteComponent>("tree-image", 16, 32, 1);
    treeA.AddComponent<BoxColliderComponent>(16, 32, 0);
    
    Entity treeB = registry -> CreateEntity();
    treeB.Group("obstacles");
    treeB.AddComponent<TransformComponent>(glm::vec2(400.0, 495.0), glm::vec2(1.0, 1.0), 0.0);
    treeB.AddComponent<SpriteComponent>("tree-image", 16, 32, 1);
    treeB.AddComponent<BoxColliderComponent>(16, 32, 0);
    
    Entity label = registry -> CreateEntity();
    SDL_Color green = {0, 255, 0};
    label.AddComponent<TextLabelComponent>(glm::vec2(windowWidth / 2 - 60, 10), "Game v1.0", "kitchensink_font", green);
}

void Game::Setup(){
    LoadLevel(1);
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

