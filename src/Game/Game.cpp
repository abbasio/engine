#include <SDL2/SDL_image.h>
#include <SDL2/SDL.h>
#include <iostream>
#include <iterator>
#include <fstream>
#include <algorithm>
#include <sstream>

#include "../../libs/glm/glm.hpp"
#include "../Components/BoxColliderComponent.h"
#include "../Components/AnimationComponent.h"
#include "../Components/TransformComponent.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/SpriteComponent.h"
#include "../Systems/RenderColliderSystem.h"
#include "../Systems/AnimationSystem.h"
#include "../Systems/CollisionSystem.h"
#include "../Systems/MovementSystem.h"
#include "../Systems/RenderSystem.h"
#include "../ECS/ECS.h"
#include "../Logger/Logger.h"
#include "Game.h"

Game::Game(){
    Logger::Log("Game constructor called!"); 
    isRunning = false;
    isDebug = false;
    registry = std::make_unique<Registry>(); 
    assetStore = std::make_unique<AssetStore>(renderer); 
}

Game::~Game(){
    Logger::Log("Game destructor called!"); 
}

void Game::Initialize(){
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        Logger::Err("Error initializing SDL."); 
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

    assetStore -> SetRenderer(renderer);
    SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
    isRunning = true;
}


void Game::ProcessInput(){
    SDL_Event sdlEvent;
    while(SDL_PollEvent(&sdlEvent)){
        switch(sdlEvent.type){
            case SDL_QUIT:
                isRunning = false;
                break;  
            case SDL_KEYDOWN:
                if (sdlEvent.key.keysym.sym == SDLK_ESCAPE){
                    isRunning = false;
                }
                if (sdlEvent.key.keysym.sym == SDLK_d){
                    isDebug = !isDebug;
                }
                break;  
        }
    }
}

void Game::LoadLevel(int level){
    // Add the systems that need to be processed in our game
    registry -> AddSystem<MovementSystem>();
    registry -> AddSystem<RenderSystem>();
    registry -> AddSystem<AnimationSystem>();
    registry -> AddSystem<CollisionSystem>();
    registry -> AddSystem<RenderColliderSystem>();

    // Add assets to the asset store
    assetStore -> AddTexture("chopper-image", "./assets/images/chopper.png"); 
    assetStore -> AddTexture("radar-image", "./assets/images/radar.png"); 
    assetStore -> AddTexture("tank-right", "./assets/images/tank-panther-right.png");
    assetStore -> AddTexture("truck-right", "./assets/images/truck-ford-right.png");
    assetStore -> AddTexture("tileset", "./assets/tilemaps/jungle.png");
    
    // Create a 2D tilemap vector
    vector<vector<int>> tileMap;
    ifstream infile("./assets/tilemaps/jungle.map");
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
            Entity tile = registry -> CreateEntity();
            // Define the tile locations based on the indices 
            int tileXPos = j * tileSize * tileScale;
            int tileYPos = i * tileSize * tileScale;
            tile.AddComponent<TransformComponent>(glm::vec2(tileXPos, tileYPos), glm::vec2(tileScale, tileScale));
            // Define sprite srcRect based on value
            int tileSetRow = floor(tileMap[i][j] / tileSetWidth);
            int tileSetColumn = (tileMap[i][j] - (tileSetRow * tileSetWidth));
            int srcRectX = tileSize * tileSetColumn;
            int srcRectY = tileSize * tileSetRow;
            tile.AddComponent<SpriteComponent>("tileset", 1, tileSize, tileSize, srcRectX, srcRectY);           
        }
    }
    
    // Create entities and add components
    Entity chopper = registry -> CreateEntity();
    chopper.AddComponent<TransformComponent>(glm::vec2(200.0, 10.0), glm::vec2(1.0, 1.0), 0.0);
    chopper.AddComponent<RigidBodyComponent>(glm::vec2(0.0, 0.0));
    chopper.AddComponent<SpriteComponent>("chopper-image", 2, 32, 32);
    chopper.AddComponent<AnimationComponent>(2, 15, true);
    chopper.AddComponent<BoxColliderComponent>(32, 32);
    
    Entity radar = registry -> CreateEntity();
    radar.AddComponent<TransformComponent>(glm::vec2(windowWidth - 74, 10.0), glm::vec2(1.0, 1.0), 0.0);
    radar.AddComponent<RigidBodyComponent>(glm::vec2(0.0, 0.0));
    radar.AddComponent<SpriteComponent>("radar-image", 2, 64, 64);
    radar.AddComponent<AnimationComponent>(8, 5, true);
    
    Entity tank = registry -> CreateEntity();
    tank.AddComponent<TransformComponent>(glm::vec2(500.0, 10.0), glm::vec2(1.0, 1.0), 0.0);
    tank.AddComponent<RigidBodyComponent>(glm::vec2(-90.0, 0.0));
    tank.AddComponent<SpriteComponent>("tank-right", 2);
    tank.AddComponent<BoxColliderComponent>(32, 32);

    Entity truck = registry -> CreateEntity();
    truck.AddComponent<TransformComponent>(glm::vec2(10.0, 10.0), glm::vec2(1.0, 1.0), 0.0);
    truck.AddComponent<RigidBodyComponent>(glm::vec2(50.0, 0.0));
    truck.AddComponent<SpriteComponent>("truck-right", 2);
    truck.AddComponent<BoxColliderComponent>(32, 32);
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
    
    // Invoke all systems that need to update
    registry -> GetSystem<MovementSystem>().Update(deltaTime);
    registry -> GetSystem<AnimationSystem>().Update();
    registry -> GetSystem<CollisionSystem>().Update(); 
    // Update the registry to process entities that are waiting to be created/deleted
    registry -> Update(); 
}

void Game::Render(){
    SDL_SetRenderDrawColor(renderer, 21, 21, 21, 255);
    SDL_RenderClear(renderer);
        
    // Invoke all systems that need to render
    registry -> GetSystem<RenderSystem>().Update(renderer, assetStore);
    if (isDebug) registry -> GetSystem<RenderColliderSystem>().Update(renderer);
  
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
   SDL_DestroyRenderer(renderer);
   SDL_DestroyWindow(window);
   SDL_Quit(); 
}

