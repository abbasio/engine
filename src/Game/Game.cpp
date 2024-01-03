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
    isRunning = false;
    registry = std::make_unique<Registry>(); 
    Logger::Log("Game constructor called!"); 
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


void Game::Setup(){
    // Add the systems that need to be processed in our game
    registry -> AddSystem<MovementSystem>();
    registry -> AddSystem<RenderSystem>(); 
    // Create entities
    Entity wideBoy = registry -> CreateEntity();
    Entity tallBoy = registry -> CreateEntity();
    // Add components
    wideBoy.AddComponent<TransformComponent>(glm::vec2(10.0, 100.0), glm::vec2(1.0, 1.0), 0.0);
    wideBoy.AddComponent<RigidBodyComponent>(glm::vec2(50.0, 0.0));
    wideBoy.AddComponent<SpriteComponent>(50, 10);
    
    tallBoy.AddComponent<TransformComponent>(glm::vec2(50.0, 100.0), glm::vec2(1.0, 1.0), 0.0);
    tallBoy.AddComponent<RigidBodyComponent>(glm::vec2(0.0, 50.0));
    tallBoy.AddComponent<SpriteComponent>(10, 50);

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
    registry -> GetSystem<RenderSystem>().Update(renderer);
  
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

