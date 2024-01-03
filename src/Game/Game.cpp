#include <SDL2/SDL_image.h>
#include <SDL2/SDL.h>
#include <iostream>

#include "../../libs/glm/glm.hpp"
#include "../Components/TransformComponent.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/SpriteComponent.h"
#include "../Systems/MovementSystem.h"
#include "../Systems/RenderSystem.h"
#include "../ECS/ECS.h"
#include "../Logger/Logger.h"
#include "Game.h"

Game::Game(){
    Logger::Log("Game constructor called!"); 
    isRunning = false;
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
    windowWidth = 800; 
    windowHeight = 600; 
    window = SDL_CreateWindow(
        NULL, 
        SDL_WINDOWPOS_CENTERED, 
        SDL_WINDOWPOS_CENTERED,
        windowWidth,
        windowHeight,
        SDL_WINDOW_BORDERLESS 
    );
    if (!window){
        Logger::Err("Error creating SDL window!"); 
        return;
    }
    renderer = SDL_CreateRenderer(
            window, 
            -1, 
            SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
        );
    if (!renderer){
        Logger::Err("Error creating SDL renderer!"); 
        return;
    }
    assetStore -> SetRenderer(renderer);
    SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
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
                break;  
        }
    }
}

void Game::LoadLevel(int level){
    // Add the systems that need to be processed in our game
    registry -> AddSystem<MovementSystem>();
    registry -> AddSystem<RenderSystem>();
    // Add assets to the asset store
    assetStore -> AddTexture("tank-right", "./assets/images/tank-panther-right.png");
    assetStore -> AddTexture("truck-right", "./assets/images/truck-ford-right.png");
    // Load tilemap
    // We need to load tilemap texture from ./assets/tilemaps/jungle.png
    // We need to load the file ./assets/tilemaps/jungle.map
    
    // Create entities
    Entity tank = registry -> CreateEntity();
    Entity truck = registry -> CreateEntity();
    // Add components
    tank.AddComponent<TransformComponent>(glm::vec2(10.0, 100.0), glm::vec2(1.0, 1.0), 0.0);
    tank.AddComponent<RigidBodyComponent>(glm::vec2(50.0, 0.0));
    tank.AddComponent<SpriteComponent>("tank-right");
    
    truck.AddComponent<TransformComponent>(glm::vec2(50.0, 100.0), glm::vec2(1.0, 1.0), 0.0);
    truck.AddComponent<RigidBodyComponent>(glm::vec2(0.0, 50.0));
    truck.AddComponent<SpriteComponent>("truck-right");


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
    
    // Update the registry to process entities that are waiting to be created/deleted
    registry -> Update(); 
}

void Game::Render(){
    SDL_SetRenderDrawColor(renderer, 21, 21, 21, 255);
    SDL_RenderClear(renderer);
        
    // Invoke all systems that need to render
    registry -> GetSystem<RenderSystem>().Update(renderer, assetStore);
  
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

