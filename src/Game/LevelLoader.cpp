#include "./LevelLoader.h"
#include "./Game.h"
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

LevelLoader::LevelLoader() {
    
}

LevelLoader::~LevelLoader(){
    
}

void LevelLoader::LoadLevel(const std::unique_ptr<Registry>& registry, const std::unique_ptr<AssetStore>& assetStore, SDL_Renderer* renderer, int level) {
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
    Game::mapWidth = tileMap[0].size() * tileSize * tileScale;
    Game::mapHeight = tileMap.size() * tileSize * tileScale;

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
    radar.AddComponent<TransformComponent>(glm::vec2(Game::windowWidth - 74, 10.0), glm::vec2(1.0, 1.0), 0.0);
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
    label.AddComponent<TextLabelComponent>(glm::vec2(Game::windowWidth / 2 - 60, 10), "Game v1.0", "kitchensink_font", green);
}
